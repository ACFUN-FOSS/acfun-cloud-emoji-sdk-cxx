/**
 * Copyright 2025 ACFUN-FOSS (Chih-hao Su).
 *
 * 本文件是 acfun-cloud-emoji-sdk-cxx 的一部分。
 *
 * acfun-cloud-emoji-sdk-cxx 是自由软件：你可以再分发之和/或依照由自由软件基金会发布的 GNU Affero 通用公共许可证修改之，无论是版本 3 许可证，还是（按你的决定）任何以后版都可以。
 *
 * 发布 acfun-cloud-emoji-sdk-cxx 是希望它能有用，但是并无保障；甚至连可销售和符合某个特定的目的都不保证。请参看 GNU Affero 通用公共许可证，了解详情。
 *
 * 你应该随程序获得一份 GNU Affero 通用公共许可证的复本。如果没有，请看 <https://www.gnu.org/licenses/>。
 */

#include "acfun_cloud_emoji.hxx"
#include <regex>
#include <chrono>
#include <format>
#include <ranges>
#include <iostream>
#include <algorithm>
#include <rfl/json.hpp>
#include <rfl.hpp>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

namespace stdr = std::ranges;
namespace stdv = std::views;


namespace AcfunFoss::AcfunCloudEmoji
{

NetError::NetError(std::string_view explain, std::string_view url, std::string_view errmsg)
	: std::runtime_error{
		std::format("NetError: {}. (url: {}, errmsg: {})", explain, url, errmsg)
	} { }

ParsingContentError::ParsingContentError(std::string_view explain)
	: std::runtime_error{
		std::format("ParsingContentError: {}", explain)
	} { }

NoSuchUserError::NoSuchUserError(std::string_view uid)
	: std::runtime_error{
		std::format("NoSuchUserError: {}", uid)
	} {}

NoEmojiArticleFoundError::NoEmojiArticleFoundError(std::string_view uid)
	: std::runtime_error{
		std::format("NoEmojiArticleFoundError: {}", uid)
	} {}

static NetError makeNetErrWithResp(std::string_view explain, const cpr::Response &resp) {
	return NetError{ explain, resp.url.str(), resp.error.message };
}

static cpr::Response getAcfunWebpage(std::string_view url) {
	return cpr::Get(
		cpr::Url{ url },
		cpr::Header{
			{ "User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/126.0.0.0 Safari/537.36" }
	});
}

static long long getTimestamp() {
	using namespace std::chrono;

	return duration_cast<milliseconds>(
		system_clock::now().time_since_epoch()
	).count();
}

// p.s. 所有正则表达式设为 static 独立变量，避免重复编译

std::string getEmojiArticleID(std::string_view uid) {
	if (uid.empty())
		throw std::invalid_argument("uid cannot be empty");

	// 1. 构造文章列表接口 URL
	using namespace std::chrono;

	auto url = std::format(
		"https://www.acfun.cn/u/{0}?quickViewId=ac-space-article-list&ajaxpipe=1&type=article&order=newest&page=1&pageSize={1}&t={2}reqId={3}",
		uid,
		100,
		getTimestamp(),
		1
	);

	// 2. 发起 HTTP GET
	const auto res = getAcfunWebpage(url);
	if (res.error)
		throw makeNetErrWithResp("Error getting articles list", res);

	// 3. 去掉尾部注释 /*…*/
	static const std::regex deCommentRegex{ R"(/\*.*?\*/)" };

	// 成功会返回一个 JSON 对象，里面有一个 html 字段，字段值是 HTML 代码
	// 失败会返回一个 HTML 页面，里面包括 <title>出错啦！.....
	const auto articlesApiResp = std::regex_replace(res.text, deCommentRegex, "");

	if (articlesApiResp.contains("<title>出错啦！"))
		throw NoSuchUserError{ uid };

	nlohmann::json list = nlohmann::json::parse(articlesApiResp);
	if (!list.contains("html") || !list["html"].is_string())
		throw ParsingContentError{ "No html field in articles list response, or html field is not a string" };

	const auto articlesListHtml = list["html"].get<std::string>();


	// 5. 从 html 中提取直播间表情的文章 ID (acXXXX)
	static const std::regex linkRegex{
		R"x(<a[^>]*\shref="/a/(ac\d+)"[^>]*title=["'][^"']*直播间表情)x",
		std::regex::ECMAScript
	};
	std::smatch m;
	if (!std::regex_search(articlesListHtml, m, linkRegex))
		throw NoEmojiArticleFoundError{ uid };

	const auto articleId = m[1].str();
	static const std::regex articleIdRegex{ R"(ac\d+)" };
	if (!std::regex_match(articleId, articleIdRegex))
		throw ParsingContentError{ std::format("Extracted article ID is not valid: {}", articleId) };


	return articleId;
}

std::unordered_map<std::string, std::string> getEmojiMap(std::string_view uid) {
	const auto emojiArtId = getEmojiArticleID(uid);

	// 1. 获取文章页面
	const auto emojiArtHtml = getAcfunWebpage(
		"https://www.acfun.cn/a/" + emojiArtId
	);
	if (emojiArtHtml.error)
		throw makeNetErrWithResp("Error getting emoji article page", emojiArtHtml);


	// 2. 从页面中提取 content 字段（JSON 字符串内部的 HTML）
	// 这里和 C# 的不一样，因为 C# 版的正则表达式在 C++ 里会导致栈溢出
	static const std::regex contentRegex{
		R"x("content":\s*"(.*)"\}\]\,)x",
		std::regex::ECMAScript
	};
	std::smatch m1;
	if (!std::regex_search(emojiArtHtml.text, m1, contentRegex))
		throw ParsingContentError{ "Cannot find content field in article page" };

	const auto rawContent = m1[1].str();

	// 3. 移除 p span 等标签干扰
	//static const std::regex dePAndSpanRegex{
	//	R"x(<(?!img\b)[^>]+>)x"
	//};
	//rawContent = std::regex_replace(rawContent, dePAndSpanRegex, "");
	// 事实证明这一步没什么用


	// 4. 收集 [name] -> url
	static const std::regex emotionRegex{
		R"x(\[(.*?)\].*?<img[^>]*\s+src=\\["']([^"']+)\\["'])x"
	};
	
	std::unordered_map<std::string, std::string> ret;
	for (std::sregex_iterator it{ rawContent.begin(), rawContent.end(), emotionRegex },
		end{};
		it != end; ++it) {
		//std::cout << it->str(1) << it->str(2) << '\n';
		ret[it->str(1)] = it->str(2);
	}

	//std::cout << rawContent << std::endl;
	return ret;
}

StdEmojiFormat getEmojiStruct(std::string_view uid) {
	return{
		.uid = std::string{ uid },
		.time = std::format("{:%FT%TZ}", std::chrono::system_clock::now()),
		.timestamp = getTimestamp(),

		// 云表情规范化格式要求表情名用中括号包裹
		.emotions = getEmojiMap(uid)
			| stdv::transform([](auto &&kv) {
				return std::pair{ "[" + kv.first + "]", kv.second };
			})
			| stdr::to<std::unordered_map<std::string, std::string>>()
	};
}

std::string getEmojiJson(std::string_view uid) {
	return rfl::json::write(getEmojiStruct(uid));
}

//////////////////////////////////
// 协程 API
//////////////////////////////////
static std::shared_ptr<coro::executor> uiThreadExecutor;
static coro::runtime *runtime;

void setupCoroEnv(coro::runtime &runtime, const Rc<coro::executor> &executor) {
	AcfunCloudEmoji::runtime = &runtime;
	uiThreadExecutor = executor;
}

static coro::executor &getWorkerThreadsExecutor() {
	assert(runtime && "runtime must be setted. Did you call `setupCoroEnv`?");
	static auto internalworkerThreadsExecutor = runtime->thread_pool_executor();
	return *internalworkerThreadsExecutor.get();
}

coro::lazy_result<StdEmojiFormat> getEmojiStructCoro(std::string_view uid) {
	assert(uiThreadExecutor && "uiThreadExecutor must be setted");
	// 1. 切换到后台线程池执行耗时操作
	co_await coro::resume_on(getWorkerThreadsExecutor());

	auto ret = getEmojiStruct(uid);

	// 2. 操作完成，切换回主线程
	co_await coro::resume_on(uiThreadExecutor);
	co_return ret;
}

coro::lazy_result<std::string> getEmojiJsonCoro(std::string_view uid) {
	assert(uiThreadExecutor && "uiThreadExecutor must be setted");
	// 1. 切换到后台线程池执行耗时操作
	co_await coro::resume_on(getWorkerThreadsExecutor());

	auto ret = getEmojiJson(uid);

	// 2. 操作完成，切换回主线程
	co_await coro::resume_on(uiThreadExecutor);
	co_return ret;
}

}

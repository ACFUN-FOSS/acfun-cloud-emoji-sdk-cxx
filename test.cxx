#include <string>
#include <iostream>
#include <print>
#include <ranges>
#include <array>
#include <algorithm>
#include <thread>
#include <chrono>
#include <gtest/gtest.h>
#include <concurrencpp/concurrencpp.h>
#include "acfun_cloud_emoji.hxx"

namespace stdr = std::ranges;
namespace stdv = std::views;
namespace coro = concurrencpp;

using namespace std::chrono_literals;
using namespace AcfunFoss::AcfunCloudEmoji;

// getEmojiArticleID - valid UID
TEST(AcfunCloudEmojiTest, GetEmojiArticleID_ValidUID) {
	// 10845128 是特里羊羊 Terry
	auto articleId = getEmojiArticleID("10845128");
	std::println("Article ID: {}", articleId);
	EXPECT_EQ(articleId, "ac47756774");
}

// getEmojiArticleID - invalid UID
TEST(AcfunCloudEmojiTest, GetEmojiArticleID_InvalidUID) {
	EXPECT_THROW(getEmojiArticleID("nonexistent_uid_123456"), NoSuchUserError);
}

// getEmojiMap - valid UID
TEST(AcfunCloudEmojiTest, GetEmojiMap_ValidUID) {
	auto emojiMap = getEmojiMap("10845128");
	std::println("Emoji count: {}", emojiMap.size());
	EXPECT_FALSE(emojiMap.empty());

	for (const auto &[name, url] : emojiMap) {
		std::println("{} -> {}", name, url);
	}

	bool allEmojiAreDesired = stdr::all_of(
		emojiMap | stdv::keys,
		[&](auto const &key) {
		return stdr::any_of(
			std::array{ "蛇年AC娘_威胁", "蛇年AC娘_亲亲", "蛇年AC娘_吃瓜" },
			[&](auto const &allowedKey) {
			return allowedKey == key;
		}
		);
	}
	);

	EXPECT_TRUE(allEmojiAreDesired);
}

// getEmojiMap - invalid UID
TEST(AcfunCloudEmojiTest, GetEmojiMap_InvalidUID) {
	EXPECT_THROW(getEmojiMap("nonexistent_uid_123456"), NoSuchUserError);
}

// getEmojiJson - valid UID
TEST(AcfunCloudEmojiTest, GetEmojiJson_ValidUID) {
	auto jsonStr = getEmojiJson("10845128");
	std::println("JSON Output:\n{}", jsonStr);
	EXPECT_TRUE(jsonStr.contains("\"uid\""));
	EXPECT_TRUE(jsonStr.contains("\"emotions\""));
}

// getEmojiJson - invalid UID
TEST(AcfunCloudEmojiTest, GetEmojiJson_InvalidUID) {
	EXPECT_THROW(getEmojiJson("nonexistent_uid_123456"), NoSuchUserError);
}

TEST(AcfunCloudEmojiTest, GetEmojiJsonCoro_ValidUID) {
	// 这个测试模拟 GUI 程序使用本 SDK 的协程 API 的场合
	concurrencpp::runtime runtime;
	auto uiThreadExecutor = runtime.make_manual_executor();

	setupCoroEnv(runtime, uiThreadExecutor);

	bool resultGetted = false;
	for (int i : stdv::iota(1, 100)) {
		std::println("处理 UI 事件 / 绘制 UI");
		std::this_thread::sleep_for(0.3s);

		// 第三「帧」时程序开始获取表情
		if (i == 3) {
			[&]() -> coro::result<void> {
				auto threadBefore = std::this_thread::get_id();
				std::println("开始请求");
				auto emojiJson = co_await getEmojiJsonCoro("10845128");
				std::println("结果：{}", emojiJson);
				auto threadAfter = std::this_thread::get_id();
				EXPECT_EQ(threadBefore, threadAfter);
				resultGetted = true;
			}();
		}

		uiThreadExecutor->loop_once();
		if (resultGetted) {
			SUCCEED();
			return;
		}
	}
	FAIL();
}


int main(int argc, char **argv) {
#ifdef WIN32
	system("chcp 65001");
#endif
	::testing::InitGoogleTest(&argc, argv);
	auto gret = RUN_ALL_TESTS();

	std::println("所有测试结束");

	return gret;
}

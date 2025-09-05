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

#ifndef __H_ACFUN_CLOUD_EMOJI
#define __H_ACFUN_CLOUD_EMOJI

#include <string>
#include <string_view>
#include <stdexcept>
#include <unordered_map>
#include <concurrencpp/concurrencpp.h>
#include "acfun_cloud_emoji_sdk_cxx_export.h"

namespace AcfunFoss::AcfunCloudEmoji
{

/**
 * @brief 云表情规范化 Emoji 格式
 */
struct StdEmojiFormat
{
	std::string uid;
	std::string time;
	long long timestamp;
	std::unordered_map<std::string, std::string> emotions;
};

/**
 * @brief 网络错误
 */
class ACEAPI NetError : public std::runtime_error
{
public:
	NetError(std::string_view explain, std::string_view url, std::string_view errmsg);
};

/**
 * @brief 解析内容错误
 */
class ACEAPI ParsingContentError : public std::runtime_error
{
public:
	ParsingContentError(std::string_view explain);
};


/**
 * @brief 无指定用户错误
 */
class ACEAPI NoSuchUserError : public std::runtime_error
{
public:
	NoSuchUserError(std::string_view uid);
};

/**
 * @brief 指定用户无表情包文章错误
 */
class ACEAPI NoEmojiArticleFoundError : public std::runtime_error
{
public:
	NoEmojiArticleFoundError(std::string_view uid);
};

/**
 * @brief 获取某用户的表情包文章 ID
 * 
 *（表情包文章规范：https://github.com/MiegoLive/AcfunCloudEmoji/blob/main/specs/article-template.md）
 * 
 *（示例：https://www.acfun.cn/a/ac47756774）
 * 
 * @param uid uid
 * @return 表情包文章 ID
 */
ACEAPI std::string getEmojiArticleID(std::string_view uid);

/**
 * @brief 获取某用户定义的云表情
 * @param uid UID
 * @return 表情名称到 URL 的映射
 */
ACEAPI std::unordered_map<std::string, std::string> getEmojiMap(std::string_view uid);

/**
 * @brief 获取某用户定义的云表情
 * @param uid UID
 * @return AcFun 云表情 规范化结构体
 */
ACEAPI StdEmojiFormat getEmojiStruct(std::string_view uid);


/**
 * @brief 获取某用户定义的云表情
 * @param uid UID
 * @return AcFun 云表情 规范化 JSON 字符串
 */
ACEAPI std::string getEmojiJson(std::string_view uid);


//////////////////////////////////
// 协程 API
// 好像提供协程 API 意义不大。因为应用程序貌似基本上只在连接到特定直播间时需要加载一次
// 表情列表，这是非频繁操作，而且耗时也很短，卡一下没人在意。
//////////////////////////////////
namespace coro = concurrencpp;
template <typename T>
using Rc = std::shared_ptr<T>;

ACEAPI void setupCoroEnv(coro::runtime &runtime, const Rc<coro::executor>& executor);

/**
 * @brief 获取某用户定义的云表情（异步）
 * @param uid UID
 * @return AcFun 云表情 规范化结构体
 */
ACEAPI coro::lazy_result<StdEmojiFormat> getEmojiStructCoro(std::string_view uid);


/**
 * @brief 获取某用户定义的云表情（异步）
 * @param uid UID
 * @return AcFun 云表情 规范化 JSON 字符串
 */
ACEAPI coro::lazy_result<std::string> getEmojiJsonCoro(std::string_view uid);

}

#endif

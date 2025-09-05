# acfun-cloud-emoji-sdk-cxx
> （请先[加入 ACFUN-FOSS 官方 VCPKG registry 到你的项目](https://github.com/ACFUN-FOSS/official-vcpkg-registry/tree/master)。）
```sh
vcpkg add port acfun-cloud-emoji-sdk-cxx
```
```cmake
find_package(acfun_cloud_emoji_sdk_cxx CONFIG REQUIRED)
target_link_libraries(myapp PRIVATE acfunfoss::acfun_cloud_emoji_sdk_cxx)
```
```c++
using namespace AcfunFoss;
auto stdJson = AcfunCloudEmoji::getEmojiJson("10845128");
auto stdStruct = AcfunCloudEmoji::getEmojiStruct("10845128");
```

[AcFun 直播间「云表情」企划](https://github.com/MiegoLive/AcfunCloudEmoji/) C++ SDK。从用户文章中提取云表情数据，并输出为结构体或 JSON。

| 同步 | 异步 | 协程 |
|-------|-------|-------|
| ⭕ | ❌ | ⭕ |

  - 获取指定 UID 的「直播间表情」文章 ID
  - 解析文章内容，生成表情名称到图片 URL 的映射表
  - 组装为标准化结构体 `StdEmojiFormat` 与 JSON 字符串
  - 可选择以 `concurrencpp` 协程 API 非阻塞获取结果





## API 一览（`acfun_cloud_emoji.hxx`）

### 同步 API：
```cxx
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
```

### 协程 API（concurrencpp）：
```cxx
/**
 * @brief 设置协程环境
 * @param runtime 协程运行时
 * @param executor ui 线程或主线程的协程执行器
 */
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

```

### 异常：
  - `NetError`：网络请求失败
  - `ParsingContentError`：响应内容解析失败
  - `NoSuchUserError`：UID 不存在
  - `NoEmojiArticleFoundError`：该 UID 未找到直播间表情文章


## 使用示例

### 同步使用

```cpp
#include <iostream>
#include "acfun_cloud_emoji.hxx"

using namespace AcfunFoss::AcfunCloudEmoji;

int main() {
    auto json = getEmojiJson("10845128");
    // ...
}
```

### 协程使用

```cpp
#include <concurrencpp/concurrencpp.h>
#include "acfun_cloud_emoji.hxx"

using namespace AcfunFoss::AcfunCloudEmoji;
namespace coro = concurrencpp;

coro::runtime runtime;
auto uiThreadExecutor = runtime.make_manual_executor();

int main() {
    setupCoroEnv(runtime, uiThreadExecutor);
}

void myWindowOnGetCloudEmojiBtn() {
    [&](){
        [=]() -> coro::result<void> {
            auto json = co_await getEmojiJsonCoro("10845128");
            // .....
        }();
    }();
}

void myWindowOnDraw(){
    uiThreadExecutor->loop_once();
}

```



## 测试

```bash
mkdir BUILD && cd BUILD
cmake .. --preset default
cmake --build
```

项目内提供 `test.cxx`，涵盖同步/协程 API 与错误场景。



## 授权

本项目以 **AGPL-3.0-or-later** 授权。详见 `LICENSE`。

---

## 参考与规范

- 云表情文章规范：`https://github.com/MiegoLive/AcfunCloudEmoji/blob/main/specs/article-template.md`
- 示例文章：`https://www.acfun.cn/a/ac47756774`


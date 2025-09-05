Copyright 2025 ACFUN-FOSS (Chih-hao Su).

本文件是 acfun-cloud-emoji-sdk-cxx 的一部分。

acfun-cloud-emoji-sdk-cxx 是自由软件：你可以再分发之和/或依照由自由软件基金会发布的 GNU Affero 通用公共许可证修改之，无论是版本 3 许可证，还是（按你的决定）任何以后版都可以。

发布 acfun-cloud-emoji-sdk-cxx 是希望它能有用，但是并无保障；甚至连可销售和符合某个特定的目的都不保证。请参看 GNU Affero 通用公共许可证，了解详情。

你应该随程序获得一份 GNU Affero 通用公共许可证的复本。如果没有，请看 <https://www.gnu.org/licenses/>。

---
---


AcFun 直播间云表情企划，特里羊羊 Terry 发起，旨在以第三方方案，为直播间提供定义、发送、显示表情的能力，简介如下：

====================================
 简介始
====================================
# AcFun 直播间“云表情”社区企划

> **让 AcFun 直播间的弹幕也能“斗图”！**  
> 这是一个由社区驱动的开源项目，旨在通过一种巧妙、轻量且对主播与观众都友好的方式，为 AcFun 网页直播引入“表情包弹幕”功能。

我已了解，[🚀快速开始](#-快速开始)

## 📌 项目缘起

AcFun 网页直播目前不支持主播上传自定义表情包，观众无法直接发送“图片弹幕”。  
为了解决这个问题，我们提出一种**基于主播文章**的“云表情”方案，无需平台改造，利用现有生态即可实现。

## 🎯 核心思路

我们的方案分为三步：

1.  **主播端：文章即仓库**  
    主播只需发布一篇**公开文章**，标题包含“直播间表情”，并在正文中按规范贴上**关键词+表情包图片**。  
    例如： `[蛇年AC娘_威胁]` 后面紧跟对应的表情包图片。

2.  **社区端：SDK 与工具**  
    我们提供多种语言的 **SDK** 和即用型工具，自动抓取主播的“表情包文章”，解析为 `{关键词: 图片URL}` 的字典。

3.  **用户端：无缝体验**  
    *   **主播**：使用 **AcFun 直播工具箱 MKII**、**点心 Chat** 等弹幕工具，加载字典后即可实现关键词触发“表情包弹幕”。
    *   **观众**：通过 **浏览器油猴脚本**，在弹幕输入框旁边获得一个**可视化表情包面板**，点击即可发送对应关键词，触发主播端的表情包展示。

## 📋 参与规范

为了让所有工具都能“说同一种语言”，我们制定了以下规范：

### 1. 主播文章规范

| 项目 | 规范 |
|------|------|
| **标题** | 必须包含 `直播间表情` 五个字，建议格式：`【直播间表情】你的直播间名/主题` |
| **关键词** | 用**英文方括号 `[]`** 包裹，如 `[AC娘_比心]` |
| **图片** | 关键词后的**第一张图片**将被视为该关键词对应的表情包 |
| **示例** | `[蛇年AC娘_威胁]`<br>![图片](https://imgs.aixifan.com/newUpload/10845128_59c608a89e6940878023087e5182cd6e.gif) |

> **💡 小贴士：** 建议将文章置顶，方便新观众快速了解表情包用法。

### 2. SDK 数据格式规范

所有 SDK 最终输出一个**字典（JSON 格式）**，结构如下：

```json
{
  "uid": "123456",
  "time": "2025-08-26T08:13:51Z",
  "timestamp": 1756196031,
  "emotions": {
    "[蛇年AC娘_威胁]": "https://cdn.aixifan.com/ac233.png",
    "[AC娘_比心]": "https://cdn.aixifan.com/ac666.png"
  }
}
```
====================================
== 简介终
====================================

以下是 C# SDK 实现。请把这个 SDK 翻译成 C++，要求：
- 使用现代 C++，C++23 标准。
- 确保符合 C++ 核心指导方针（C++ Core Guidelines），包括但不限于：
  - 避免 C 语言指针，使用引用、智能指针、`std::optional<T>` 代替。
  - 避免 C 语言字符串以及数组，使用 `std::string` 和其他 STL 容器（如 `std::vector`, `std::map`）。
  - 避免不必要地使用动态内存。
  - 使用范围 for 循环和标准算法处理集合。
  - 使用异常处理机制处理错误，而非返回错误码。遇到不可处理之情况，应尽早、尽精确地抛出异常以报告，而不是一声不吭地返回一个空值。
  - 使用 `const` 修饰不可变变量和成员函数，确保代码的可读性和安全性。
  - 避免使用全局变量，使用类或命名空间封装相关功能。
  - 必要时使用 GSL (Guidelines Support Library) 库。
- 为实现功能，使用两个功能库： cpr 进行网络访问，nlohmann/json 来处理 json。
- 严格禁止使用任何平台、操作系统相关的 API，只允许使用 C++ 标准库和上述两个功能库。
- 分头文件与源文件。且避免在头文件中引入用户不需要用到的头文件。
- 不要一对一地翻译 C# 代码，而是要用 C++ 的习惯和风格来实现同样的功能，保证可读性和易用性。

```cs
using System.Text.Json;
using System.Text.Json.Nodes;
using System.Text.RegularExpressions;

namespace AcfunCloudEmoji;

public static class Helper
{
    private static readonly HttpClient Http = new()
    {
        DefaultRequestHeaders =
        {
            { "User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/126.0.0.0 Safari/537.36" }
        }
    };
    
    private const string ArticleListUrl =
        "https://www.acfun.cn/u/{0}?quickViewId=ac-space-article-list&ajaxpipe=1&type=article&order=newest&page=1&pageSize={1}&t={2}reqId={3}";
    private const string ArticleUrl = "https://www.acfun.cn/a/";
    
    /// <summary>
    /// 根据用户 uid 拉取其直播间表情包，返回规范化 JSON。
    /// </summary>
    public static async Task<string> GetEmotionsJsonAsync(string uid)
    {
        if (string.IsNullOrWhiteSpace(uid))
            throw new ArgumentException("uid 不能为空");

        // 1. 找文章 id
        // 毫秒时间戳
        var timestamp = (int)(DateTimeOffset.Now.ToUnixTimeMilliseconds() / 1000);
        var articleListJsonString = await Http.GetStringAsync(
            string.Format(ArticleListUrl, uid, 100, timestamp, 1));
        // 去除末尾的注释
        articleListJsonString = Regex.Replace(articleListJsonString, @"/\*.*?\*/", "");
        var articleListJson = JsonSerializer.Deserialize<JsonObject>(articleListJsonString);
        var htmlString = articleListJson?["html"]?.ToString();
        if (string.IsNullOrWhiteSpace(htmlString)) return BuildJson(uid, new JsonObject());
        var m = Regex.Match(htmlString, @"<a[^>]*\shref=""/a/(ac\d+)""[^>]*title=[""'][^""']*直播间表情");
        if (!m.Success) return BuildJson(uid, new JsonObject());
        var acId = m.Groups[1].Value;

        // 2. 拿文章正文
        var articleHtml = await Http.GetStringAsync($"{ArticleUrl}{acId}");

        // 2.1 提取 content
        var contentMatch = Regex.Match(articleHtml, @"""content"":(\s?)""(.*?)(?<!\\)""");
        if (!contentMatch.Success) return BuildJson(uid, new JsonObject());
        var rawContent = contentMatch.Groups[2].Value;
        // 移除 p span 等标签干扰
        var content = Regex.Replace(rawContent, @"<(?!img\b)[^>]+>", "");

        // 3. 找 [name] -> url
        var dict = new JsonObject();
        var pattern = new Regex(@"\[(.*?)\].*?<img[^>]*\s+src=\\[""\']([^""\']+)\\[""\']",
            RegexOptions.Singleline);

        foreach (Match match in pattern.Matches(content))
        {
            var name = match.Groups[1].Value;
            var url  = match.Groups[2].Value.Replace("&amp;", "&");
            dict.TryAdd($"[{name}]",url);
        }
        
        return BuildJson(uid, dict);
    }

    private static string BuildJson(string uid, JsonObject emotions)
    {
        var root = new JsonObject
        {
            ["uid"]  = uid,
            ["time"] = DateTimeOffset.UtcNow.ToString("yyyy-MM-ddTHH:mm:ssZ"),
            ["timestamp"] = DateTimeOffset.UtcNow.ToUnixTimeSeconds(),
            ["emotions"] = emotions
        };
        return root.ToJsonString(new JsonSerializerOptions
        {
            WriteIndented = true,
            Encoder = System.Text.Encodings.Web.JavaScriptEncoder.UnsafeRelaxedJsonEscaping
        });
    }
}
```

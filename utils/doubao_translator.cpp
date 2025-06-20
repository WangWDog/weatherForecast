#include <curl/curl.h>
#include <json.hpp>
#include <fstream>
#include <iostream>
#include "../config/config_key.h"
#include "doubao_translator.h"
#include "../core/weather_manager.h"


using json = nlohmann::json;

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    // 计算总数据大小 = 块大小 × 块数量
    size_t totalSize = size * nmemb;

    // 将数据追加到输出字符串
    // 将 void* 转换为 char* 并添加到字符串末尾
    output->append((char*)contents, totalSize);

    // 返回处理的数据大小（告知CURL已处理）
    return totalSize;
}

// 翻译农历数据（如果当前语言是英文）
// data: 农历数据结构体引用
// lang: 当前语言设置
// configKey: 配置对象（包含API密钥）
void translateLunarDataIfEnglish(LunarData& data, const std::string& lang, const ConfigKey& configKey) {
    // 如果当前语言不是英文，直接返回（不需要翻译）
    if (lang != "en") return;

    // 定义lambda函数用于翻译单个字段
    // [&] 捕获所有外部变量引用
    auto t = [&](std::string& field) {
        // 检查字段是否为空
        if (!field.empty()) {
            // 调用翻译函数翻译字段内容
            // 源语言为中文（"英文"参数表示目标语言是英文）
            std::string translated = translateWithDoubao(field, "英文", configKey);

            // 如果翻译结果非空，则更新字段
            if (!translated.empty()) field = translated;
        }
    };

    // 对农历数据的各个字段进行翻译
    t(data.festivals);    // 翻译节日信息
    t(data.yi);           // 翻译"宜"事项
    t(data.ji);           // 翻译"忌"事项
    t(data.weiyuShort);   // 翻译简短尾语
    t(data.weiyuLong);    // 翻译详细尾语
}

// 使用豆包API进行翻译
std::string translateWithDoubao(const std::string& text, const std::string& targetLang, const ConfigKey& configKey) {
    // 从配置获取豆包API密钥
    std::string apiKey = configKey.getDoubaoKey();
    // 从配置获取API端点ID
    std::string endpointId = configKey.getDoubaoEndpoint();

    // 检查API配置是否完整
    if (apiKey.empty() || endpointId.empty()) {
        std::cerr << "❌ Doubao 配置缺失。" << std::endl;
        return text;  // 返回原文
    }

    // 构建翻译提示词
    // 格式："请将以下内容翻译为[目标语言]：[文本]"
    std::string prompt = "请将以下内容翻译为" + targetLang + "：\n" + text;

    // 构建JSON请求体
    json payload = {
        {"model", endpointId},  // 指定模型端点
        {"messages", {
            // 系统提示（定义助手角色）
            {{"role", "system"}, {"content", "You are a helpful assistant. 请在json字串中每个值前加入\\t"}},
            // 用户输入（翻译请求）
            {{"role", "user"}, {"content", prompt}}
        }}
    };

    std::string response;  // 存储API响应
    CURL* curl = curl_easy_init();  // 初始化CURL
    if (!curl) return text;  // 初始化失败返回原文

    // 设置HTTP请求头
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");  // JSON类型
    headers = curl_slist_append(headers, ("Authorization: Bearer " + apiKey).c_str());  // 认证头

    // 设置CURL选项
    curl_easy_setopt(curl, CURLOPT_URL, "https://ark.cn-beijing.volces.com/api/v3/chat/completions");  // API地址
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);  // 设置请求头
    std::string body = payload.dump();  // 将JSON转换为字符串
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());  // 设置POST数据
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);  // 设置写回调函数
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);  // 设置写数据目标
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);  // 禁用SSL验证（不安全，生产环境应启用）
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip");  // 支持gzip压缩

    // 执行HTTP请求
    CURLcode res = curl_easy_perform(curl);

    // 清理资源
    curl_slist_free_all(headers);  // 释放请求头
    curl_easy_cleanup(curl);       // 清理CURL

    // 检查请求是否成功
    if (res != CURLE_OK) {
        std::cerr << "❌ Doubao 请求失败: " << curl_easy_strerror(res) << std::endl;
        return text;  // 失败返回原文
    }

    // 解析API响应
    try {
        auto j = json::parse(response);  // 解析JSON响应

        // 检查响应中是否包含"choices"字段
        if (j.contains("choices")) {
            // 提取第一个选择的文本内容
            return j["choices"][0]["message"]["content"];
        }
    } catch (...) {
        // 捕获所有解析异常
        std::cerr << "❌ 返回 JSON 解析失败" << std::endl;
    }

    return text;  // 解析失败返回原文
}

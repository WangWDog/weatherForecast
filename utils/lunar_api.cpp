#include "lunar_api.h"
#include <iostream>
#include <json.hpp>
#include <curl/curl.h>
#include <ctime>
#include <iomanip>
#include <sstream>
#include "doubao_translator.h"

using json = nlohmann::json;

// CURL 回调函数：用于处理HTTP响应数据
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    // 将接收到的数据追加到用户提供的字符串中
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    // 返回处理的数据大小
    return size * nmemb;
}

// 获取当前日期的YYYYMMDD格式字符串
std::string getTodayDateYYYYMMDD() {
    std::time_t now = std::time(nullptr);  // 获取当前时间
    std::tm* local = std::localtime(&now); // 转换为本地时间
    std::ostringstream oss;  // 创建字符串流
    // 使用put_time格式化时间为"20240605"格式
    oss << std::put_time(local, "%Y%m%d");
    return oss.str();  // 返回格式化后的字符串
}

// 格式化农历信息为字符串
std::string formatLunarInfo(const LunarData& d, I18n& i18n) {
    std::ostringstream oss;  // 创建输出字符串流

    // 定义lambda函数：获取翻译文本
    auto label = [&](const std::string& key) {
        return i18n.tr("lunar", key);  // 从i18n获取翻译
    };

    // 格式化各部分信息（带表情符号和标签）
    if (!d.solarDate.empty())
        oss << "\t📅 " << label("solar") << "：" << d.solarDate << "（" << d.week << "）\n";
    if (!d.lunarYear.empty() || !d.lunar.empty())
        oss << "\t🌙 " << label("lunar_year") << "：" << d.lunarYear << " "
            << d.lunar << "（" << d.thisYear << "）\n";
    if (!d.ganzhiYear.empty() || !d.constellation.empty())
        oss << "\t📖 " << label("ganzhi_year") << "：" << d.ganzhiYear << " | "
            << label("constellation") << "：" << d.constellation << "\n";
    if (!d.festivals.empty())
        oss << "\t🎉 " << label("festival") << "：" << d.festivals << "\n";
    if (!d.jieqi.empty())
        oss << "\t🌾 " << label("jieqi") << "：" << d.jieqi << "\n";
    if (!d.yi.empty())
        oss << "\t✅ " << label("yi") << "：" << d.yi << "\n";
    if (!d.ji.empty())
        oss << "\t⚠️ " << label("ji") << "：" << d.ji << "\n";
    if (!d.weiyuShort.empty())
        oss << "\t💬 " << label("weiyu_short") << "：" << d.weiyuShort << "\n";
    if (!d.weiyuLong.empty())
        oss << "\t📖 " << label("weiyu_long") << "：" << d.weiyuLong << "\n";

    return oss.str();  // 返回格式化后的字符串
}

// 调用农历API获取数据
std::string callLunarApi(std::string apiKey) {
    CURL* curl;  // CURL句柄
    CURLcode res; // CURL操作结果
    std::string readBuffer; // 存储API响应

    curl_global_init(CURL_GLOBAL_DEFAULT);  // 初始化CURL
    curl = curl_easy_init();  // 创建CURL实例

    // 获取当前日期（YYYYMMDD格式）
    std::string date = getTodayDateYYYYMMDD();

    // 构建API请求URL
    std::string final_url = "https://api.shwgij.com/api/lunars/lunar?date=" + date +
                            "&key=" + apiKey;  // 可添加语言参数 &lang=...

    if (curl) {
        // 设置CURL选项
        curl_easy_setopt(curl, CURLOPT_URL, final_url.c_str());  // 设置URL
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);  // 设置回调
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);  // 设置数据存储
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);  // 禁用SSL验证
        curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip");  // 支持gzip压缩

        // 执行请求
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "❌ cURL 请求失败：" << curl_easy_strerror(res) << std::endl;
            readBuffer.clear();  // 清空响应
        }

        curl_easy_cleanup(curl);  // 清理CURL资源
    }

    return readBuffer;  // 返回API响应
}

// 解析农历API返回的JSON数据
LunarData parseLunarJson(const std::string& jsonStr) {
    LunarData data;  // 创建农历数据结构体

    try {
        // 解析JSON字符串
        auto j = json::parse(jsonStr);

        // 检查是否有"data"字段
        if (j.contains("data")) {
            const auto& d = j["data"];  // 获取数据对象

            // 提取并填充各个字段
            data.solarDate     = d.value("Solar", "");         // 公历日期
            data.week          = d.value("Week", "");          // 星期几
            data.constellation = d.value("Constellation", ""); // 星座
            data.festivals     = d.value("Festivals", "");     // 节日
            data.lunarYear     = d.value("LunarYear", "");     // 农历年份
            data.lunar         = d.value("Lunar", "");         // 农历日期
            data.thisYear      = d.value("ThisYear", "");      // 今年生肖
            data.ganzhiYear    = d.value("GanZhiYear", "");    // 干支年份
            data.jieqi         = d.value("JieQi1", "");        // 节气
            data.yi            = d.value("YiDay", "");         // 宜事项
            data.ji            = d.value("JiDay", "");         // 忌事项
            data.weiyuShort    = d.value("WeiYu_s", "");       // 简短尾语
            data.weiyuLong     = d.value("WeiYu_l", "");       // 详细尾语
        }
    } catch (const std::exception& e) {
        // 捕获解析异常
        std::cerr << "❌ JSON 解析失败：" << e.what() << std::endl;
    }

    return data;  // 返回农历数据
}

// 打印农历信息
void printLunarData(const LunarData& d, const std::string& lang, const ConfigKey& configKey, const I18n& i18n) {
    LunarData data = d;  // 创建副本（可能修改）

    // 如果语言是英文，翻译农历数据
    if (lang == "en") {
        translateLunarDataIfEnglish(data, lang, configKey);
    }

    // 使用i18n获取翻译标签，打印格式化信息
    std::cout << i18n.tr("lunar", "solar_date") << ": " << data.solarDate << " (" << data.week << ")\n";
    std::cout << i18n.tr("lunar", "lunar_date") << ": " << data.lunarYear << " " << data.lunar
              << " (" << data.thisYear << ")\n";
    std::cout << i18n.tr("lunar", "ganzhi_year") << ": " << data.ganzhiYear
              << " | " << i18n.tr("lunar", "constellation") << ": " << data.constellation << "\n";
    std::cout << i18n.tr("lunar", "festivals") << ": " << data.festivals << "\n";
    std::cout << i18n.tr("lunar", "jieqi") << ": " << data.jieqi << "\n";
    std::cout << i18n.tr("lunar", "yi") << ": " << data.yi << "\n";
    std::cout << i18n.tr("lunar", "ji") << ": " << data.ji << "\n";
    std::cout << i18n.tr("lunar", "weiyu_short") << ": " << data.weiyuShort << "\n";
    std::cout << i18n.tr("lunar", "weiyu_long") << ": " << data.weiyuLong << "\n";
}
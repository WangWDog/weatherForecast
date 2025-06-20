#include "ip_locator.h"
#include "json.hpp"  // nlohmann/json
#include <curl/curl.h>
#include <sstream>
#include <iostream>

using json = nlohmann::json;


static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    // 将数据追加到用户提供的字符串中
    // 计算总大小 = 块大小 × 块数量
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;  // 返回处理的数据大小
}

// 通过IP地址获取地理位置信息
// 返回值：Location结构体，包含城市名、纬度和经度
Location getLocationByIP() {
    CURL* curl;           // CURL句柄
    CURLcode res;          // CURL操作结果
    std::string readBuffer; // 存储API响应的字符串

    // 初始化CURL
    curl = curl_easy_init();
    if (curl) {
        // 设置请求URL（ip-api.com提供的免费IP定位服务）
        curl_easy_setopt(curl, CURLOPT_URL, "http://ip-api.com/json/");

        // 设置数据接收回调函数
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

        // 设置接收数据的缓冲区
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // 执行HTTP GET请求
        res = curl_easy_perform(curl);

        // 清理CURL资源
        curl_easy_cleanup(curl);
    }

    // 创建位置对象（默认值）
    Location loc;
    try {
        // 解析JSON响应
        auto j = json::parse(readBuffer);

        // 检查API返回状态
        if (j["status"] == "success") {
            // 提取城市名称
            loc.city = j["city"];
            // 提取纬度
            loc.latitude = j["lat"];
            // 提取经度
            loc.longitude = j["lon"];
        } else {
            // 输出错误信息（如果API返回失败）
            std::cerr << "Failed to get location: " << j["message"] << std::endl;
        }
    } catch (...) {
        // 捕获所有JSON解析异常
        std::cerr << "Failed to parse location JSON" << std::endl;
    }

    return loc;
}
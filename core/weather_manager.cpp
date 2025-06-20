#include "weather_manager.h"
#include <json.hpp>
#include <curl/curl.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <utility>
#include <ctime>

using json = nlohmann::json;

// 初始化 API 密钥、主机地址和语言
WeatherManager::WeatherManager(std::string key, std::string host, std::string lang)
    : apiKey(std::move(key)), host(std::move(host)), lang(std::move(lang)) {}

// CURL 回调函数：将接收到的数据追加到字符串中
static size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;  // 计算数据总大小
    output->append((char*)contents, totalSize);  // 将数据追加到输出字符串
    return totalSize;  // 返回已处理数据大小
}

// 搜索城市功能
std::vector<CityResult> WeatherManager::searchCity(const std::string& keyword, const std::string& lang) {
    std::vector<CityResult> results;  // 存储搜索结果
    std::string response;             // 存储API响应
    std::string url = "https://" + host + "/geo/v2/city/lookup?location=";  // 基础URL

    CURL* curl = curl_easy_init();  // 初始化CURL
    if (curl) {
        // 对关键词进行URL编码
        char* encodedKeyword = curl_easy_escape(curl, keyword.c_str(), keyword.length());
        if (!encodedKeyword) {
            std::cerr << "URL 编码失败" << std::endl;
            curl_easy_cleanup(curl);
            return results;  // 返回空结果
        }

        // 构建完整URL
        std::string fullUrl = url + encodedKeyword + "&lang=" + lang;
        curl_free(encodedKeyword);  // 释放编码后的字符串

        // 设置CURL选项
        curl_easy_setopt(curl, CURLOPT_URL, fullUrl.c_str());  // 设置请求URL

        // 添加API密钥到请求头
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, ("X-QW-Api-Key: " + apiKey).c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // 设置其他选项
        curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip");  // 支持gzip压缩
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);  // 设置写回调
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);  // 设置写数据目标
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);  // 禁用SSL验证
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);  // 禁用主机验证

        // 执行请求
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "请求失败: " << curl_easy_strerror(res) << std::endl;
        }

        // 清理资源
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    // 解析API响应
    try {
        auto j = json::parse(response);  // 解析JSON响应
        if (j.contains("location")) {
            // 遍历所有城市结果
            for (const auto& item : j["location"]) {
                CityResult city;
                // 提取城市信息
                city.name = item.value("name", "");
                city.id = item.value("id", "");
                city.country = item.value("country", "");
                city.adm1 = item.value("adm1", "");
                city.adm2 = item.value("adm2", "");
                results.push_back(city);  // 添加到结果列表
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "解析JSON失败: " << e.what() << std::endl;
    }

    return results;  // 返回搜索结果
}

// 获取7天天气预报（带缓存功能）
ForecastResult WeatherManager::get7DayForecast(const std::string& locationId, const std::string& language, int cacheExpiryMinutes) {
    ForecastResult result;  // 存储结果
    const std::string cacheFile = "cache.json";  // 缓存文件名
    json cache;  // JSON缓存对象

    // 尝试读取缓存文件
    std::ifstream fin(cacheFile);
    if (fin) {
        try {
            fin >> cache;  // 读取JSON缓存
        } catch (...) {
            cache.clear();  // 解析失败时清空缓存
        }
        fin.close();

        // 检查缓存是否有效
        if (cache.contains("forecast") && cache["forecast"].contains("timestamp")) {
            std::time_t now = std::time(nullptr);  // 当前时间
            std::time_t ts = cache["forecast"]["timestamp"].get<std::time_t>();  // 缓存时间

            // 检查缓存是否过期
            if (difftime(now, ts) < cacheExpiryMinutes * 60) {
                result.fromCache = true;  // 标记为缓存数据
                result.timestamp = ts;    // 设置时间戳

                // 从缓存中提取预报数据
                for (const auto& item : cache["forecast"]["data"]) {
                    DailyForecast f;
                    f.date = item["date"];
                    f.textDay = item["textDay"];
                    f.textNight = item["textNight"];
                    f.tempMax = item["tempMax"];
                    f.tempMin = item["tempMin"];
                    f.windDirDay = item["windDirDay"];
                    f.windScaleDay = item["windScaleDay"];
                    f.precip = item["precip"];
                    f.humidity = item["humidity"];
                    result.forecasts.push_back(f);  // 添加到预报列表
                }
                return result;  // 返回缓存数据
            }
        }
    }

    // 没有有效缓存，从API获取数据
    std::string url = "https://" + host + "/v7/weather/7d?location=" + locationId + "&lang=" + language;
    std::string response;
    CURL* curl = curl_easy_init();
    if (curl) {
        // 设置请求头
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, ("X-QW-Api-Key:" + apiKey).c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // 设置其他选项
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip");

        // 执行请求
        CURLcode res = curl_easy_perform(curl);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        if (res == CURLE_OK) {
            // 解析API响应
            auto j = json::parse(response, nullptr, false);
            if (!j.is_discarded() && j["code"] == "200") {
                result.fromCache = false;  // 标记为网络数据
                result.timestamp = std::time(nullptr);  // 设置当前时间戳

                // 提取每日预报数据
                for (const auto& day : j["daily"]) {
                    DailyForecast df;
                    df.date = day["fxDate"];
                    df.textDay = day["textDay"];
                    df.textNight = day["textNight"];
                    df.tempMax = day["tempMax"];
                    df.tempMin = day["tempMin"];
                    df.windDirDay = day["windDirDay"];
                    df.windScaleDay = day["windScaleDay"];
                    df.precip = day["precip"];
                    df.humidity = day["humidity"];
                    result.forecasts.push_back(df);  // 添加到预报列表
                }

                // 更新缓存
                cache["forecast"]["timestamp"] = result.timestamp;
                cache["forecast"]["data"] = json::array();
                for (const auto& f : result.forecasts) {
                    // 将预报数据添加到缓存JSON
                    cache["forecast"]["data"].push_back({
                        {"date", f.date},
                        {"textDay", f.textDay},
                        {"textNight", f.textNight},
                        {"tempMax", f.tempMax},
                        {"tempMin", f.tempMin},
                        {"windDirDay", f.windDirDay},
                        {"windScaleDay", f.windScaleDay},
                        {"precip", f.precip},
                        {"humidity", f.humidity}
                    });
                }

                // 保存缓存到文件
                std::ofstream fout(cacheFile);
                if (fout) fout << cache.dump(4);  // 格式化输出JSON
            }
        } else {
            // 请求失败处理
            std::cerr << "结果请求失败 res状态值：" << res << std::endl;
            exit(1);  // 退出程序（可能过于严格）
        }
    }
    return result;  // 返回预报结果
}

// 获取生活指数（带缓存功能）
LifeIndexWithMeta WeatherManager::getLifeIndices(const std::string& locationId, int expiryMinutes) {
    LifeIndexWithMeta result;  // 存储结果
    const std::string cacheFile = "cache.json";  // 缓存文件名
    nlohmann::json cache;  // JSON缓存对象

    // 尝试读取缓存文件
    std::ifstream fin(cacheFile);
    if (fin) {
        try {
            fin >> cache;  // 读取JSON缓存
        } catch (...) {
            cache.clear();  // 解析失败时清空缓存
        }
        fin.close();

        // 检查缓存是否有效
        if (cache.contains("life_index") && cache["life_index"].contains("timestamp")) {
            std::time_t now = std::time(nullptr);  // 当前时间
            std::time_t ts = cache["life_index"]["timestamp"].get<std::time_t>();  // 缓存时间

            // 检查缓存是否过期
            if (difftime(now, ts) < expiryMinutes * 60) {
                result.timestamp = ts;    // 设置时间戳
                result.fromCache = true;  // 标记为缓存数据

                // 从缓存中提取生活指数
                for (const auto& item : cache["life_index"]["data"]) {
                    result.indices.push_back({
                        item["date"],
                        item["name"],
                        item["level"],
                        item["category"],
                        item["text"]
                    });  // 添加到指数列表
                }
                return result;  // 返回缓存数据
            }
        }
    }

    // 没有有效缓存，从API获取数据
    std::string url = "https://" + host + "/v7/indices/1d?type=0&location=" + locationId + "&lang=" + lang;
    CURL* curl = curl_easy_init();
    std::string response;

    if (curl) {
        // 设置请求头
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, ("X-QW-Api-Key: " + apiKey).c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // 设置其他选项
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip");

        // 执行请求
        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);

        if (res == CURLE_OK) {
            try {
                // 解析API响应
                auto j = nlohmann::json::parse(response);
                if (j.contains("daily")) {
                    result.timestamp = std::time(nullptr);  // 设置当前时间戳

                    // 提取生活指数数据
                    for (const auto& item : j["daily"]) {
                        result.indices.push_back({
                            item.value("date", ""),
                            item.value("name", ""),
                            item.value("level", ""),
                            item.value("category", ""),
                            item.value("text", "")
                        });  // 添加到指数列表
                    }

                    // 更新缓存
                    nlohmann::json cacheOut;
                    cacheOut["life_index"]["timestamp"] = result.timestamp;
                    cacheOut["life_index"]["data"] = nlohmann::json::array();
                    for (const auto& idx : result.indices) {
                        // 将指数数据添加到缓存JSON
                        cacheOut["life_index"]["data"].push_back({
                            {"date", idx.date},
                            {"name", idx.name},
                            {"level", idx.level},
                            {"category", idx.category},
                            {"text", idx.text}
                        });
                    }

                    // 保存缓存到文件
                    std::ofstream fout(cacheFile);
                    if (fout) fout << cacheOut.dump(4);  // 格式化输出JSON
                }
            } catch (...) {
                // 忽略解析错误
            }
        }
    }

    return result;  // 返回生活指数结果
}

// 获取缓存的当前天气信息
WeatherNow WeatherManager::getCachedCurrentWeather(const std::string& locationId) {
    WeatherNow result;  // 存储结果
    const std::string cacheFile = "cache_weather.json";  // 缓存文件名

    // 尝试读取缓存文件
    std::ifstream fin(cacheFile);
    if (fin) {
        try {
            json cache;
            fin >> cache;  // 读取JSON缓存
            fin.close();

            // 检查缓存是否有效
            if (cache.contains("current_weather") && cache["current_weather"].contains("timestamp")) {
                std::time_t now = std::time(nullptr);  // 当前时间
                std::time_t ts = cache["current_weather"]["timestamp"].get<std::time_t>();  // 缓存时间

                // 检查缓存是否过期（1小时有效期）
                if (difftime(now, ts) < 3600) {
                    result.success = true;  // 标记为成功获取

                    // 从缓存中提取天气数据（带默认值）
                    if (cache["current_weather"].contains("temperature")) {
                        result.data.temp = cache["current_weather"]["temperature"];
                    } else {
                        result.data.temp = "未知";
                    }

                    if (cache["current_weather"].contains("text")) {
                        result.data.text = cache["current_weather"]["text"];
                    } else {
                        result.data.text = "未知";
                    }

                    if (cache["current_weather"].contains("windSpeed")) {
                        result.data.windSpeed = cache["current_weather"]["windSpeed"];
                    } else {
                        result.data.windSpeed = "未知";
                    }

                    if (cache["current_weather"].contains("humidity")) {
                        result.data.humidity = cache["current_weather"]["humidity"];
                    } else {
                        result.data.humidity = "未知";
                    }
                }
            }
        } catch (const std::exception& e) {
            // 处理缓存读取错误
            std::cerr << "读取缓存出错: " << e.what() << std::endl;
        }
    }

    return result;  // 返回天气结果
}

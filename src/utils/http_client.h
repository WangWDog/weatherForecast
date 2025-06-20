#pragma once
#include <string>
#include <vector>
#include <curl/curl.h>

class HttpClient {
public:
    static bool get(const std::string& url, std::string& response,
                    const std::vector<std::string>& headers = {});
    static bool post(const std::string& url, const std::string& body,
                     std::string& response,
                     const std::vector<std::string>& headers = {});
private:
    static size_t writeCallback(char* ptr, size_t size, size_t nmemb, void* userdata);
};

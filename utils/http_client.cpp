#include "http_client.h"
#include <utility>

size_t HttpClient::writeCallback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    std::string* str = static_cast<std::string*>(userdata);
    str->append(ptr, size * nmemb);
    return size * nmemb;
}

static void applyHeaders(CURL* curl, struct curl_slist*& headerList, const std::vector<std::string>& headers) {
    for (const auto& h : headers) {
        headerList = curl_slist_append(headerList, h.c_str());
    }
    if (headerList) {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerList);
    }
}

bool HttpClient::get(const std::string& url, std::string& response,
                     const std::vector<std::string>& headers) {
    CURL* curl = curl_easy_init();
    if (!curl) return false;
    struct curl_slist* headerList = nullptr;
    applyHeaders(curl, headerList, headers);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip");
    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headerList);
    curl_easy_cleanup(curl);
    return res == CURLE_OK;
}

bool HttpClient::post(const std::string& url, const std::string& body,
                      std::string& response,
                      const std::vector<std::string>& headers) {
    CURL* curl = curl_easy_init();
    if (!curl) return false;
    struct curl_slist* headerList = nullptr;
    applyHeaders(curl, headerList, headers);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip");
    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headerList);
    curl_easy_cleanup(curl);
    return res == CURLE_OK;
}

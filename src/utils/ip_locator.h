#pragma once
#include <string>

struct Location {
    // 成员变量：城市名称（字符串类型）
    std::string city;

    // 成员变量：纬度（双精度浮点数）
    // 表示地理位置的纬度坐标（北纬为正，南纬为负）
    double latitude;

    // 成员变量：经度（双精度浮点数）
    // 表示地理位置的经度坐标（东经为正，西经为负）
    double longitude;
};

Location getLocationByIP();
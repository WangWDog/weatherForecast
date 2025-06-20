#pragma once

#include <string>

#include "config_context.h"
#include "../config/config_key.h"
#include "i18n_loader.h"

// 农历数据结构体
// 用于存储从农历API获取的各种传统农历信息
struct LunarData {
    std::string solarDate;     // 公历日期（如 "2024-06-05"）
    std::string week;          // 星期几（如 "星期三"）
    std::string constellation; // 星座（如 "双子座"）
    std::string festivals;     // 节日信息（如 "端午节"）
    std::string lunarYear;     // 农历年份（如 "二〇二四"）
    std::string lunar;         // 农历日期（如 "五月初一"）
    std::string thisYear;      // 生肖年（如 "龙年"）
    std::string ganzhiYear;    // 干支年份（如 "甲辰"）
    std::string jieqi;         // 节气信息（如 "芒种"）
    std::string yi;            // 宜事项（如 "嫁娶、出行"）
    std::string ji;            // 忌事项（如 "动土、破土"）
    std::string weiyuShort;    // 简短尾语/运势（如 "宜静不宜动"）
    std::string weiyuLong;     // 详细尾语/运势（如 "今日宜静心修养..."）
};

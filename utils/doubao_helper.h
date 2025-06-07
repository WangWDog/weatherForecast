#ifndef WEATHER_CLI_DOUBAO_HELPER_H
#define WEATHER_CLI_DOUBAO_HELPER_H
#pragma once

#include <string>

// 调用豆包 AI，返回生活建议文本
// 参数：
// - token：API Key
// - endpointId：模型名称（如 "doubao-1.5-pro-32k-250115"）
// - cityName：城市名称（如 "温州"），可用于构造 prompt
// 修改声明：将 cityName 替换为 fullPrompt
std::string callDoubaoAI(const std::string& token, const std::string& endpointId, const std::string& fullPrompt);



#endif // WEATHER_CLI_DOUBAO_HELPER_H

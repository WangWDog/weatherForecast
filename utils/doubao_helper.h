#ifndef DOUBAO_HELPER_H
#define DOUBAO_HELPER_H

#include <string>

// 调用豆包大模型，返回 AI 建议文本
std::string callDoubaoAI(const std::string& token, const std::string& endpointId, const std::string& prompt);

#endif // DOUBAO_HELPER_H

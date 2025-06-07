#ifndef WEATHERFORECAST_DOUBAO_HELPER_H
#define WEATHERFORECAST_DOUBAO_HELPER_H

#include <string>

// 使用 Doubao AI 接口生成生活建议
// 参数：token（API 密钥）、endpointId（模型 ID）、prompt（提示语）
// 返回：AI 生成的回答，如遇错误返回错误描述文本
std::string callDoubaoAI(const std::string& token, const std::string& endpointId, const std::string& prompt);

#endif // WEATHERFORECAST_DOUBAO_HELPER_H

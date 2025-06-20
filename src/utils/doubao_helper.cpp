#include "doubao_helper.h"
#include "../core/doubao_manager.h"

std::string callDoubaoAI(const std::string& token, const std::string& endpointId, const std::string& fullPrompt) {
    DoubaoManager mgr(token, endpointId);
    return mgr.getAdvice(fullPrompt);
}

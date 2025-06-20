#include "doubao_translator.h"
#include "../core/doubao_manager.h"
#include <iostream>

void translateLunarDataIfEnglish(LunarData& data, const std::string& lang, const ConfigKey& configKey) {
    if (lang != "en") return;
    DoubaoManager mgr(configKey.getDoubaoKey(), configKey.getDoubaoEndpoint());
    auto t = [&](std::string& field) {
        if (!field.empty()) {
            std::string translated = mgr.translate(field, "英文");
            if (!translated.empty()) field = translated;
        }
    };
    t(data.festivals);
    t(data.yi);
    t(data.ji);
    t(data.weiyuShort);
    t(data.weiyuLong);
}

std::string translateWithDoubao(const std::string& text, const std::string& targetLang, const ConfigKey& configKey) {
    DoubaoManager mgr(configKey.getDoubaoKey(), configKey.getDoubaoEndpoint());
    return mgr.translate(text, targetLang);
}

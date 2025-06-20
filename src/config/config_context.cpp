
#include "config_context.h"

ConfigContext::ConfigContext(const std::string& userPath, const std::string& keyPath)
    : configUser(userPath), configKey(keyPath) {
    configUser.load();
    configKey.load();
}

ConfigUser& ConfigContext::user() {
    return configUser;
}

ConfigKey& ConfigContext::key() {
    return configKey;
}

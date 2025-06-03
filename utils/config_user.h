#pragma once
#include "base_config.h"

class ConfigUser : public BaseConfig {
public:
    explicit ConfigUser(const std::string& path = "configUser.json");

    std::string getDefaultCity() const;
    void setDefaultCity(const std::string& city);

    bool getUseIPLocation() const;
    void setUseIPLocation(bool useIP);

    std::string getUnits() const;
    void setUnits(const std::string& units);

    std::string getLanguage() const;
    void setLanguage(const std::string& lang);

    int getCacheExpiry(const std::string& key) const;
    void setCacheExpiry(const std::string& key, int minutes);

    std::string getCityId() const;
    void setCityId(const std::string& id);
};
#pragma once
#include "base_config.h"

class ConfigUser : public BaseFile {
public:
    explicit ConfigUser(const std::string& path = "configs/configUser.json");


    bool getUseIPLocation() const;
    void setUseIPLocation(bool useIP);

    std::string getUnits() const;
    void setUnits(const std::string& units);

    std::string getIndexUnits() const;
    void setIndexUnits(const std::string& units);

    std::string getLanguage() const;
    void setLanguage(const std::string& lang);

    int getCacheExpiry(const std::string& key) const;
    void setCacheExpiry(const std::string& key, int minutes);

    std::string getDateFormateMenu() const;
    void setDateFormateMenu(const std::string& dateFormate);

    std::string getCityId() const;
    void setCityId(const std::string& id);

    std::string getCityName() const;
    void setCityName(const std::string& name);


};
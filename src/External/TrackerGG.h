#pragma once
#include <string>
#include <dpp/cluster.h>
#include <nlohmann/json.hpp>

class TrackerGG
{
public:
    static std::string GetToken();
    static nlohmann::json GetValorantPlayerStats(dpp::cluster& bot, const std::string& username);
    
};

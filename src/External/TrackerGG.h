#pragma once
#include <string>
#include <dpp/cluster.h>
#include <nlohmann/json.hpp>

#define TRACKERGG_TOKEN "7413a1fb-4259-4cb6-bd64-392f36ed6c23"

class TrackerGG
{
public:
    static nlohmann::json GetValorantPlayerStats(dpp::cluster& bot, const std::string& username);
    
};

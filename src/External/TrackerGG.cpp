#include "TrackerGG.h"

#include <iostream>
#include <dpp/dpp.h>
#include <future>

nlohmann::json TrackerGG::GetValorantPlayerStats(dpp::cluster& bot, const std::string& username) {
    const std::string apiKey = TRACKERGG_TOKEN;
    const std::string platform = "riot";
    const std::string game = "standard";
    const std::string encodedUsername = dpp::utility::url_encode(username);

    std::promise<nlohmann::json> p;
    std::future<nlohmann::basic_json<>> f = p.get_future();

    std::string url =
      "https://api.tracker.gg/api/v2/valorant/standard/profile/riot/" + encodedUsername;

    std::multimap<std::string,std::string> headers = {
        {"Accept", "application/json"},
        {"User-Agent", "dpp-bot/1.0"},
        {"TRN-Api-Key", apiKey}
    };

    bot.request(
      url, dpp::m_get,
      [&p](const dpp::http_request_completion_t& cc) {
        std::cerr << "Tracker.gg status=" << cc.status
                  << " body=" << cc.body << "\n";
        if (cc.status != 200) {
          p.set_value(nullptr);
        } else {
          p.set_value(nlohmann::json::parse(cc.body));
        }
      },
      "", "", headers
    );

    return f.get();
}
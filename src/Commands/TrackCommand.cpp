#include "TrackCommand.h"

#include "Bot.h"
#include "External/TrackerGG.h"

void TrackCommand::Execute(const dpp::interaction_create_t& event)
{
    if (std::get<std::string>(event.get_parameter("game")) == "val")
    {
        // event.reply("Tracking Valorant...");
        auto username = std::get<std::string>(event.get_parameter("username"));

        auto stats = TrackerGG::GetValorantPlayerStats(p_bot->GetHandle(), username);

        if (stats == nullptr)
        {
            event.reply("Player not found.");
            return;
        }

        if (stats["error"] != nullptr)
        {
            event.reply(stats["error"]["message"]);
            return;
        }

        auto embed = dpp::embed()
            .set_color(0x00FF00)
            .set_title("Valorant stats for " + username)
            .set_url("https://tracker.gg/valorant/profile/riot/" + username)
            .add_field("Rank", stats["data"]["segments"][0]["stats"]["rank"], true)
            .add_field("Winrate", stats["data"]["segments"][0]["stats"]["winrate"], true)
            .add_field("KDA", stats["data"]["segments"][0]["stats"]["kda"], true)
            .add_field("Headshots", stats["data"]["segments"][0]["stats"]["headshots"], true)
            .add_field("Kills", stats["data"]["segments"][0]["stats"]["kills"], true)
            .add_field("Assists", stats["data"]["segments"][0]["stats"]["assists"], true)
            .add_field("Deaths", stats["data"]["segments"][0]["stats"]["deaths"], true)
            .add_field("KD", stats["data"]["segments"][0]["stats"]["kd"], true);

        event.reply(embed);
    }
    else
    {
        event.reply("Other game than Valorant are not supported yet.");
    }
}

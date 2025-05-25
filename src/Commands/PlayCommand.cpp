#include "PlayCommand.h"

#include "External/YoutubeDL.h"

void PlayCommand::Execute(const dpp::interaction_create_t& event)
{
    std::string url = std::get<std::string>(event.get_parameter("song"));
    event.reply("Playing...");

    auto ouputPath = YoutubeDL::DownloadVideo(p_bot, url);
}

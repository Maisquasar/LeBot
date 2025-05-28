#include "PlayCommand.h"

#include "Bot.h"
#include "ThreadManager.h"
#include "External/YoutubeDL.h"

void PlayCommand::Execute(const dpp::interaction_create_t& event)
{    
    std::string url = std::get<std::string>(event.get_parameter("song"));

    if (!p_bot->JoinVocalChannel(event.command.guild_id, event.command.usr.id))
    {
        event.reply("You are not in a voice channel.");
        return;
    }

    AudioPlayer& audioPlayer = p_bot->GetAudioPlayer();
    if (YoutubeDL::IsPlaylist(url))
    {
        event.reply("Playlist " + url + " added to queue.");
    
        ThreadManager::AddTask(&AudioPlayer::AddPlaylist, &audioPlayer, url);
    }
    else
    {
        event.reply("Song " + url + " added to queue.");
    
        ThreadManager::AddTask(&AudioPlayer::AddSong, &audioPlayer, url);
    }
}

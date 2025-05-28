#include "QueueCommand.h"

#include "Bot.h"

#include <Audio/AudioPlayer.h>

void QueueCommand::Execute(const dpp::interaction_create_t& event)
{
    auto queue = p_bot->GetAudioPlayer().GetAudioQueue(); // std::queue<std::shared_ptr<Audio>>

    dpp::embed embed;
    embed.set_title("🎶 File d'attente")
         .set_color(dpp::colors::blue);

    if (auto currentAudio = p_bot->GetAudioPlayer().GetCurrentAudio(); currentAudio)
        embed.add_field("En cours", currentAudio->GetName(), true);

    if (queue.empty())
    {
        embed.set_description("La file d'attente est vide.");
    }
    else
        {
        std::string desc;
        int index = 0;

        while (!queue.empty() && index <= 10) {
            auto track = queue.front();
            queue.pop();
            desc += std::to_string(index) + ". " + track->GetName() + "\n"; // adapte selon la structure Audio
            ++index;
        }

        embed.set_description(desc);
    }

    event.reply(dpp::message().add_embed(embed));
}



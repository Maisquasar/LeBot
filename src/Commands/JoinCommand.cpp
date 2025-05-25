#include "JoinCommand.h"

#include "Bot.h"

void JoinCommand::Execute(const dpp::interaction_create_t& event)
{
    event.reply("Joining...");
    p_bot->JoinVocalChannel(event.command.guild_id, event.command.usr.id);
    event.delete_original_response();
}

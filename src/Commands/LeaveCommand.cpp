#include "LeaveCommand.h"

#include "Bot.h"

void LeaveCommand::Execute(const dpp::interaction_create_t& event)
{
    event.reply("Leaving...");
    p_bot->LeaveVocalChannel(event.command.guild_id, event.from);
    event.delete_original_response();
}

#include "PingCommand.h"

void PingCommand::Execute(const dpp::interaction_create_t& event)
{
    event.reply("Pong!");
}

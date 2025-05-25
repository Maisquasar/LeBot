#include "TestCommand.h"

#include "Bot.h"

void TestCommand::Execute(const dpp::interaction_create_t& event)
{
    p_bot->Message(event.command.channel_id, "Test command executed!");
}

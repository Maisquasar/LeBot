#include "ICommand.h"

#include "Bot.h"

ICommand::ICommand(Bot* bot, const std::string& name, const std::string& description,
                   dpp::command_option options) : p_bot(bot), p_name(name), p_description(description), p_options({options})
{
}

ICommand::ICommand(Bot* bot, const std::string& name, const std::string& description,
                   std::vector<dpp::command_option> options) : p_name(name), p_description(description), p_options(options), p_bot(bot)
{
    
}

void ICommand::Register()
{
    dpp::cluster& handle = p_bot->GetHandle();
    p_command.set_name(p_name)
               .set_description(p_description)
               .set_application_id(handle.me.id);

    for (const auto& option : p_options)
    {
        p_command.add_option(option);
    }
    
    handle.global_command_create(p_command);
}

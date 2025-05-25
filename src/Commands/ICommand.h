#pragma once
#include <dpp/cluster.h>
class Bot;

class ICommand
{
public:
    virtual ~ICommand() = default;
    ICommand(Bot* bot, const std::string& name, const std::string& description, dpp::command_option options);
    ICommand(Bot* bot, const std::string& name, const std::string& description, std::vector<dpp::command_option> options = {});

    virtual void Register();
    
    virtual void Execute(const dpp::interaction_create_t& event) = 0;

    std::string GetName() const { return p_name; }
protected:
    std::string p_name;
    std::string p_description;
    
    std::vector<dpp::command_option> p_options;
    
    Bot* p_bot;
    dpp::slashcommand p_command;
    
};

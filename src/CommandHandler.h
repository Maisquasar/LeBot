#pragma once
#include <memory>
#include <vector>

#include "Commands/ICommand.h"

class Bot;

class CommandHandler
{
public:
    CommandHandler(Bot* bot) : m_bot(bot) {}
    
    void RegisterCommands();

    void HandleInteraction(const dpp::interaction_create_t& event) const;

private:
    Bot* m_bot;
    std::vector<std::unique_ptr<ICommand>> m_commands;
};

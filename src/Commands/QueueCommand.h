#pragma once
#include "ICommand.h"

class QueueCommand : public ICommand
{
public:
    using ICommand::ICommand;

    void Execute(const dpp::interaction_create_t& event) override;
    
};

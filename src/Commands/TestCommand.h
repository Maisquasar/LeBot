#pragma once
#include "ICommand.h"

class TestCommand : public ICommand
{
public:
    using ICommand::ICommand;
    
    void Execute(const dpp::interaction_create_t& event) override;
};

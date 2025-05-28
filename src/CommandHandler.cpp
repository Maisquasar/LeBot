#include "CommandHandler.h"

#include "Bot.h"
#include "Commands/DeleteMessageCommand.h"
#include "Commands/JoinCommand.h"
#include "Commands/LeaveCommand.h"
#include "Commands/PingCommand.h"
#include "Commands/PlayCommand.h"
#include "Commands/TestCommand.h"
#include "Commands/TrackCommand.h"

void CommandHandler::RegisterCommands()
{
    dpp::cluster& handle = m_bot->GetHandle();
    // Ping
    m_commands.push_back(std::make_unique<PingCommand>(m_bot, "ping", "Ping the bot"));

    // Test
    m_commands.push_back(std::make_unique<TestCommand>(m_bot, "test", "Test command (debug)"));

    // Delete
    m_commands.push_back(std::make_unique<DeleteMessageCommand>(m_bot, "delete", "Delete last message(s)",
        dpp::command_option(dpp::co_integer, "amount", "The amount of messages to delete", true)));

    // Join
    m_commands.push_back(std::make_unique<JoinCommand>(m_bot, "join", "Join a voice channel"));

    // Leave
    m_commands.push_back(std::make_unique<LeaveCommand>(m_bot, "leave", "Leave a voice channel"));

    // Track
    {
        dpp::command_option gameOptions = dpp::command_option(dpp::co_string, "game", "The game to track", true);
        gameOptions.add_choice({"Valorant", "val"});
        gameOptions.add_choice({"League of Legends", "lol"});
        gameOptions.add_choice({"Overwatch", "ow"});
        gameOptions.add_choice({"Counter-Strike: Global Offensive", "csgo"});

        dpp::command_option username = dpp::command_option(dpp::co_string, "username", "The username to track", true);
        TrackCommand trackCommand(m_bot, "ping", "Ping the bot", {gameOptions, username});
        m_commands.push_back(
            std::make_unique<TrackCommand>(
                m_bot,
                "track",
                "Track a player",
                std::initializer_list<dpp::command_option>{ gameOptions, username }
            )
        );
    }

    // Play a song
    m_commands.push_back(std::make_unique<PlayCommand>(m_bot, "play", "Play a song", dpp::command_option(dpp::co_string, "song", "The song url (youtube only)", true)));
    
    handle.global_commands_get([&](const dpp::confirmation_callback_t& callback) {
        std::unordered_map<std::string, uint64_t> existingCommand;
    
        if (!callback.is_error()) {
            auto commands = std::get<dpp::slashcommand_map>(callback.value);
            for (const auto& [id, cmd] : commands) {
                // std::cout << "Existing Command: " << cmd.name << " (ID: " << id << ")\n";
                existingCommand[cmd.name] = id;
            }
        }
        
        for (auto& command : m_commands)
        {
            command->Register();
            m_bot->Log("Registered command {}", command->GetName());

            auto it = existingCommand.find(command->GetName());
            if (it != existingCommand.end())
            {
                existingCommand.erase(command->GetName());
            }
        }

        for (const auto& [name, id] : existingCommand)
        {
            handle.global_command_delete(id);
            m_bot->Log("Deleted command {}", name);
        }
    });
}

void CommandHandler::HandleInteraction(const dpp::interaction_create_t& event) const
{
    if (event.command.type != dpp::it_application_command)
        return;
    {
        dpp::command_interaction cmd_data = std::get<dpp::command_interaction>(event.command.data);

        for (const auto& command : m_commands)
        {
            if (command->GetName() == cmd_data.name)
            {
                command->Execute(event);
                break;
            }
        }
    }
}

#include "Bot.h"

#include "TrackerGG.h"

Bot::Bot(const std::string& token): m_bot(token), m_commandHandler(this)
{
    
}

void Bot::Initialize()
{
    m_bot.on_log(dpp::utility::cout_logger());
    
    m_bot.on_interaction_create(
        [&](const dpp::interaction_create_t &event)
        {
            m_commandHandler.HandleInteraction(event);
        }
    );

    m_bot.on_ready([&](const dpp::ready_t &event)
    {
        m_bot.log(dpp::ll_info, "Ready!");
        m_commandHandler.RegisterCommands();
    });

    
}

void Bot::Run()
{
    m_tickThread = std::thread(&Bot::RunThread, this);
    
    m_bot.start(dpp::st_wait);
    
    m_isRunning = false;
    if (m_tickThread.joinable()) {
        m_tickThread.join();
    }
}

void Bot::RunThread() const
{
    while (m_isRunning)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void Bot::Destroy()
{
    m_bot.shutdown();
    
    if (m_tickThread.joinable()) {
        m_tickThread.join();
    }
}

void Bot::Message(dpp::snowflake channelId, const std::string& message)
{
    m_bot.message_create(dpp::message(channelId, message));
}

void Bot::DeleteMessage(dpp::snowflake channelId, dpp::snowflake messageId)
{
    m_bot.message_delete(channelId, messageId);
}

bool Bot::JoinVocalChannel(const dpp::snowflake& guildId, const dpp::snowflake& userId)
{
    dpp::guild * g = dpp::find_guild(guildId);
    if (!g->connect_member_voice(userId)) {
        return false;
    }

    return true;
}

void Bot::LeaveVocalChannel(const dpp::snowflake& guildId, dpp::discord_client* client)
{
    client->disconnect_voice(guildId);
}

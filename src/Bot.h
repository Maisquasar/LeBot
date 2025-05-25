#pragma once
#include <dpp/dpp.h>
#include <atomic>
#include <thread>

#include <format>
#include "CommandHandler.h"

class Bot
{
public:
    explicit Bot(const std::string& token);

    void Initialize();
    void Run();
    void RunThread() const;
    void Destroy();

    void Message(dpp::snowflake channelId, const std::string& message);
    void DeleteMessage(dpp::snowflake channelId, dpp::snowflake messageId);

    bool JoinVocalChannel(const dpp::snowflake& guildId, const dpp::snowflake& userId);
    void LeaveVocalChannel(const dpp::snowflake& guildId, dpp::discord_client* client);

    dpp::cluster& GetHandle() { return m_bot; }
    
    template <typename... Args>
    void Log(std::string_view fmt, Args&&... args)
    {
        // auto fmt_args = std::make_format_args(std::forward<Args>(args)...);
        // std::string out = std::vformat(fmt, fmt_args);
        // m_bot.log(dpp::ll_info, out);
    }
    
    operator dpp::cluster&() { return m_bot; }
private:
    CommandHandler m_commandHandler;
    
    dpp::cluster m_bot;
    
    std::atomic<bool> m_isRunning = true;
    std::thread m_tickThread;
    
};

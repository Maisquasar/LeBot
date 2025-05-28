#pragma once
#include <dpp/dpp.h>
#include <atomic>
#include <thread>

#include <format>
#include "CommandHandler.h"
#include "Audio/AudioPlayer.h"

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

    void PlayAudio(Audio* audio);

    dpp::cluster& GetHandle() { return m_bot; }
    
    template <typename... Args>
    void Log(std::string_view fmt, const Args&... args)
    {
        auto fmt_args = std::make_format_args(args...);
        std::string out = std::vformat(fmt, fmt_args);
        m_bot.log(dpp::ll_info, out);
    }

    
    operator dpp::cluster&() { return m_bot; }

    AudioPlayer& GetAudioPlayer() { return m_audioPlayer; }
    bool IsRunning() const { return m_isRunning.load(); }
    bool IsVoiceReady() const { return m_isVoiceReady.load(); }

private:
    CommandHandler m_commandHandler;
    AudioPlayer m_audioPlayer = AudioPlayer(this);
    
    dpp::cluster m_bot;
    dpp::discord_voice_client* m_voiceClient = nullptr;
    
    std::atomic<bool> m_isRunning = true;

    std::atomic<bool> m_isVoiceReady = false;
};

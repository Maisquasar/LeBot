#pragma once
#include <string>
#include <cluster.h>

#include "Event.h"

#undef PlaySound
#undef SendMessage

class Sound;

class Bot
{
public:
    Bot(const std::string& token);
    ~Bot() = default;

    // ==== Initialize ==== //
    void Initialize();
    void CreateSlashCommands();

    void Run();
    void RunThread();

    void Destroy();

    bool JoinVocalChannel(const dpp::interaction_create_t &event);
    void PlayAudio(const std::string& url, const dpp::interaction_create_t& event);
    // The bot need to be on a voice channel
    bool PlaySound(Sound* sound, const dpp::interaction_create_t& event);

    void SendMessage(dpp::snowflake channelId, const std::string& message);
    void DeleteMessage(dpp::snowflake channelId, dpp::snowflake messageId);
    void DeleteLastMessages(dpp::snowflake channelId, int count = 100);
    
    dpp::message_map GetMessages(dpp::snowflake channelId, int count = 100);
    dpp::message_map GetMessagesOfUser(dpp::snowflake channelId, dpp::snowflake userId, int count = 100);
    
    // ==== Events ==== //
    void OnInteractionCreate(const dpp::interaction_create_t &event);
    void OnPlay(const dpp::interaction_create_t& event);

    // Tick on a separate thread
    void OnTick();
    
    // Function to delete all global commands
    void DeleteAllGlobalCommands();
private:
    dpp::cluster m_bot;

    std::atomic<bool> m_isRunning = true;
    std::thread m_tickThread;

    std::atomic_bool m_isVoiceReady = false;
    Event<> m_onVoiceReady;
    
};

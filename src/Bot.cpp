#include "Bot.h"

#include "ThreadManager.h"

Bot::Bot(const std::string& token): m_bot(token), m_commandHandler(this)
{
    
}

void Bot::Initialize()
{
    ThreadManager::GetInstance()->Initialize();
    
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

    m_bot.on_voice_state_update([&](const dpp::voice_state_update_t &event)
    {
        if (event.state.channel_id == 0)
        {
            m_isVoiceReady = false;
            m_voiceClient = nullptr;
        }
    });

    m_bot.on_voice_ready([&](const dpp::voice_ready_t &event)
    {
        m_isVoiceReady = true;
        m_voiceClient = event.voice_client;
    });
}

void Bot::Run()
{
    ThreadManager::AddTask(&Bot::RunThread, this);

    m_audioPlayer.Start();
    
    m_bot.start(dpp::st_wait);
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
    ThreadManager::GetInstance()->Terminate();
    
    m_isRunning = false;

    m_bot.shutdown();
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

void Bot::PlayAudio(Audio* audio)
{
    if (!m_isVoiceReady)
    {
        Log("Voice is not ready.");
        return;
    }

    audio->Play(m_voiceClient);
}

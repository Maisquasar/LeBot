#include "Bot.h"

#include  "AudioPlayer.h"

Bot::Bot(const std::string& token) : m_bot(token)
{
}

void Bot::Initialize()
{
    m_bot.on_interaction_create(
    [&](const dpp::interaction_create_t &event)
    {
        OnInteractionCreate(event);
    }
    );

    m_bot.on_ready([&](const dpp::ready_t &event) {
        // DeleteAllGlobalCommands(m_bot);
        CreateSlashCommands();
    });
    
            
    m_bot.on_voice_state_update([&](const dpp::voice_state_update_t& voice_state) {
    
        if (voice_state.state.channel_id == 0) {
            m_isVoiceReady = false;
        }
        else
        {
            m_isVoiceReady = true;
        }
    });

    m_bot.on_voice_ready([&](const dpp::voice_ready_t& voice_ready) {
        m_onVoiceReady.Invoke();
    });
}

void Bot::CreateSlashCommands()
{
    std::cout << "Logged in as " << m_bot.me.username << "!\n";
    
    dpp::slashcommand stopCommand;
    stopCommand.set_name("stop")
               .set_description("Stop the bot")
               .set_application_id(m_bot.me.id);

    dpp::slashcommand playVideoCommand;
    playVideoCommand.set_name("play")
                    .set_description("Play a video from youtube")
                    .add_option(
                        dpp::command_option(dpp::co_string, "url", "The URL of the video", true)
                    )
                    .set_application_id(m_bot.me.id);

    dpp::slashcommand pingCommand;
    pingCommand.set_name("ping")
               .set_description("Ping the bot")
               .set_application_id(m_bot.me.id);

    dpp::slashcommand clearCommand;
    clearCommand.set_name("clear")
               .set_description("Clear the last 100 messages")
               .set_application_id(m_bot.me.id);

    /* Register the commands */
    m_bot.global_command_create(stopCommand);
    m_bot.global_command_create(playVideoCommand);
    m_bot.global_command_create(pingCommand);
    m_bot.global_command_create(clearCommand);
}

void Bot::Run()
{
    m_tickThread = std::thread(&Bot::RunThread, this);
    
    m_bot.start(false);
    
    m_isRunning = false;
    if (m_tickThread.joinable()) {
        m_tickThread.join();
    }
}

void Bot::RunThread()
{
    while (m_isRunning)
    {
        OnTick();

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void Bot::Destroy()
{
    m_bot.shutdown();
    if (m_tickThread.joinable()) {
        m_tickThread.join();
    }
}

bool Bot::JoinVocalChannel(const dpp::interaction_create_t& event)
{
    dpp::guild * g = dpp::find_guild(event.command.guild_id);
    if (!g->connect_member_voice(event.command.usr.id)) {
        m_bot.message_create(dpp::message(event.command.channel_id, "You don't seem to be on a voice channel! :("));
        return false;
    }

    return true;
}

void Bot::PlayAudio(const std::string& url, const dpp::interaction_create_t& event)
{
    if (!JoinVocalChannel(event))
        return;

    SendMessage(event.command.channel_id, "Downloading video...");
    Sound* sound = AudioPlayer::DownloadVideo(url);
    SendMessage(event.command.channel_id, "Video Downloaded");
    if (!sound)
    {
        event.reply(dpp::ir_channel_message_with_source, "Failed to download video");
        return;
    }
    SoundManager::GetInstance()->AddSound(sound->GetName(), sound->GetPath());

    auto playAudio = [&](Sound* sound, const dpp::interaction_create_t& event)
    {
        PlaySound(sound, event);
        event.reply(dpp::ir_channel_message_with_source, "Playing audio");
    };

    if (m_isVoiceReady)
    {
        playAudio(sound, event);
        return;
    }
    m_onVoiceReady.Bind([&, sound, event]() {
        playAudio(sound, event);
    });
}

bool Bot::PlaySound(Sound* sound, const dpp::interaction_create_t& event)
{
    if (!sound)
    {
        std::cout << "Sound is null" << std::endl;
        return false;
    }
    
    auto path = sound->GetPath();
    if (!std::filesystem::exists(path)) {
        std::cout << "Audio file " << path << " not found" << std::endl;
        return false;
    }
    dpp::voiceconn* v = event.from->get_voice(event.command.guild_id);

    auto timeout = std::chrono::system_clock::now() + std::chrono::seconds(20);
    
    while (!v || !v->voiceclient || !v->voiceclient->is_ready()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        if (std::chrono::system_clock::now() > timeout) {
            std::cout << "Voice client not ready" << '\n';
            return false;
        }
    }
    std::cout << "Voice client ready" << '\n';
    dpp::discord_voice_client* voiceClient = v->voiceclient;

    sound->Load();

    voiceClient->send_audio_raw(reinterpret_cast<uint16_t*>(sound->GetData()), sound->GetDataSize());

    std::cout << "Playing audio" << '\n';
    return true;
}

void Bot::SendMessage(dpp::snowflake channelId, const std::string& message)
{
    m_bot.message_create(dpp::message(channelId, message));
}

void Bot::DeleteMessage(dpp::snowflake channelId, dpp::snowflake messageId)
{
    m_bot.message_delete(messageId, channelId);
}

void Bot::DeleteLastMessages(dpp::snowflake channelId, int count)
{
    auto messages = GetMessages(channelId, count);

    for (const auto& messageID : messages | std::views::keys)
    {
        DeleteMessage(channelId, messageID);
    }
}

dpp::message_map Bot::GetMessages(dpp::snowflake channelId, int count)
{
    dpp::message_map messages;
    std::atomic_bool done = false;
    m_bot.messages_get(channelId, count, {}, {}, {},
        [&](const dpp::confirmation_callback_t& callback)
        {
            if (callback.is_error()) {
                std::cerr << "Failed to get messages: " << callback.get_error().message << std::endl;
                return;
            }

            messages = std::get<dpp::message_map>(callback.value);
            done = true;
        }
    );
    while (!done)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return messages;
}

dpp::message_map Bot::GetMessagesOfUser(dpp::snowflake channelId, dpp::snowflake userId, int count)
{
    dpp::message_map messages;
    std::atomic_bool done = false;
    m_bot.messages_get(channelId, count, {}, {}, {},
        [&](const dpp::confirmation_callback_t& callback)
        {
            if (callback.is_error()) {
                std::cerr << "Failed to get messages: " << callback.get_error().message << std::endl;
                return;
            }

            auto allMessages = std::get<dpp::message_map>(callback.value);

             for (const auto& [message_id, message] : allMessages)
            {
                if (message.author.id == userId)
                {
                    messages[message_id] = message;
                }
            }
            
            done = true;
        }
    );
    while (!done)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return messages;
}

void Bot::DeleteAllGlobalCommands()
{
    m_bot.global_commands_get([&](const dpp::confirmation_callback_t& callback) {
        if (callback.is_error()) {
            std::cerr << "Failed to fetch global commands: " 
                << callback.get_error().message << '\n';
            return;
        }

        // Get the list of commands
        auto commands = std::get<dpp::slashcommand_map>(callback.value);
        std::cout << "Found " << commands.size() << " global commands.\n";

        // Delete each command
        for (const auto& cmd : commands) {
            m_bot.global_command_delete(cmd.first, [](const dpp::confirmation_callback_t& delete_callback) {
                if (delete_callback.is_error()) {
                    std::cerr << "Failed to delete a command: " 
                        << delete_callback.get_error().message << '\n';
                } else {
                    std::cout << "Deleted command with ID: " << delete_callback.bot->cluster_id << "\n";
                }
            });
        }
    });
}

void Bot::OnInteractionCreate(const dpp::interaction_create_t& event)
{
    if (event.command.type == dpp::it_application_command)
    {
        dpp::command_interaction cmd_data = std::get<dpp::command_interaction>(event.command.data);

        if (cmd_data.name == "ping")
        {
            event.reply(dpp::ir_channel_message_with_source, "Pong!");
        }
        else if (cmd_data.name == "stop")
        {
            event.reply(dpp::ir_channel_message_with_source, "Bot is stopping...");
            m_bot.shutdown();
        }
        else if (cmd_data.name == "play")
        {
            OnPlay(event);
        }
        else if (cmd_data.name == "clear")
        {
            DeleteLastMessages(event.command.channel_id, 100);
        }
    }
}

void Bot::OnPlay(const dpp::interaction_create_t& event)
{
    std::string url = std::get<std::string>(event.get_parameter("url"));

    if (url.empty()) {
        event.reply(dpp::ir_channel_message_with_source, "No url provided");
        return;
    }

    PlayAudio(url, event);
}

void Bot::OnTick()
{
}

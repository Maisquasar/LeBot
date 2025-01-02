#include <dpp/dpp.h>

#include "AudioPlayer.h"

using path = std::filesystem::path;

#define TOKEN "MTMyNDEwNDA3NzQ2NTg3ODY5MA.GtQzGf.K_GL9va3aSYcmjcZud9KYLyYyaBA52dnnfh4PY"


void CreateSlashCommands(dpp::cluster& bot)
{
    std::cout << "Logged in as " << bot.me.username << "!\n";
    
    dpp::slashcommand stopCommand;
    stopCommand.set_name("stop")
        .set_description("Stop the bot")
        .set_application_id(bot.me.id);

    dpp::slashcommand playVideoCommand;
    playVideoCommand.set_name("play")
        .set_description("Play a video from youtube")
        .add_option(
        dpp::command_option(dpp::co_string, "url", "The URL of the video", true)
        )
        .set_application_id(bot.me.id);

    dpp::slashcommand pingCommand;
    pingCommand.set_name("ping")
        .set_description("Ping the bot")
        .set_application_id(bot.me.id);

    /* Register the commands */
    bot.global_command_create(stopCommand);
    bot.global_command_create(playVideoCommand);
    bot.global_command_create(pingCommand);
}

// Function to delete all global commands
void DeleteAllGlobalCommands(dpp::cluster& bot) {
    bot.global_commands_get([&bot](const dpp::confirmation_callback_t& callback) {
        if (callback.is_error()) {
            std::cerr << "Failed to fetch global commands: " 
                      << callback.get_error().message << std::endl;
            return;
        }

        // Get the list of commands
        auto commands = std::get<dpp::slashcommand_map>(callback.value);
        std::cout << "Found " << commands.size() << " global commands.\n";

        // Delete each command
        for (const auto& cmd : commands) {
            bot.global_command_delete(cmd.first, [](const dpp::confirmation_callback_t& delete_callback) {
                if (delete_callback.is_error()) {
                    std::cerr << "Failed to delete a command: " 
                              << delete_callback.get_error().message << std::endl;
                } else {
                    std::cout << "Deleted command with ID: " << delete_callback.bot->cluster_id << "\n";
                }
            });
        }
    });
}

bool JoinVocalChannel(dpp::cluster& bot, const dpp::interaction_create_t &event) 
{
    dpp::guild * g = dpp::find_guild(event.command.guild_id);
    if (!g->connect_member_voice(event.command.usr.id)) {
        bot.message_create(dpp::message(event.command.channel_id, "You don't seem to be on a voice channel! :("));
        return false;
    }

    return true;
}

void OnPlay(dpp::cluster& bot, const dpp::interaction_create_t& event)
{
    std::string url = std::get<std::string>(event.get_parameter("url"));

    if (url.empty()) {
        event.reply(dpp::ir_channel_message_with_source, "No url provided");
        return;
    }

    if (!JoinVocalChannel(bot, event))
        return;

    path outputPath;
    Sound* sound = AudioPlayer::DownloadVideo(url);
    SoundManager::GetInstance()->AddSound(sound->GetName(), sound->GetPath());
    if (!sound)
    {
        event.reply(dpp::ir_channel_message_with_source, "Failed to download video");
        return;
    }
            
    bot.on_voice_ready([&bot, &event, sound](const dpp::voice_ready_t& voice_ready) {
        if (!AudioPlayer::PlayAudio(bot, event, sound))
        {
            event.reply(dpp::ir_channel_message_with_source, "Failed to play audio");
            return;
        }
        delete sound;

        event.reply(dpp::ir_channel_message_with_source, "Audio played successfully!");
    });
}

void OnInteractionCreate(dpp::cluster& bot, const dpp::interaction_create_t &event)
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
            bot.shutdown();
        }
        else if (cmd_data.name == "play")
        {
            OnPlay(bot, event);
        }
    }
}

std::atomic<bool> running{true};

void TickFunction()
{
    while (running) {
        // Perform tick update tasks here
        std::cout << "Tick update" << std::endl;

        // Sleep for a specific interval, e.g., 1 second
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int Main(int argc, char** argv)
{
    //TODO : Make class to organize this code !!!
    // TODO : Add queue system
    // TODO : Handle playlist
    // TODO : Handle other website than youtube
    SoundManager::Initialize();
    try
    {
        // auto path = std::filesystem::current_path() / "data" / "audio.mp4";
        // AudioPlayer::DownloadVideo("https://www.youtube.com/watch?v=KfT5qVREqlM", path);
        dpp::cluster bot(TOKEN);

        bot.on_interaction_create([&bot](const dpp::interaction_create_t &event) {
            OnInteractionCreate(bot, event);
        });

        bot.on_ready([&bot](const dpp::ready_t &event) {
            // DeleteAllGlobalCommands(bot);
            CreateSlashCommands(bot);
        });

        // Start the tick function in a separate thread
        std::thread tickThread(TickFunction);

        bot.start(false);

        // Wait for bot to stop before terminating the tick thread
        running = false;
        if (tickThread.joinable()) {
            tickThread.join();
        }
    }
    catch (dpp::exception e)
    {
        std::cout << "Caught exception: " << e.what() << "\n";
    }

    SoundManager::Destroy();
    return 0;
}
int main(int argc, char** argv)
{
#ifdef _WIN32
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    //  TODO: Remove Comments To Break on leaks
    // |
    // V
    //_CrtSetBreakAlloc(863);
#endif
    Main(argc, argv);
    return 0;
}

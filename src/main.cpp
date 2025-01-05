#include <dpp/dpp.h>

#include "AudioPlayer.h"
#include "Bot.h"
#include "ThreadManager.h"

#define TOKEN "MTMyNDEwNDA3NzQ2NTg3ODY5MA.GtQzGf.K_GL9va3aSYcmjcZud9KYLyYyaBA52dnnfh4PY"

int Main(int argc, char** argv)
{
    // TODO : Handle playlist
    // TODO : Handle other website than youtube
    auto threadManager = ThreadManager::GetInstance();
    threadManager->Initialize();
    SoundManager::Initialize();
    try
    {
        // auto path = std::filesystem::current_path() / "data" / "audio.mp4";
        // AudioPlayer::DownloadVideo("https://www.youtube.com/watch?v=KfT5qVREqlM", path);
        Bot bot(TOKEN);
        
        bot.Initialize();

        bot.Run();
    }
    catch (dpp::exception e)
    {
        std::cout << "Caught exception: " << e.what() << "\n";
    }

    SoundManager::Destroy();
    threadManager->Destroy();
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

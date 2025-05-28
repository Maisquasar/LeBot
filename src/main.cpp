#include <iostream>

#include "Bot.h"

#include <nlohmann/json.hpp>

static std::string GetBotToken()
{
    std::ifstream file("tokens.json");
    nlohmann::json config;
    file >> config;
    return config["bot-token"];
}


int Main(int argc, char** argv)
{
    try
    {
        std::string TOKEN = GetBotToken();
        Bot bot(TOKEN);
        
        bot.Initialize();

        bot.Run();

        bot.Destroy();
    }
    catch (dpp::exception e)
    {
        std::cout << "Caught exception: " << e.what() << "\n";
    }
    
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

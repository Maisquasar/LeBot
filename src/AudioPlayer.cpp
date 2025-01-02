#include "AudioPlayer.h"

#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>

#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#endif

// Function to execute a shell command and capture its output
std::string execCommand(const std::string& command) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

Sound::Sound(const std::string& name, const std::filesystem::path& path) : m_name(name), m_path(path), m_length(0)
{
}

bool Sound::Load()
{
    std::ifstream audioFile(m_path, std::ios::in|std::ios::binary|std::ios::ate);
    if (audioFile.is_open()) {
        m_size = audioFile.tellg();
        m_data = new uint8_t[m_size];
        audioFile.seekg (0, std::ios::beg);
        audioFile.read(reinterpret_cast<char*>(m_data), m_size);
        audioFile.close();
        return true;
    }
    else {
        std::cout << "Failed to read audio file" << std::endl;
        return false;
    }
}

SoundManager* SoundManager::m_instance = nullptr;

Sound* AudioPlayer::DownloadVideo(const std::string& url)
{
    if (url.empty()) {
        std::cout << "No video provided" << std::endl;
        return nullptr;
    }

    if (!std::filesystem::exists(YTP_DLP_PATH)) {
        std::cout << "yt-dlp not found" << std::endl;
        return nullptr;
    }

    if (!std::filesystem::exists(OUTPUT_PATH)) {
        std::filesystem::create_directory(OUTPUT_PATH);
    }

    std::filesystem::path currentPath = std::filesystem::current_path();
    std::string currentPathString = currentPath.generic_string();
    std::filesystem::path fileOutputPath;
    std::string videoTitle;
#ifdef _WIN32
    {
        // Construct the yt-dlp command
        std::string command = "cmd /C \"" + currentPathString + "/" YTP_DLP_PATH "\" --get-id " + url;

        // Execute the command and capture the output
        videoTitle = execCommand(command);

        // Remove the trailing newline character
        videoTitle.erase(videoTitle.find_last_not_of(" \t\r\n") + 1);

        std::filesystem::path title = videoTitle;

        fileOutputPath = (currentPath / OUTPUT_PATH / title).generic_string() + ".wav";
    }
#else
    TODO
#endif
    
    if (std::filesystem::exists(fileOutputPath.generic_string() + ".pcm")) {
        std::cout << "Video already downloaded" << '\n';
        Sound* outputSound = new Sound(videoTitle, fileOutputPath.generic_string() + ".pcm");
        return outputSound;
    }

#ifdef _WIN32
    std::string ffmpegLoc = "--ffmpeg-location " + currentPath.parent_path().generic_string() + "/libs/ffmpeg/bin";
    std::string outputPath = "-o " + currentPathString + "/" OUTPUT_PATH + "%(id)s.%(ext)s";
    std::string command = "cmd /C \"" + currentPathString + "/" YTP_DLP_PATH "\" " + ffmpegLoc + " --extract-audio --audio-format wav " + outputPath + " ";
#else
    TODO
#endif
    command += url;
    
    std::system(command.c_str());

    // Convert to raw pcm
    {
        std::string outputPath = fileOutputPath.generic_string();
        // 48kHz pcm 2 channels 16 bit
        std::string ffmpegCommand = "cmd /C " + currentPath.parent_path().generic_string() + "/libs/ffmpeg/bin/ffmpeg.exe -i " + outputPath + " -acodec pcm_s16le -f s16le -ac 2 -ar 48000 " + outputPath + ".pcm";
        std::system(ffmpegCommand.c_str());

        std::remove(fileOutputPath.generic_string().c_str());

        fileOutputPath = fileOutputPath.generic_string() + ".pcm";
    }
    Sound* outputSound = new Sound(videoTitle, fileOutputPath);

    if (!std::filesystem::exists(fileOutputPath)) {
        std::cout << "Failed to convert video " << fileOutputPath << " to pcm" << '\n';
        return nullptr;
    }

    std::cout << "Video downloaded" << '\n';
    
    return outputSound;
}

bool AudioPlayer::PlayAudio(const dpp::cluster& bot, const dpp::interaction_create_t& event, Sound* sound)
{
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
            std::cout << "Voice client not ready" << std::endl;
            return false;
        }
    }
    std::cout << "Voice client ready" << std::endl;
    dpp::discord_voice_client* voiceClient = v->voiceclient;

    sound->Load();

    voiceClient->send_audio_raw(reinterpret_cast<uint16_t*>(sound->GetData()), sound->GetDataSize());

    std::cout << "Playing audio" << '\n';
    return true;
}

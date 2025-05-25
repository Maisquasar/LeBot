#include "YoutubeDL.h"

#include <iostream>

#include "Bot.h"

#define TOOL_DIR "tools/"
#define YT_DLP_EXE "yt-dlp.exe"
#define YT_DLP_PATH TOOL_DIR YT_DLP_EXE
#define OUTPUT_DIR  "output/"
#define FFMPEG_PATH TOOL_DIR "ffmpeg/bin/ffmpeg.exe"


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

std::string YoutubeDL::GetVideoID(const std::string& url)
{
    const std::string baseDir = std::filesystem::current_path().generic_string();
    const std::string cmd = 
        "cd " + baseDir + "/" TOOL_DIR +
        " && " YT_DLP_EXE " --ffmpeg-location \"" + baseDir + "/" FFMPEG_PATH +
        "\" --get-id " + url;

    std::string videoID = execCommand(cmd);
    videoID.erase(videoID.find_last_not_of(" \t\r\n") + 1);
    return videoID;
}

std::filesystem::path YoutubeDL::DownloadVideo(Bot* bot, const std::string& url)
{
    const auto cwd = std::filesystem::current_path();
    const auto ytDlpPath = cwd / YT_DLP_PATH;
    if (!std::filesystem::exists(ytDlpPath)) {
        bot->Log("yt-dlp not found in {}", YT_DLP_PATH);
        return {};
    }

    const auto outputDir = OUTPUT_DIR;
    if (!std::filesystem::exists(outputDir)) {
        std::filesystem::create_directory(outputDir);
    }

    std::string videoID = GetVideoID(url);

    const std::string outputPath = (cwd / outputDir / videoID).generic_string() + ".ogg";

    ExtractVideo(bot, url, outputPath);

    return "";
}

void YoutubeDL::ExtractVideo(Bot* bot, const std::string& url, const std::filesystem::path& outputPath)
{
    const std::string baseDir = std::filesystem::current_path().generic_string();
    const std::string cmd = 
        "cd " + baseDir + "/" TOOL_DIR +
        " && " YT_DLP_EXE " --ffmpeg-location \"" + baseDir + "/" FFMPEG_PATH + "\" --extract-audio --remux-video ogg -o \"" + outputPath.generic_string() + "\" " + url;

    std::system(cmd.c_str());

    bot->Log("Video downloaded to {}", outputPath.string());
}


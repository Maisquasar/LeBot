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
std::string ExecCommand(const std::string& command) {
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

    std::string videoID = ExecCommand(cmd);
    videoID.erase(videoID.find_last_not_of(" \t\r\n") + 1);
    return videoID;
}

std::string YoutubeDL::GetVideoTitle(const std::string& url)
{
    const std::string baseDir = std::filesystem::current_path().generic_string();
    const std::string cmd = 
        "cd " + baseDir + "/" TOOL_DIR +
        " && " YT_DLP_EXE " --ffmpeg-location \"" + baseDir + "/" FFMPEG_PATH +
        "\" --get-title " + url;

    std::string videoTitle = ExecCommand(cmd);
    videoTitle.erase(videoTitle.find_last_not_of(" \t\r\n") + 1);
    return videoTitle;
}

std::filesystem::path YoutubeDL::DownloadVideo(const std::string& url)
{
    const auto cwd = std::filesystem::current_path();
    const auto ytDlpPath = cwd / YT_DLP_PATH;
    if (!std::filesystem::exists(ytDlpPath))
    {
        return {};
    }

    const auto outputDir = OUTPUT_DIR;
    if (!std::filesystem::exists(outputDir)) {
        std::filesystem::create_directory(outputDir);
    }

    std::string videoID = GetVideoID(url);

    const std::string outputPath = (cwd / outputDir / videoID).generic_string() + ".ogg";

    if (!std::filesystem::exists(outputPath))
        ExtractVideo(url, outputPath);

    return outputPath;
}

void YoutubeDL::ExtractVideo(const std::string& url, const std::filesystem::path& outputPath)
{
    const std::string baseDir = std::filesystem::current_path().generic_string();
    const std::string cmd = 
        "cd " + baseDir + "/" TOOL_DIR +
        " && " YT_DLP_EXE " --ffmpeg-location \"" + baseDir + "/" FFMPEG_PATH + "\" --extract-audio --remux-video ogg -o \"" + outputPath.generic_string() + "\" " + url;

    std::system(cmd.c_str());
}


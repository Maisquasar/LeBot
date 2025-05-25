#pragma once
#include <filesystem>

class Bot;

class YoutubeDL
{
public:
    static std::string GetVideoID(const std::string& url);
    static std::filesystem::path DownloadVideo(Bot* bot, const std::string& url);
private:
    static void ExtractVideo(Bot* bot, const std::string& url, const std::filesystem::path& outputPath);
};

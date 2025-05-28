#pragma once
#include <filesystem>

class Bot;

class YoutubeDL
{
public:
    static std::string GetVideoID(const std::string& url);
    static std::string GetVideoTitle(const std::string& url);
    static std::filesystem::path DownloadVideo(const std::string& url);
private:
    static void ExtractVideo(const std::string& url, const std::filesystem::path& outputPath);
};

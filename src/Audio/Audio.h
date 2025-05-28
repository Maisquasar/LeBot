#pragma once
#include <filesystem>
#include <string>
#include <dpp/discordvoiceclient.h>

#include <opus/opusfile.h>

#include "Event.h"

class Bot;

class Audio
{
public:
    Audio(const std::filesystem::path& path);
    Audio(const std::string& name, const std::filesystem::path& path);
    ~Audio();

    bool Load();
    void Unload();
    
    void Play(dpp::discord_voice_client* voiceclient);

    std::string GetName() const { return m_name; }
    std::string GetID() const { return m_id; }
    std::filesystem::path GetPath() const { return m_path; }

    Event<> EOnLoad;
private:
    std::filesystem::path m_path;
    std::string m_name;
    std::string m_id;

    std::atomic<bool> m_isLoaded = false;

    double m_length = 0.0;
    
    OpusHead header;
    ogg_sync_state oy; 
    ogg_stream_state os;
    ogg_page og;
    ogg_packet op;
    int samples;
};

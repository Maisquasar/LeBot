#pragma once
#include <filesystem>
#include <string>

#include <opus/opusfile.h>

class Audio
{
public:
private:
    std::filesystem::path m_path;
    std::string m_name;
    std::string m_id;

    double m_length = 0.0;
    
    OpusHead header;
    ogg_sync_state oy; 
    ogg_stream_state os;
    ogg_page og;
    ogg_packet op;
    int samples;
};

#include "Audio.h"

#include <iostream>

Audio::Audio(const std::filesystem::path& path) : m_path(path)
{
}

Audio::Audio(const std::string& name, const std::filesystem::path& path): m_name(name), m_path(path)
{
    
}

Audio::~Audio()
{
    std::cout << "Audio::~Audio()" << "\n";
    if (m_isLoaded.load())
    {
        Unload();
    }
}

bool Audio::Load()
{
    if (m_isLoaded.load())
    {
        std::cout << "Audio is already loaded." << "\n";
        return true;
    }
    char *buffer;

    FILE *fd;

    fd = fopen(m_path.generic_string().c_str(), "rb");

    fseek(fd, 0L, SEEK_END);
    size_t sz = ftell(fd);
    rewind(fd);

    ogg_sync_init(&oy);

    int eos = 0;
    int i;

    buffer = ogg_sync_buffer(&oy, sz);
    fread(buffer, 1, sz, fd);

    ogg_sync_wrote(&oy, sz);

    if (ogg_sync_pageout(&oy, &og) != 1)
    {
        std::cout << "Does not appear to be ogg stream." << "\n";
        
        return false;
    }

    ogg_stream_init(&os, ogg_page_serialno(&og));

    if (ogg_stream_pagein(&os,&og) < 0) {
        std::cout << "Error reading initial page of ogg stream." << "\n";
        
        return false;
    }

    if (ogg_stream_packetout(&os,&op) != 1)
    {
        std::cout << "Error reading header packet of ogg stream." << "\n";
        
        return false;
    }

    /* We must ensure that the ogg stream actually contains opus data */
    if (!(op.bytes > 8 && !memcmp("OpusHead", op.packet, 8)))
    {
        std::cout << "Not an ogg opus stream." << "\n";
        
        return false;
    }

    /* Parse the header to get stream info */
    int err = opus_head_parse(&header, op.packet, op.bytes);
    if (err)
    {
        std::cout << "Not a ogg opus stream" << "\n";
        
        return false;
    }
    /* Now we ensure the encoding is correct for Discord */
    if (header.channel_count != 2 && header.input_sample_rate != 48000)
    {
        std::cout << "Wrong encoding for Discord, must be 48000Hz sample rate with 2 channels." << "\n";
        
        return false;
    }

    m_isLoaded.store(true);
    EOnLoad.Invoke();
    return true;
}

void Audio::Unload()
{
    m_isLoaded.store(false);
    ogg_stream_clear(&os);
    ogg_sync_clear(&oy);
}

void Audio::Play(dpp::discord_voice_client* voiceclient)
{
    double duration = 0;
    /* Now loop though all the pages and send the packets to the vc */
    while (ogg_sync_pageout(&oy, &og) == 1){
        ogg_stream_init(&os, ogg_page_serialno(&og));

        if(ogg_stream_pagein(&os,&og)<0){
            std::cout << "Error reading page of Ogg bitstream data." << "\n";
            exit(1);
        }

        while (ogg_stream_packetout(&os,&op) != 0)
        {
            /* Read remaining headers */
            if (op.bytes > 8 && !memcmp("OpusHead", op.packet, 8))
            {
                int err = opus_head_parse(&header, op.packet, op.bytes);
                if (err)
                {
                    std::cout << "Not a ogg opus stream" << "\n";
                    return;
                }
                if (header.channel_count != 2 && header.input_sample_rate != 48000)
                {
                    std::cout << "Wrong encoding for Discord, must be 48000Hz sample rate with 2 channels." << "\n";
                    return;
                }
                continue;
            }
            /* Skip the opus tags */
            if (op.bytes > 8 && !memcmp("OpusTags", op.packet, 8))
                continue; 

            samples = opus_packet_get_samples_per_frame(op.packet, 48000);

            duration += static_cast<double>(samples) / header.input_sample_rate;
            
            voiceclient->send_audio_opus(op.packet, op.bytes, samples / 48);
        }
    }
    m_length = duration;
}

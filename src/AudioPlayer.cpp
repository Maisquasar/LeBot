#include "AudioPlayer.h"

#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>

#include <ogg/ogg.h>
#include <opus/opusfile.h>

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

Sound::Sound(const std::string& name, const std::filesystem::path& path, const std::string& url)
    : m_path(path), m_name(name), m_url(url), m_length(0)
{
}

Sound::~Sound()
{
    Unload();
}

bool Sound::Load(dpp::discord_voice_client* voiceclient)
{
    OpusHead header;
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
                    return false;
                }
                if (header.channel_count != 2 && header.input_sample_rate != 48000)
                {
                    std::cout << "Wrong encoding for Discord, must be 48000Hz sample rate with 2 channels." << "\n";
                    return false;
                }
                continue;
            }
            /* Skip the opus tags */
            if (op.bytes > 8 && !memcmp("OpusTags", op.packet, 8))
                continue; 

            samples = opus_packet_get_samples_per_frame(op.packet, 48000);
            
            voiceclient->send_audio_opus(op.packet, op.bytes, samples / 48);
        }
    }
    return true;
}

void Sound::Unload()
{
    /* Cleanup */
    ogg_stream_clear(&os);
    ogg_sync_clear(&oy);
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

        fileOutputPath = (currentPath / OUTPUT_PATH / title).generic_string() + ".ogg";
    }
#else
    TODO
#endif

    if (std::filesystem::exists(fileOutputPath)) {
        std::cout << "Video " << url << " already downloaded" << '\n';
        Sound* outputSound = new Sound(videoTitle, fileOutputPath, url);
        return outputSound;
    }

#ifdef _WIN32
    std::string ffmpegLoc = "--ffmpeg-location " + currentPath.parent_path().generic_string() + "/libs/ffmpeg/bin";
    std::string outputPath = "-o " + currentPathString + "/" OUTPUT_PATH + "%(id)s.%(ext)s";
    std::string command = "cmd /C \"" + currentPathString + "/" YTP_DLP_PATH "\" " + ffmpegLoc + " --extract-audio --remux-video ogg " + outputPath + " ";
#else
    TODO
#endif
    command += url;
    
    std::system(command.c_str());

    /*
    // Convert to raw pcm
    {
        std::string outputPath = fileOutputPath.generic_string();
        // 48kHz pcm 2 channels 16 bit
        std::string ffmpegCommand = "cmd /C " + currentPath.parent_path().generic_string() + "/libs/ffmpeg/bin/ffmpeg.exe -i " + outputPath + " -acodec pcm_s16le -f s16le -ac 2 -ar 48000 " + outputPath + ".pcm";
        std::system(ffmpegCommand.c_str());

        std::remove(fileOutputPath.generic_string().c_str());

        fileOutputPath = fileOutputPath.generic_string() + ".pcm";
    }
    */
    Sound* outputSound = new Sound(videoTitle, fileOutputPath, url);

    if (!std::filesystem::exists(fileOutputPath)) {
        std::cout << "Failed to convert video " << fileOutputPath << " to ogg" << '\n';
        return nullptr;
    }

    std::cout << "Video " << url << " downloaded" << '\n';
    
    return outputSound;
}

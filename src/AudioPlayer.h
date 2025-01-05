#pragma once
#include <cluster.h>
#include <filesystem>
#include <string>
#include <ogg/ogg.h>
#include <opus/opusfile.h>

#define OUTPUT_PATH "audio/"

#ifdef _WIN32
#define YTP_DLP_PATH "../libs/yt-dlp/yt-dlp.exe"
#else
TODO
#endif

class Sound
{
public:
    explicit Sound(const std::string& name, const std::filesystem::path& path, const std::string& url);
    ~Sound();

    bool Load();
    void Play(dpp::discord_voice_client* voiceclient);
    void Unload();

    std::filesystem::path GetPath() const { return m_path; }
    std::string GetName() const { return m_name; }
    double GetLength() const { return m_length; }
    std::string GetURL() const { return m_url; }

    ogg_packet* GetPacket() { return &op; }
    int GetSamples() const { return samples; }
private:
    std::filesystem::path m_path;
    std::string m_name;
    std::string m_url;
    double m_length;

    OpusHead header;
    ogg_sync_state oy; 
    ogg_stream_state os;
    ogg_page og;
    ogg_packet op;
    int samples;
};

class SoundManager
{
public:
    SoundManager() = default;
    ~SoundManager() = default;

    static SoundManager* GetInstance() { return m_instance; }

    static void Initialize() { m_instance = new SoundManager(); }

    static void Destroy() { delete m_instance; }

    bool AddSound(const std::string& name, Sound* sound) { return m_sounds[name] = sound; }

    Sound* GetSound(const std::string& name) { return m_sounds[name]; }
    std::map<std::string, Sound*> GetSounds() { return m_sounds; }
private:
    static SoundManager* m_instance;

    std::map<std::string, Sound*> m_sounds;
};

class AudioPlayer
{
public:
    static Sound* DownloadVideo(const std::string& url);
};

#pragma once
#include <cluster.h>
#include <filesystem>
#include <string>

#define OUTPUT_PATH "audio/"

#ifdef _WIN32
#define YTP_DLP_PATH "../libs/yt-dlp/yt-dlp.exe"
#else
TODO
#endif

class Sound
{
public:
    explicit Sound(const std::string& name, const std::filesystem::path& path);
    ~Sound() = default;

    bool Load();

    std::filesystem::path GetPath() const { return m_path; }
    std::string GetName() const { return m_name; }
    uint32_t GetLength() const { return m_length; }

    uint8_t* GetData() const { return m_data; }
    size_t GetDataSize() const { return m_size; }
private:
    std::filesystem::path m_path;
    std::string m_name;
    uint32_t m_length;
    size_t m_size = 0;
    uint8_t* m_data = nullptr;
};

class SoundManager
{
public:
    SoundManager() = default;
    ~SoundManager() = default;

    static SoundManager* GetInstance() { return m_instance; }

    static void Initialize() { m_instance = new SoundManager(); }

    static void Destroy() { delete m_instance; }

    bool AddSound(const std::string& name, const std::filesystem::path& path) { return m_sounds[name] = new Sound(name, path); }

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

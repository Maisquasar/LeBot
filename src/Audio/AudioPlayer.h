#pragma once
#include <memory>
#include <queue>

#include "Audio.h"

class AudioPlayer
{
public:
    AudioPlayer(Bot* bot) : m_bot(bot) {}
    
    void AddSong(const std::string& url);

    void AddPlaylist(const std::string& url);

    void ThreadLoop();

    void Start();

    std::shared_ptr<Audio> GetCurrentAudio() { return m_currentAudio; }
    std::queue<std::shared_ptr<Audio>>& GetAudioQueue() { return m_audioQueue; }
private:
    Bot* m_bot;
    std::shared_ptr<Audio> m_currentAudio;
    std::queue<std::shared_ptr<Audio>> m_audioQueue;
};

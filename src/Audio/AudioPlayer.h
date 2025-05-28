#pragma once
#include <memory>
#include <queue>

#include "Audio.h"

class AudioPlayer
{
public:
    AudioPlayer(Bot* bot) : m_bot(bot) {}
    
    void AddSong(const std::string& url);

    void ThreadLoop();

    void Start();

private:
    Bot* m_bot;
    std::queue<std::shared_ptr<Audio>> m_audioQueue;
};

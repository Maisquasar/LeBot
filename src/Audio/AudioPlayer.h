#pragma once
#include <memory>
#include <queue>

#include "Audio.h"

class AudioPlayer
{
public:
    void AddSong(std::unique_ptr<Audio> audio);

private:
    std::queue<std::unique_ptr<Audio>> audioQueue;
};

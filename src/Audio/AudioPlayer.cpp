#include "AudioPlayer.h"

#include "Bot.h"
#include "ThreadManager.h"
#include "External/YoutubeDL.h"

// void AudioPlayer::AddSong(const std::shared_ptr<Audio>& audio)
// {
//     m_audioQueue.push(audio);
// }

void AudioPlayer::AddSong(const std::string& url)
{
    std::filesystem::path path = YoutubeDL::DownloadVideo(url);

    if (path.empty()) {
        m_bot->Log("Failed to download video {}", url);
        return;
    }
    
    std::shared_ptr<Audio> audio = std::make_shared<Audio>(path);
    
    audio->EOnLoad += [this, audio]() { m_audioQueue.push(audio); };

    ThreadManager::AddTask(&Audio::Load, audio.get());
}

void AudioPlayer::ThreadLoop()
{
    while (m_bot->IsRunning()) {
        if (m_audioQueue.empty() || !m_bot->IsVoiceReady()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            continue;
        }

        std::shared_ptr<Audio> audio = m_audioQueue.front();
        m_audioQueue.pop();

        m_bot->PlayAudio(audio.get());
    }   
}

void AudioPlayer::Start()
{
    ThreadManager::AddTask(&AudioPlayer::ThreadLoop, this);
}
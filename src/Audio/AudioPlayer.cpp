#include "AudioPlayer.h"

#include "Bot.h"
#include "ThreadManager.h"
#include "External/YoutubeDL.h"

void AudioPlayer::AddSong(const std::string& url)
{
    std::filesystem::path path = YoutubeDL::DownloadVideo(url);

    if (path.empty()) {
        m_bot->Log("Failed to download video {}", url);
        return;
    }
    
    std::shared_ptr<Audio> audio = std::make_shared<Audio>(YoutubeDL::GetVideoTitle(url), path);
    
    audio->EOnLoad += [this, audio]() { m_audioQueue.push(audio); };

    ThreadManager::AddTask(&Audio::Load, audio.get());
}

void AudioPlayer::AddPlaylist(const std::string& url)
{
    m_bot->Log("Getting urls from playlist {}", url);
    auto urls = YoutubeDL::GetURLsFromPlaylist(url);
    
    m_bot->Log("Got {} urls", urls.size());
    for (const auto& url : urls) {
        m_bot->Log("Adding song {}", url);
        AddSong(url);
    }
}


void AudioPlayer::ThreadLoop()
{
    while (m_bot->IsRunning()) {
        if (m_audioQueue.empty() || !m_bot->IsVoiceReady()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            continue;
        }

        ThreadManager::Lock();
        m_currentAudio = m_audioQueue.front();
        m_audioQueue.pop();
        ThreadManager::Unlock();

        m_bot->PlayAudio(m_currentAudio.get());
    }   
}

void AudioPlayer::Start()
{
    ThreadManager::AddTask(&AudioPlayer::ThreadLoop, this);
}
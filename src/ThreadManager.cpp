﻿#include "ThreadManager.h"

std::unique_ptr<ThreadManager> ThreadManager::m_instance;

void ThreadManager::Initialize()
{
    m_mainThreadID = std::this_thread::get_id();
    m_threadList.resize(std::thread::hardware_concurrency());
    for (std::thread& i : m_threadList)
    {
        i = std::thread(&ThreadManager::ThreadLoop, this);
    }
}

void ThreadManager::ThreadLoop()
{
    while (!m_terminate)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        if (!m_tasks.empty())
        {
            Lock();
            if (!m_tasks.empty()) {
                const std::function<void()> task = m_tasks.front();
                m_tasks.pop();
                Unlock();
                if (task != nullptr)
                    task();
                break;
            }
            Unlock();
        }
    }
    if (!m_terminate)
        ThreadLoop();
}

void ThreadManager::Lock()
{
    while (!m_instance->m_mutex.try_lock()) {}
}

void ThreadManager::Unlock()
{
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 26110)
#endif
    m_instance->m_mutex.unlock();
#ifdef _MSC_VER
#pragma warning(pop) 
#endif
}

ThreadManager* ThreadManager::GetInstance()
{
    if (m_instance == nullptr) {
        m_instance = std::make_unique<ThreadManager>();
    }
    return m_instance.get();
}

void ThreadManager::Destroy()
{
    Terminate();
    // clear threadList
    while (!m_threadList.empty())
    {
        for (size_t i = 0; i < m_threadList.size(); i++) {
            if (m_threadList[i].joinable())
            {
                m_threadList[i].join();
                m_threadList.erase(m_threadList.begin() + i--);
            }
        }
    }
}

bool ThreadManager::IsMainThread()
{
    return std::this_thread::get_id() == m_instance->m_mainThreadID;
}

void ThreadManager::ForceLock()
{
    m_instance->m_mutex.lock();
}

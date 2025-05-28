#pragma once
#include <thread>
#include <queue>
#include <functional>
#include <mutex>

class ThreadManager
{
public:
    ~ThreadManager() = default;

    void Initialize();
    void Destroy();

    void ThreadLoop();

    void Terminate() { m_terminate = true; }

    template <typename F, typename... A> static void AddTask(F&& task, A&&... args)
    {
        std::function<void()> task_function = [task = std::forward<F>(task), args = std::make_tuple(std::forward<A>(args)...)]() mutable {
            std::apply(task, args);
        };
        if (task_function) {
            std::lock_guard lock(m_instance->m_mutex);
            m_instance->m_tasks.push(task_function);
        }
    }

    static void Lock();
    static void ForceLock();
    static void Unlock();

    static bool IsMainThread();

    static ThreadManager* GetInstance();

    static bool ShouldTerminate() { return m_instance->m_terminate; }
    static std::mutex& GetMutex() { return  m_instance->m_mutex; }
private:
    static std::unique_ptr<ThreadManager> m_instance;

    std::thread::id m_mainThreadID = std::this_thread::get_id();
    std::queue<std::function<void()>> m_tasks = {};
    std::vector<std::thread> m_threadList = {};
    std::mutex m_mutex;
    std::atomic_bool m_locked = false;
    bool m_terminate = false;

};

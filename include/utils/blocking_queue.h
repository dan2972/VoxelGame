#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <stdexcept>

template<typename T>
class BlockingQueue
{

public:
    BlockingQueue() = default;
    ~BlockingQueue() = default;

    void push(const T& item) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(item);
        m_condVar.notify_one();
    }

    void pop(T& item) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_condVar.wait(lock, [this] { return !m_queue.empty(); });
        item = m_queue.front();
        m_queue.pop();
    }

    bool pop(T& item, int timeoutMs) {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (!m_condVar.wait_for(lock, std::chrono::milliseconds(timeoutMs), [this] { return !m_queue.empty(); }))
            return false;
        
        item = m_queue.front();
        m_queue.pop();
        return true;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.size();
    }
private:
    mutable std::mutex m_mutex;
    std::condition_variable m_condVar;
    std::queue<T> m_queue;    
};

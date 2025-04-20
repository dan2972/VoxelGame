#pragma once

#include <deque>
#include <mutex>
#include <condition_variable>
#include <stdexcept>

template<typename T>
class BlockingDeque
{

public:
    BlockingDeque() = default;
    ~BlockingDeque() = default;

    void pushFront(const T& item) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push_front(item);
        m_condVar.notify_one();
    }

    void pushBack(const T& item) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push_back(item);
        m_condVar.notify_one();
    }

    void popFront(T& item) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_condVar.wait(lock, [this] { return !m_queue.empty(); });
        item = m_queue.front();
        m_queue.pop_front();
    }

    bool popFront(T& item, int timeoutMs) {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (!m_condVar.wait_for(lock, std::chrono::milliseconds(timeoutMs), [this] { return !m_queue.empty(); }))
            return false;
        
        item = m_queue.front();
        m_queue.pop_front();
        return true;
    }

    bool popFrontNoWait(T& item) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_queue.empty())
            return false;
        
        item = m_queue.front();
        m_queue.pop_front();
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

    void clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        while (!m_queue.empty()) {
            m_queue.pop();
        }
    }
private:
    mutable std::mutex m_mutex;
    std::condition_variable m_condVar;
    std::deque<T> m_queue;    
};

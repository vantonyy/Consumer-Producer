#ifndef THREAD_SAFE_QUEUE_HPP
#define THREAD_SAFE_QUEUE_HPP

#include <cassert>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

namespace consumerProducer {

//@class threadSafeQueue
class threadSafeQueue
{
public:
	typedef int valueType;
private:
	enum sizeConstraints {
		maxSize = 100,
		allowedSize = 80
	};
public:
	threadSafeQueue() = default;
	threadSafeQueue(threadSafeQueue&&) = delete;
	threadSafeQueue(const threadSafeQueue&) = delete;
	threadSafeQueue& operator=(threadSafeQueue&&) = delete;
	threadSafeQueue& operator=(const threadSafeQueue&) = delete;
public:
	//@brief removes the first element
	//@returns false if queue is empty, true otherwise
	bool pop(valueType& value)
	{
		std::lock_guard<std::mutex> uniqueLock(m_mutex);
		if (m_container.empty()) {
			return false;
		}
		value = m_container.front();
		m_container.pop();
		if (m_container.size() <= allowedSize) {
			m_cvPush.notify_all();
		}
		return true;
	}

	//@brief inserts element at the end
	void push(const valueType& value)
	{
		std::unique_lock<std::mutex> uniqueLock(m_mutex);
		m_cvPush.wait(uniqueLock, [this]() { return m_container.size() < maxSize; });
		assert(m_container.size() < maxSize);
		m_container.push(value);
	}

	//@brief returns the number of elements 
	size_t size() const
	{
		std::lock_guard<std::mutex> lockGuard(m_mutex);
		return m_container.size();
	}
private:
	mutable std::mutex m_mutex;
	std::condition_variable m_cvPush;
	std::queue<valueType> m_container;
}; // class threadSafeQueue

} // namespace consumerProducer

#endif // THREAD_SAFE_QUEUE_HPP

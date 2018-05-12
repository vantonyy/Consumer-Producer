#ifndef THREAD_SAFE_QUEUE_HPP
#define THREAD_SAFE_QUEUE_HPP

#include <cassert>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

namespace consumerProducer {

//@class threadSafeQueue
template <typename T>
class threadSafeQueue
{
public:
	typedef T valueType;
public:
	threadSafeQueue(size_t mSize, size_t aSize)
		: m_maxSize(mSize)
		, m_allowedSize(aSize)
	{}

	threadSafeQueue(threadSafeQueue&&) = delete;
	threadSafeQueue(const threadSafeQueue&) = delete;
	threadSafeQueue& operator=(threadSafeQueue&&) = delete;
	threadSafeQueue& operator=(const threadSafeQueue&) = delete;
public:
	//@brief removes the first element
	//@returns false if queue is empty, true otherwise
	bool pop(valueType& value, int timeout)
	{
		{
			std::unique_lock<std::mutex> uniqueLock(m_mutex);
			if (!m_cvPop.wait_for(uniqueLock, std::chrono::microseconds(timeout), 
					      [this]() {return !m_container.empty(); })) {
				return false;
			}
			value = m_container.front();
			m_container.pop();
			if (m_container.size() > m_allowedSize) {
				return true;
			}
		}
		m_cvPush.notify_all();
		return true;
	}

	//@brief inserts element at the end
	void push(const valueType& value)
	{
		{
			std::unique_lock<std::mutex> uniqueLock(m_mutex);
			m_cvPush.wait(uniqueLock, [this]() { return m_container.size() < m_maxSize; });
			assert(m_container.size() < m_maxSize);
			m_container.push(value);
		}
		m_cvPop.notify_all();
	}

	//@brief returns the number of elements 
	size_t size() const
	{
		std::lock_guard<std::mutex> lockGuard(m_mutex);
		return m_container.size();
	}
private:
	const size_t m_maxSize;
	const size_t m_allowedSize;
	mutable std::mutex m_mutex;
	std::condition_variable m_cvPush;
	std::condition_variable m_cvPop;
	std::queue<valueType> m_container;
}; // class threadSafeQueue

} // namespace consumerProducer

#endif // THREAD_SAFE_QUEUE_HPP

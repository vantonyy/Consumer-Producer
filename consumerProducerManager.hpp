#ifndef CONSUMER_PRODUCER_MANAGER_HPP
#define CONSUMER_PRODUCER_MANAGER_HPP

#include "threadSafeQueue.hpp"

#include <atomic>
#include <cassert>
#include <csignal>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace consumerProducer {

//@class workerThread
class workerThread
{
public:
	typedef std::shared_ptr<workerThread> ptr;
public:
	//@brief executes the thread.
	void execute()
	{
		m_thread = std::thread(&workerThread::work, this);
	}

	//@brief the function returns when the thread execution has completed
	void join()
	{
		assert(m_thread.joinable());
		m_thread.join();
	}

	//@brief sleeps the thread
	static void sleep(int ms);

	//@brief sets interrupt flag
	static void interrupt(bool);
protected:
	virtual void work() = 0;
protected:
	static std::atomic<bool> s_interrupted;
	static std::atomic<int> s_activeProducersCount;
	static threadSafeQueue s_dataQueue;
private:
	std::thread m_thread;
}; // class workerThread

//@class producer
class producer
	: public workerThread
{
private:
	virtual void work();
}; //class producer

//@class consumer
class consumer
	: public workerThread
{
public:
	explicit consumer(std::ofstream& out)
		: workerThread()
		, m_fileRef(out)
	{
	}
private:
	virtual void work();
private:
	std::ofstream& m_fileRef;
	static std::mutex s_fileMutex;
}; // class consumer

   //@class printer
class printer
	: public workerThread
{
private:
	virtual void work();
private:
	static std::mutex s_coutMutex;
}; // class printer

//@class consumerProducerManager
class consumerProducerManager
{
private:
	consumerProducerManager() = default;
public:
	consumerProducerManager(const consumerProducerManager&) = delete;
	consumerProducerManager& operator=(const consumerProducerManager&) = delete;
public:
	//@brief gets consumerProducerManager instance
	static consumerProducerManager& getInstance();

	//@brief function for handling of POSIX signals
	static void signalHandler(int);

	//@brief creates N thread for producers
	void createProducers(unsigned N);

	//@brief creates N thread for consumers
	void createConsumers(unsigned M, std::ofstream&);

	//@brief creates a thread for printer
	void createPrinter();

	//@brief function returns when the threads execution has completed.
	void joinThreads();
private:
	template<typename WorkerType, typename... Args>
	void createWorkers(unsigned, Args&&...);
private:
	std::vector<workerThread::ptr> m_threads;
}; //class consumerProducerManager

} // namespace consumerProducer

#endif // CONSUMER_PRODUCER_MANAGER_HPP

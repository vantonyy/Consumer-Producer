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
	//@brief default constructor
	workerThread() = default;

	//@brief returns when the thread execution has completed
	virtual ~workerThread();

	//@brief executes the thread.
	void execute();

	//@brief sleeps the thread
	static void sleep(int ms);

	//@brief sets interrupt flag
	static void interrupt(bool);

	//@brief gets error message
	const std::string& getErrorMsg() const;

	//@brief returns true if an error is occurred, false otherwise
	bool hasError() const;
protected:
	virtual void work() = 0;
private:
	void exceptionSafeWork() noexcept;
protected:
	static std::atomic<bool> s_interrupted;
private:
	std::thread m_thread;
	std::string m_errorMsg;
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

	//@brief reports errors which occurred during execution of threads
	void reportErrors();

	//@brief removes consumers, producers and printer threads
	void cleanup();
private:
	template<typename WorkerType, typename... Args>
	void createWorkers(unsigned, Args&&...);
private:
	std::vector<workerThread::ptr> m_threads;
}; //class consumerProducerManager

} // namespace consumerProducer

#endif // CONSUMER_PRODUCER_MANAGER_HPP

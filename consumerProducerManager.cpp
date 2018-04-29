#include "consumerProducerManager.hpp"

#include <algorithm>
#include <csignal>
#include <random>

namespace consumerProducer {

namespace {

//@class sharedData
class sharedData
{
public:
	static std::atomic<int> activeProducersCount;
	static threadSafeQueue<int> dataQueue;
}; // class sharedData

std::atomic<int> sharedData::activeProducersCount(0);
threadSafeQueue<int> sharedData::dataQueue(100, 80);

//@brief generates random number in given range
inline int random(int lb, int ub)
{
	static thread_local std::mt19937 gen;
	std::uniform_int_distribution<int> d(lb, ub);
	return d(gen);
}

} //unnamed namespace

 //Implementation of consumerProducerManager class

consumerProducerManager& consumerProducerManager::getInstance()
{
	static consumerProducerManager m;
	return m;
}

void consumerProducerManager::signalHandler(int sigNum)
{
	switch (sigNum)	{
	case SIGINT:
		workerThread::interrupt(true);
		break;
	default:
		break;
	}
}

void consumerProducerManager::createProducers(unsigned numOfProducers)
{
	m_threads.reserve(m_threads.size() + numOfProducers);
	createWorkers<producer>(numOfProducers);
}

void consumerProducerManager::createConsumers(unsigned numOfConsumers, std::ofstream& file)
{
	m_threads.reserve(m_threads.size() + numOfConsumers);
	createWorkers<consumer>(numOfConsumers, file);
}

void consumerProducerManager::createPrinter()
{
	createWorkers<printer>(1);
}

template <typename WorkerType, typename... Args>
void consumerProducerManager::createWorkers(unsigned n, Args&&... args)
{
	for (unsigned i = 0; i != n; ++i) {
		workerThread::ptr p(new WorkerType(args...));
		m_threads.push_back(p);
		p->execute();
	}
}

void consumerProducerManager::cleanup()
{
	m_threads.clear();
}

//Implementation of workerThread class

std::atomic<bool> workerThread::s_interrupted(false);

workerThread::~workerThread()
{
	if (m_thread.joinable()) {
		m_thread.join();
	}
}

void workerThread::execute()
{
	m_thread = std::thread(&workerThread::work, this);
}

void workerThread::interrupt(bool interrupted)
{
	s_interrupted = interrupted;
}

void workerThread::sleep(int ms)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

//Implementation of producer class
void producer::work()
{
	++sharedData::activeProducersCount;
	while (!s_interrupted) {
		sharedData::dataQueue.push(random(1, 100));
		sleep(random(0, 100));
	}
	--sharedData::activeProducersCount;
}

//Implementation of consumer class
std::mutex consumer::s_fileMutex;

void consumer::work()
{
	while (!s_interrupted || 0 != sharedData::activeProducersCount) {
		int value = 0;
		if (sharedData::dataQueue.pop(value, 100)) {
			std::lock_guard<std::mutex> lockerGuard(s_fileMutex);
			m_fileRef << value << ", ";
		}
		sleep(random(0, 100));
	}
}

//Implementation of printer class
std::mutex printer::s_coutMutex;

void printer::work()
{
	while (!s_interrupted || 0 != sharedData::activeProducersCount) {
		std::lock_guard<std::mutex> locker(s_coutMutex);
		std::cout << "Current num of elements: " << sharedData::dataQueue.size() << std::endl;
		sleep(1000);
	}
}

} // namespace consumerProducer

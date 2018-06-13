#include "application.hpp"
#include "consumerProducerManager.hpp"

#include <thread>
#include <iostream>

namespace consumerProducer {

namespace {

//@class outFileOpener
class outFileOpener
{
public:
	explicit outFileOpener(std::ofstream& file)
		: m_fileRef(file)
	{
	}

	~outFileOpener()
	{
		if (m_fileRef.is_open()) {
			m_fileRef.close();
		}
	}

	//@brief opens empty file by given filename
	std::ofstream& open(const std::string& fileName)
	{
		assert(!fileName.empty());
		if (!m_fileRef.is_open()) {
			m_fileRef.open(fileName, std::ios::trunc);
		}
		return m_fileRef;
	}
private:
	std::ofstream& m_fileRef;
}; // class outFileOpener

} // unnamed namespace

//Implementation of application class

application::application(int argc, char const** argv)
	: m_fileName()
	, m_numOfProducers(0)
	, m_numOfConsumers(0)
	, m_isValid(parseOptions(argc, argv))
{
}

int application::run()
{
	if (!m_isValid) {
		return 1;
	}
	std::ofstream file;
	outFileOpener opener(file);
	signal(SIGINT, &consumerProducerManager::signalHandler);
	consumerProducerManager& m = consumerProducerManager::getInstance();
	m.createProducers(m_numOfProducers);
	m.createPrinter();
	m.createConsumers(m_numOfConsumers, opener.open(m_fileName));
	m.reportErrors();
	m.cleanup();
	return 0;
}

void application::initOptions(options& opts)
{
	opts.addOption(options::def("-outFileName", "data.txt"));
	opts.addOption(options::def("-numOfProducers", "10"));
	opts.addOption(options::def("-numOfConsumers", "10"));
}

const std::string& application::usage()
{
	static const std::string str = "Usage: ./task1 [options] [arguments] ...\n"
					"Options:\n"
					"\t-outFileName\t<string = data.txt>\tOutput file name\n"
					"\t-numOfProducers\t<number = 10>\t\tNumber of producers [1, 10]\n"
					"\t-numOfConsumers\t<number = 10>\t\tNumber of consumers [1, 10]";
	return str;
}

bool application::parseOptions(int argc, char const** argv)
{
	options opts;
	initOptions(opts);
	assert(0 != argv);
	assert(1 <= argc);
	for (int i = 1; i != argc; i += 2) {
		std::string optionName = argv[i];
		if ("-help" == optionName) {
			std::cout << usage();
			return false;
		}
		if (!opts.isValidOption(optionName)) {
			std::cout << "Unknown option: " + std::string(argv[i]);
			return false;
		}
		if (1 + i == argc) {
			std::cout << "Incorect argument for '" + std::string(argv[i]) + "' option";
			return false;
		}
		opts.setValue(optionName, argv[i + 1]);
	}
	if (!opts.getValue("-outFileName", m_fileName) || m_fileName.empty()) {
		std::cout << usage();
		return false;
	}
	auto checkThreadsNum = [](unsigned value) { return 1 <= value && 10 >= value; };
	if (!opts.getValue("-numOfProducers", m_numOfProducers) || !checkThreadsNum(m_numOfProducers)) {
		std::cout << usage();
		return false;
	}
	if (!opts.getValue("-numOfConsumers", m_numOfConsumers) || !checkThreadsNum(m_numOfConsumers)) {
		std::cout << usage();
		return false;
	}
	return true;
}

} // namespace consumerProducer

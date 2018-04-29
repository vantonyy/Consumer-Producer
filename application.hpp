#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "options.hpp"

#include <cassert>
#include <string>
#include <vector>

namespace consumerProducer {

//@class applicatio
class application
{
public:
	application(const application&) = delete;
	application& operator=(const application&) = delete;
public:
	application(int, char const**);
	//@brief runs the application
	int run();
private:
	static void initOptions(options&);
	static const std::string& usage();
	bool parseOptions(int, char const**);
private:
	std::string m_fileName;
	unsigned m_numOfProducers;
	unsigned m_numOfConsumers;
	bool m_isValid;
};

} // namespace consumerProducer

#endif // APPLICATION_HPP

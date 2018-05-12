#include "application.hpp"

#include <iostream>
#include <exception>

int main(int argc, char const **argv)
{
	consumerProducer::application app(argc, argv);
        try {
                return app.run();
        } catch(const std::exception& e) {
                std::cerr << "Exception: " << e.what() << std::endl;
        } catch(...) {
		std::cerr << "Unhandled exception" << std::endl;
        }
        return 1;
}

#ifndef MIRROR_LOGGING_LIBRARY_H
#define MIRROR_LOGGING_LIBRARY_H

#include <cstdint>
#include <string>
#include 

namespace {
    class Logger {
    public:
        // Constructors
        Logger() = delete;
        Logger(uint16_t port, std::string componentName);

        void info();
        void warn();
        void error();
        void fatal();

    private:
        std::string m_componentName;
    };
}

#endif //MIRROR_LOGGING_LIBRARY_H

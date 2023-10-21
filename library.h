#pragma once

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

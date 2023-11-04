#pragma once

// std Includes
#include <mutex>
#include <string>

// Library Includes
#include <zmq.hpp>

namespace mirror {
    /**
     * Singleton class to be used when logging in projects associated with the Clarkson Open Source Institute's Mirror
     */
    class Logger {
    public: // Functions
        // Deleted
        Logger(Logger &) = delete;

        Logger(Logger &&) = delete;

        bool operator=(const Logger &) = delete;

        bool operator=(const Logger &&) = delete;

        // Instance Fetch Method
        static Logger *getInstance();

        // Logging Methods
        [[maybe_unused]] void info(const std::string &logMessage);

        [[maybe_unused]] void warn(const std::string &logMessage);

        [[maybe_unused]] void error(const std::string &logMessage);

        [[maybe_unused]] void fatal(const std::string &logMessage);

        // Configuration
        void configure(uint16_t port, const std::string &componentName, const std::string &address = "localhost");

    protected: // Functions
        Logger() : m_Configured(false) {}

        ~Logger() { m_LogServerSocket.close(); }

    private: // Functions
        void f_SendLine(const std::string &lineToSend);
        static void f_initializeKeepAliveThread();

    private: // Data
        static Logger *s_Instance;

        zmq::socket_t m_LogServerSocket{s_SocketContext, zmq::socket_type::stream};
        static zmq::context_t s_SocketContext;
        bool m_Configured;

        std::string m_ComponentName;

        static std::mutex s_AccessMutex;

        enum class LogLevels : int {
            Info, Warn, Error, Fatal
        };

    }; // class Logger

} // namespace mirror

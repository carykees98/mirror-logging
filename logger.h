#pragma once

#include <cstdint>
#include <string>
#include <memory>
#include <zmq.hpp>

namespace mirror {
    /**
     * Singleton class to be used when logging in projects associated with the Clarkson Open Source Institute's Mirror
     */
    class Logger {
    public:
        // Deleted Constructors
        Logger(Logger &&) = delete;

        Logger(Logger &) = delete;

        /**
         * Allows for access to the singleton instance of the Logger class
         * @return instance of Logger class
         */
        static std::shared_ptr<Logger> getInstance();

        /**
         * Sends message to log server so long as configure() has been called previously
         * @throws std::logic_error thrown if configure() hasn't been called previously
         * @param logMessage message to send to log server prefixed by logging level
         */
        void sendLogMessage(const std::string &logMessage);

        /**
         * Configures Logger class, must be executed else program will shutdown
         * @param port Port that the log server is running on
         * @param componentName Name to be displayed along side log messages for identification
         */
        void configure(uint16_t port, const std::string &componentName);

        /**
         * Sends a message to the log server with severity [info]
         * @param logMessage message to send to log server
         */
        [[maybe_unused]] void info(const std::string &logMessage);

        /**
         * Sends a message to the log server with severity [warn]
         * @param logMessage message to send to log server
         */
        [[maybe_unused]] void warn(const std::string &logMessage);

        /**
        * Sends a message to the log server with severity [error]
        * @param logMessage message to send to log server
        */
        [[maybe_unused]] void error(const std::string &logMessage);

        /**
         * Sends a message to the log server with severity [fatal]
         * @param logMessage message to send to log server
         */
        [[maybe_unused]] void fatal(const std::string &logMessage);


    private:
        bool m_Configured;
        zmq::socket_t m_LogServerSocket;
        zmq::context_t m_SocketContext {1};

        /**
         * Available Log Levels
         */
        enum class LogLevels {
            Info,
            Warn,
            Error,
            Fatal
        };

    private:
        /**
         * Default constructor. Kept private so that the class may be used as a singleton.
         */
        Logger() : m_Configured(false) {}

    };
}

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

        /**
         * Sets this Logger's component name  
         * @param componentName Name to be displayed along side log messages for identification
         */
        void setComponentName(const std::string &componentName);


    private:
        /**
         * True if configure() has been run, false otherwise
         */
        bool m_Configured;

        /**
         * socket used to communicate with the log server
         */
        zmq::socket_t m_LogServerSocket;

        /**
         * context for m_LogServerSocket
         */
        zmq::context_t m_SocketContext{1, 1};

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

        /**
         * Sends line to log server so long as configure() has been called previously
         * @throws std::logic_error thrown if configure() hasn't been called previously
         * @param line Line to send to log server
         */
        void sendLine(const std::string &line);

        /**
         * Responsible for keeping the connection between the component and the log server alive during periods of reduced log traffic
         */
        [[noreturn]] static void initializeKeepAliveThread();
    };
}

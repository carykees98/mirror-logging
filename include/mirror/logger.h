#pragma once

// std Includes
#include <mutex>
#include <string>
#include <thread>

// Library Includes
#include <zmq.hpp>

namespace mirror {

    /**
     * Context for the connection to the log server. Defined in `logger.cpp`
     */
    extern zmq::context_t socketContext;

    /**
     * Singleton class to be used when logging in projects associated with the Clarkson Open Source Institute's Mirror
     */
    class Logger {
    public: // Functions
        // Deleted
        /**
         * Copy Constructor - Deleted
         */
        Logger(Logger &) = delete;

        /**
         * Move Constructor - Deleted
         */
        Logger(Logger &&) = delete;

        /**
         * Copy Assignment Operator - Deleted
         *
         * @return Nothing
         */
        Logger &operator=(const Logger &) = delete;

        /**
         * Move Assignment Operator - Deleted
         * @return Nothing
         */
        Logger &operator=(const Logger &&) = delete;

        // Instance Fetch Method
        /**
         * Method used to fetch a pointer to the static instance of the Logger class
         *
         * @return Pointer to the Loggers static instance
         */
        static Logger *getInstance();

        // Logging Methods
        /**
         * Sends information to be logged to the log server
         *
         * @param logMessage Information to be logged
         */
        [[maybe_unused]] void info(const std::string &logMessage);

        /**
         * Sends a warning message to the log server
         *
         * @param logMessage Information to be logged
         */
        [[maybe_unused]]void warn(const std::string &logMessage);

        /**
         * Sends an error message to the log server
         *
         * @param logMessage Information to be logged
         */
        [[maybe_unused]] void error(const std::string &logMessage);

        /**
         * Sends a fatal message to the log server
         *
         * @param logMessage Information to be logged
         */
        [[maybe_unused]] void fatal(const std::string &logMessage);

        // Configuration
        /**
         * Configures the connection to the log server
         *
         * **Must be called before any log messages can be sent**
         *
         * @param port Port that the log server is running on
         * @param componentName Name of the component that the logger is being used in
         * @param address IP address of the machine that the log server is running on
         */
        void configure(uint16_t port, const std::string &componentName, const std::string &address = "localhost");

    protected: // Functions
        /**
         * Default constructor. Registers a function to be called upon program exit that calls the Logger destructor
         */
        inline Logger() : m_Configured(false) { std::atexit([]() { Logger::getInstance()->~Logger(); }); }

        // Destructor
        /**
         * Destructor for the Logger class. Disconnects from the log server upon being called
         */
        inline ~Logger() { m_LogServerSocket.disconnect(m_URL); }

    private: // Functions
        /**
         * Sends a line of text to the log server
         *
         * @param lineToSend Message to send to the log server
         */
        void f_SendLine(const std::string &lineToSend);

    private: // Data
        /**
         * Pointer to the instance of the Logger class
         */
        static Logger *s_Instance;

        /**
         * Mutex to prevent issues related to access of the Logger instance in a multi-threaded environment
         */
        static std::mutex s_AccessMutex;

        /**
         * Socket used to communicate with the log server
         */
        zmq::socket_t m_LogServerSocket{socketContext, zmq::socket_type::stream};

        /**
         * URL of the log server
         */
        std::string m_URL;

        /**
         * False until `configure()` is run.
         *
         * If false, will cause program shutdown if an attempt is made to send a line to the log server
         */
        bool m_Configured;

        /**
         * Name of the component this class is instantiated in. Sent to the log server with each log message
         */
        std::string m_ComponentName;


        /**
         * Available log levels, used to increase readability in the log methods
         */
        enum class LogLevels : int {
            /**
             * LogLevels::Info
             */
            Info,
            /**
             * LogLevels::Warn
             */
            Warn,
            /**
             * LogLevels::Error
             */
            Error,
            /**
             * LogLevels::Fatal
             */
            Fatal
        };

    }; // class Logger

} // namespace mirror

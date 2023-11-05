// Header Being Defined
#include <mirror/logger.h>

// std includes
#include <mutex>
#include <string>
#include <thread>

// Library includes
#include <zmq.hpp>

namespace mirror {

    zmq::context_t socketContext(1, 1);

    // Static Member Initializations
    Logger *Logger::s_Instance = nullptr;
    std::mutex Logger::s_AccessMutex;

    /*
     * Start Of Public Functions
     */

    Logger *Logger::getInstance() {
        std::lock_guard<std::mutex> instanceLock(s_AccessMutex);

        // If no instance exists, one is created
        if (s_Instance == nullptr) {
            s_Instance = new Logger();
        }

        return s_Instance;
    }

    [[maybe_unused]] void Logger::info(const std::string &logMessage) {
        std::string lineToSend = "@" + std::to_string((int) LogLevels::Info) + logMessage;
        f_SendLine(lineToSend);
    }

    [[maybe_unused]] void Logger::warn(const std::string &logMessage) {
        std::string lineToSend = "@" + std::to_string((int) LogLevels::Warn) + logMessage;
        f_SendLine(lineToSend);
    }

    [[maybe_unused]] void Logger::error(const std::string &logMessage) {
        std::string lineToSend = "@" + std::to_string((int) LogLevels::Error) + logMessage;
        f_SendLine(lineToSend);
    }

    [[maybe_unused]] void Logger::fatal(const std::string &logMessage) {
        std::string lineToSend = "@" + std::to_string((int) LogLevels::Fatal) + logMessage;
        f_SendLine(lineToSend);
    }

    void Logger::configure(uint16_t port, const std::string &componentName, const std::string &address) {
        std::lock_guard<std::mutex> instanceLock(s_AccessMutex);

        m_URL = "tcp://" + address + ":" + std::to_string(port);
        m_LogServerSocket.connect(m_URL);

        m_ComponentName = componentName;
        m_Configured = true;
    }

    /*
     * Start Of Private Functions
     */

    void Logger::f_SendLine(const std::string &lineToSend) {
        std::lock_guard<std::mutex> instanceGuard(s_AccessMutex);

        // Throws exception if logger is not configured. Should cause the program to end
        if (!m_Configured)
            throw std::logic_error("Logger not configured");

        // Routing ID required for sending TCP messages, not part of the packet by default
        std::string routingID = m_LogServerSocket.get(zmq::sockopt::routing_id);
        m_LogServerSocket.send(zmq::message_t(routingID), zmq::send_flags::sndmore);

        zmq::message_t message{"@" + m_ComponentName + lineToSend + "\r\n"};
        m_LogServerSocket.send(message, zmq::send_flags::none);
    }

} // namespace mirror
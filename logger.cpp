#include "logger.h"

#include <iostream>
#include <exception>
#include <string>
#include <zmq.hpp>

namespace mirror {
    std::shared_ptr<Logger> Logger::getInstance() {
        static std::shared_ptr<Logger> logger(new Logger());
        return logger;
    }

    void Logger::configure(uint16_t port, const std::string &componentName) {
        // Initialize connection to log server
        m_LogServerSocket = zmq::socket_t(m_SocketContext, zmq::socket_type::stream);
        m_LogServerSocket.connect("tcp://localhost:" + std::to_string(port));

        // Send log server the component name
        std::string componentNameMessage = "@ComponentName " + componentName + "\r\n";
        m_LogServerSocket.send(zmq::message_t(componentNameMessage), zmq::send_flags::none);

        m_Configured = true;
    }

    void Logger::sendLogMessage(const std::string &logMessage) {
        // if not configured throw exception, will likely end program unless wrapped in try catch
        if (!m_Configured) { throw std::logic_error("Error: Logger Not Configured"); }

        // send message
        m_LogServerSocket.send(zmq::message_t(logMessage + "\r\n"), zmq::send_flags::none);
    }

    [[maybe_unused]] void Logger::info(const std::string &logMessage) {
        sendLogMessage(std::to_string((int) LogLevels::Info) + logMessage);
    }

    [[maybe_unused]] void Logger::warn(const std::string &logMessage) {
        sendLogMessage(std::to_string((int) LogLevels::Warn) + logMessage);
    }

    [[maybe_unused]] void Logger::error(const std::string &logMessage) {
        sendLogMessage(std::to_string((int)LogLevels::Error) + logMessage);
    }

    [[maybe_unused]] void Logger::fatal(const std::string &logMessage) {
        sendLogMessage(std::to_string((int) LogLevels::Fatal) + logMessage);
    }
}


#include <mirror/logger.h>

#include <exception>
#include <string>
#include <zmq.hpp>
#include <thread>
#include <chrono>
#include <mutex>

namespace mirror {
    //mutex used to prevent race conditions when sending messages to the log server
    std::mutex socketMutex;

    std::shared_ptr<Logger> Logger::getInstance() {
        static std::shared_ptr<Logger> logger(new Logger());
        return logger;
    }

    void Logger::configure(uint16_t port, const std::string &componentName) {
        // Initialize connection to log server
        m_LogServerSocket = zmq::socket_t(m_SocketContext, zmq::socket_type::stream);
        m_LogServerSocket.connect("tcp://localhost:" + std::to_string(port));
        m_Configured = true;

        // Send log server the component name
        setComponentName(componentName);

        initializeKeepAliveThread();
    }

    void Logger::sendLine(const std::string &line) {
        // if not configured throw exception, will likely end program unless wrapped in try catch
        if (!m_Configured) { throw std::logic_error("Error: Logger Not Configured"); }

        {
            // lock mutex so that messages aren't sent simultaneously
            std::lock_guard<std::mutex> lock(socketMutex);

            // send message
            std::string routingID = m_LogServerSocket.get(zmq::sockopt::routing_id);
            m_LogServerSocket.send(zmq::message_t(routingID), zmq::send_flags::sndmore);
            m_LogServerSocket.send(zmq::message_t(line + "\r\n"), zmq::send_flags::none);
        }
    }

    [[maybe_unused]] void Logger::info(const std::string &logMessage) {
        sendLine(std::to_string((int) LogLevels::Info) + logMessage);
    }

    [[maybe_unused]] void Logger::warn(const std::string &logMessage) {
        sendLine(std::to_string((int) LogLevels::Warn) + logMessage);
    }

    [[maybe_unused]] void Logger::error(const std::string &logMessage) {
        sendLine(std::to_string((int) LogLevels::Error) + logMessage);
    }

    [[maybe_unused]] void Logger::fatal(const std::string &logMessage) {
        sendLine(std::to_string((int) LogLevels::Fatal) + logMessage);
    }

    void Logger::setComponentName(const std::string &componentName) {
        sendLine("@ComponentName " + componentName);
    }

    [[noreturn]] void Logger::initializeKeepAliveThread() {
        using namespace std::chrono_literals;

        std::thread(
                []() {
                    while (true) {
                        mirror::Logger::getInstance()->sendLine("@KeepAlive");
                        std::this_thread::sleep_for(15min);
                    }
                }).detach();
    }
}

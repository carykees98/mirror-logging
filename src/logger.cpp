// Header Being Defined
#include <mirror/logger.h>

// std includes
#include <mutex>
#include <string>
#include <thread>

// Library includes
#include <zmq.hpp>

namespace mirror {

    zmq::context_t socketContext(1, 1)

    // Static Member Initializations
    Logger *Logger::s_Instance = nullptr;
    std::mutex Logger::s_AccessMutex;

    /*
     * Start Of Public Functions
     */

    Logger *Logger::getInstance() {
        std::lock_guard<std::mutex> instanceLock(s_AccessMutex);

        if (s_Instance == nullptr) {
            s_Instance = new Logger();
        }

        return s_Instance;
    }

    [[maybe_unused]] void Logger::info(const std::string &logMessage) {
        f_SendLine(std::to_string((int) LogLevels::Info) + logMessage);
    }

    [[maybe_unused]] void Logger::warn(const std::string &logMessage) {
        f_SendLine(std::to_string((int) LogLevels::Warn) + logMessage);
    }

    [[maybe_unused]] void Logger::error(const std::string &logMessage) {
        f_SendLine(std::to_string((int) LogLevels::Error) + logMessage);
    }

    [[maybe_unused]] void Logger::fatal(const std::string &logMessage) {
        f_SendLine(std::to_string((int) LogLevels::Fatal) + logMessage);
    }

    void Logger::configure(uint16_t port, const std::string &componentName, const std::string &address) {
        std::lock_guard<std::mutex> instanceLock(s_AccessMutex);

        std::string url = "tcp://" + address + ":" + std::to_string(port);
        m_LogServerSocket.connect(url);

        m_Configured = true;
        m_ComponentName = componentName;

        f_initializeKeepAliveThread();
    }

    /*
     * Start Of Private Functions
     */

    void Logger::f_SendLine(const std::string &lineToSend) {
        std::lock_guard<std::mutex> instanceGuard(s_AccessMutex);

        if (!m_Configured) throw std::logic_error("Logger not configured");

        std::string routingID = m_LogServerSocket.get(zmq::sockopt::routing_id);
        m_LogServerSocket.send(zmq::message_t(routingID), zmq::send_flags::sndmore);
        m_LogServerSocket.send(zmq::message_t(lineToSend + "\r\n"), zmq::send_flags::none);
    }


    void Logger::f_initializeKeepAliveThread() {
        using namespace std::chrono_literals;

        std::thread(
                []() {
                    while (true) {
                        Logger::getInstance()->f_SendLine("@KeepAlive");
                        std::this_thread::sleep_for(15min);
                    }
                }
        ).detach();
    }

} // namespace mirror
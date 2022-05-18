#pragma once

#include <string>
#include <thread>             // std::thread
#include <mutex>              // std::mutex, std::unique_lock
#include <condition_variable> // std::condition_variable
#include "utils/IReciever.hpp"
#include "utils/TelloSocket.hpp"
#include "flight_data/FlightData.hpp"
#include "log_data/LogData.hpp"
#include "asio.hpp"
#include "TelloCommander.hpp"
#include "utils/telemetry_data/TelemetryData.hpp"
#include "utils/tello_observer/ISubject.hpp"
#include "utils/tello_observer/IObserver.hpp"
#define DISCONNECT_TIMEOUT_MS 1000
using namespace std::chrono_literals;

// static bool deleteAll(IObserver *theElement)
// {
//     delete theElement;
//     return true;
// };

namespace tello_protocol
{
    class TelloTelemetry : public ISubject
    {
    public:
        void SetSocket(std::shared_ptr<IReciever>);

        TelloTelemetry(std::shared_ptr<spdlog::logger>, spdlog::level::level_enum lvl = spdlog::level::info);
        ~TelloTelemetry();

        /**
         * @brief  Stop the listening thread
         * 
         * Set m_keep_receiving to <code>false<code>, And join on the listener thread.
         */
        void StopListening();

        /**
         * @brief Start listening thread.
         * 
         */
        void StartListening();

        /**
         * @brief Listener thread
         * 
         * Receive new raw bytes from TelloSocket.
         * Maintain a *HeartBeat* monitor, If no bytes received more than DISCONNECT_TIMEOUT_MS, then assuming connection is lost.
         * Notify all observers using the IObserver interface.
         * 
         */
        void Listener();

        void Attach(IObserver *observer) override;
        void Detach(IObserver *observer) override;
        void Notify() override;

        void HowManyObserver();

    private:
        std::list<IObserver *> list_observer_;
        void reset_bytes_received();

        std::shared_ptr<spdlog::logger> m_logger;

        bool process_data(const std::vector<unsigned char> &);
        std::shared_ptr<IReciever> m_socket;

        bool m_IsConnectedToDrone = false;
        bool m_keep_receiving = true;
        bool m_anyDataReceived = false;
        int m_BytesReceived;
        std::thread m_Listener;
        std::vector<unsigned char> m_buffer, m_received_data;
        std::chrono::milliseconds m_time_of_last_packet_received;
    };

} // namespace tello_protocol

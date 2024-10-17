#include "TelloTelemetry.hpp"

namespace tello_protocol
{
    /**
   * The subscription management methods.
   */
    void TelloTelemetry::Attach(IObserver *observer)
    {
        list_observer_.push_back(observer);
    }
    void TelloTelemetry::Detach(IObserver *observer)
    {
        list_observer_.remove(observer);
    }

    void TelloTelemetry::Notify()
    {
        std::list<IObserver *>::iterator iterator = list_observer_.begin();
        HowManyObserver();
        while (iterator != list_observer_.end())
        {
            (*iterator)->Update(m_received_data);
            ++iterator;
        }
    }

    void TelloTelemetry::HowManyObserver()
    {
        m_logger->debug("There are " + std::to_string(list_observer_.size()) + " observers in the list.\n");
    }

    bool TelloTelemetry::legacy_process_data(const std::vector<unsigned char> &data)
    {
        auto received = tello_protocol::Packet(data);

        // if (received.GetBuffer().substr(0, 8) == "conn_ack")
        // {

        // }

        unsigned short cmd;
        std::memcpy(&cmd, &data[5], sizeof(unsigned short));
        if (cmd == tello_protocol::LOG_HEADER_MSG)
        {
            // m_logger->debug("LOG_HEADER_MSG received");
            // LOG HEADER MSG is the biggest log message.
            // Drone will continue to send log_header connection requests with incremental ID.
            // Until sending back to drone 'ack_conn'.
            // m_LogHeaderReceivedId = uint16(data[9], data[10]);

            // SetBuildDate(received.GetBuffer().substr(28, 26));
            // DJI LOG VERSION something like this: DJI_LOG_V3I��Rc
            // auto f = received.GetBuffer().find("DJI");
            // SetDJILogVersion(received.GetBuffer().substr(f, 15));
            // SetLogHeaderReceived();
        }
        else if (cmd == tello_protocol::LOG_DATA_MSG)
        {
            // m_logger->debug("LOG_DATA_MSG received");
            // /*
            // Is cmd  is LOG_DATA_MSG.
            // It means that a LOG_DATA connections has been already assured.
            // */
            // try
            // {
            //     std::vector<unsigned char> trimmed(data.begin() + 10, data.end());
            //     // m_LogData->Update(trimmed);
            // }
            // catch (const std::out_of_range &e)
            // {
            // m_logger->error("LOG_DATA_MSG");
            // }

            // // It is possible that a connection was open from a previous session.
            // SetLogHeaderReceived();
        }
        else if (cmd == tello_protocol::WIFI_MSG)
        {
            // m_logger->info("WIFI_MSG received: {}", int(received.GetData()[0]));
            // m_FlightData->SetWifiStrength(received.GetData()[0]);
        }
        else if (cmd == tello_protocol::ALT_LIMIT_MSG)
        {
            // m_logger->info("ALT_LIMIT_MSG received: {}", short(received.GetData()[1]));
            // m_FlightData->SetAltLimit(received.GetData());
        }
        else if (cmd == tello_protocol::ATT_LIMIT_MSG)
        {
            // m_logger->info("ATT_LIMIT_MSG received: {}", float(received.GetData()[1]));
            // m_FlightData->SetAttLimit(received.GetData());
        }
        else if (cmd == tello_protocol::LOW_BAT_THRESHOLD_MSG)
        {
            // m_logger->info("recv: low battery threshold: {}", received.GetData());
        }

        else if (cmd == tello_protocol::FLIGHT_MSG)
        {
            // m_logger->info("recv: FLIGHT_MSG: {}", spdlog::to_hex(data));
        }
        else if (cmd == tello_protocol::TIME_CMD)
        {
            // m_logger->info("recv: time data: {}", spdlog::to_hex(data));
            // int sec, min, hour;

            // std::memcpy(&hour, &data[8], sizeof(char));

            // std::time_t result;// = std::time(&data[7]);
            // std::memcpy(&result, &data[7], sizeof(result));
            // std::cout << std::asctime(std::localtime(&result))
            //           << result << " seconds since the Epoch\n";

            // m_logger->info(sec);
        }
        else if (cmd == 0x0035)
        {
            /**
             * @brief This is undocumented, need help to understand what this message is!
             * @todo Infer this message and implement a observer to it.
             * 
             * I think this message  relate to on-air/on-ground modes and timings
             * 
             */
            // 0000: cc 60 00 27 88 35 00 23 00 00 81 21
            // m_logger->error("recv undocumented: some ack: {}", spdlog::to_hex(data));
            // m_logger->error("cmd: {}", std::to_string(cmd));
            // short undoc;
            // std::memcpy(&undoc, &data[7], sizeof(short));
            // m_logger->error("data: {}", spdlog::to_hex(data));
            // m_logger->error("undoc: {}", undoc);

            /* code */
        }
        else if (cmd == tello_protocol::LIGHT_MSG)
        {
            m_logger->error("recv: light: {}", spdlog::to_hex(data));
            // log.debug("recv: light: %s" % byte_to_hexstring(data[9:-2]))
        }

        else
        {
            m_logger->error("recv: some ack: {}", spdlog::to_hex(data));
        }
        std::fill(m_buffer.begin(), m_buffer.end(), 0);
        return true;
    }

    void TelloTelemetry::Listener()
    {
        std::chrono::_V2::system_clock::time_point p1;
        std::chrono::milliseconds time_now, duration;

        while (m_keep_receiving)
        {
            if (m_socket != nullptr)
            {
                m_BytesReceived = m_socket->Receive(m_buffer);
            }

            m_logger->debug("Bytes received: {}", m_BytesReceived);

            p1 = std::chrono::system_clock::now();
            time_now = std::chrono::duration_cast<std::chrono::milliseconds>(p1.time_since_epoch());

            if (m_BytesReceived > 0)
            {
                m_IsConnectedToDrone = true;
                m_time_of_last_packet_received = time_now;
                m_anyDataReceived = true;

                /**
                 * @brief Pack data from sender, And notify all observers.
                 * 
                 */
                m_received_data = std::vector<unsigned char>(m_buffer.begin(), m_buffer.begin() + m_BytesReceived);
                Notify();

                /**
                 * @todo Move all processing into attached observers.
                 * 
                 */
                if (!legacy_process_data(m_received_data))
                {
                    m_logger->error("Could not process data! Dumping:\n {DATA SUPPOSE TO BE HERE}");
                }
            }

            duration = time_now - m_time_of_last_packet_received;
            m_logger->debug("Millisec since last received packet [ms]: " + std::to_string(duration.count()));

            if (m_anyDataReceived && duration.count() > DISCONNECT_TIMEOUT_MS)
            {
                m_IsConnectedToDrone = false;
                m_logger->warn("Lost connection with drone more than: " + std::to_string(DISCONNECT_TIMEOUT_MS) + "[ms]!");
                std::this_thread::sleep_for(1s);
            }

            reset_bytes_received();
            std::this_thread::sleep_for(5ms);
        }
    }

    void TelloTelemetry::reset_bytes_received()
    {
        m_BytesReceived = 0;
    }

    // const int TelloTelemetry::AmountOfBytesReceived() const
    // {
    //     return m_BytesReceived;
    // }
    // bool TelloTelemetry::IsDroneConnected() const
    // {
    //     return m_IsConnectedToDrone;
    // }
    // bool TelloTelemetry::IsAnyDataReceived() const
    // {
    //     return m_anyDataReceived;
    // }

    // This capability is aborted duo to SOLID principles.
    // void TelloTelemetry::SetTelloCommander(std::shared_ptr<tello_protocol::TelloCommander> telloCommander)
    // {
    //     m_TelloCommander = telloCommander;
    // };

    // const int TelloTelemetry::GetLogHeaderId() const
    // {
    //     return m_LogHeaderReceivedId;
    // };
    // void TelloTelemetry::SetLogHeaderReceived()
    // {
    //     m_IsLogHeaderReceived = true;
    // };

    // bool TelloTelemetry::IsConnReqAckReceived() const
    // {

    // }
    // bool TelloTelemetry::IsLogHeaderReceived() const
    // {
    //     return m_IsLogHeaderReceived;
    // }

    // void TelloTelemetry::SetLogData(std::shared_ptr<LogData> log_data)
    // {
    //     m_LogData = log_data;
    // }
    // std::shared_ptr<LogData> TelloTelemetry::GetLogData() const
    // {
    //     return m_LogData;
    // }

    // void TelloTelemetry::SetFlightData(std::shared_ptr<FlightData> flight_data)
    // {
    //     m_FlightData = flight_data;
    // }
    // std::shared_ptr<FlightData> TelloTelemetry::GetFlightData() const
    // {
    //     return m_FlightData;
    // }

    // void TelloTelemetry::SetDJILogVersion(const std::string &log_version)
    // {
    //     m_DJI_LOG_VERSION = log_version;
    //     m_logger->info("DJI LOG VERSION: {};", m_DJI_LOG_VERSION);
    // }
    // const std::string &TelloTelemetry::GetDJILogVersion() const
    // {
    //     return m_DJI_LOG_VERSION;
    // }

    // void TelloTelemetry::SetBuildDate(const std::string &build_date)
    // {
    //     m_BuildDate = build_date;
    //     m_logger->info("BUILD date: {};", m_BuildDate);
    // }
    // const std::string &TelloTelemetry::GetBuildDate() const { return m_BuildDate; }

    void TelloTelemetry::StopListening()
    {
        m_keep_receiving = true;
        if (m_Listener.joinable())
        {
            m_Listener.join();
        }
        m_logger->debug(m_logger->name() + " Listener thread stopped");
    }

    void TelloTelemetry::StartListening()
    {
        m_keep_receiving = true;
        m_Listener = std::thread(&TelloTelemetry::Listener, this);
        m_logger->debug(m_logger->name() + " Listener thread started");
    }

    void TelloTelemetry::SetSocket(std::shared_ptr<IReceiver> socket)
    {
        m_socket = socket;
    }

    TelloTelemetry::TelloTelemetry(std::shared_ptr<spdlog::logger> logger, spdlog::level::level_enum lvl)
        : m_logger(logger)
    {
        m_logger->set_level(lvl);
        m_logger->info(m_logger->name() + " Initiated.");

        const auto p1 = std::chrono::system_clock::now();
        m_time_of_last_packet_received = std::chrono::duration_cast<std::chrono::milliseconds>(p1.time_since_epoch());
        m_buffer = std::vector<unsigned char>(1024);
    }

    TelloTelemetry::~TelloTelemetry()
    {
        m_logger->info(m_logger->name() + " Destructing.");
        m_keep_receiving = false;
        if (m_Listener.joinable())
            m_Listener.join();
        m_logger->info(m_logger->name() + " thread joined");
        m_logger->info(m_logger->name() + " Emptying observers list");
        while (!list_observer_.empty())
        {
            list_observer_.pop_back();
        }
        m_logger->info(m_logger->name() + " Done emptying observers list");
        m_logger->info(m_logger->name() + " Destructed.");
    }

} // namespace tello_protocol

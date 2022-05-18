#include "TelloFlightDataMsgObserver.hpp"

namespace tello_protocol
{
    void TelloFlightDataMsgObserver::Update(const std::vector<unsigned char> &message_from_subject)
    {

        unsigned short cmd;
        std::memcpy(&cmd, &message_from_subject[5], sizeof(unsigned short));
        if (cmd == tello_protocol::FLIGHT_MSG)
        {
            try
            {
                std::vector<unsigned char> data(message_from_subject.begin() + 9, message_from_subject.end());
                if (!m_flight_data_processor->Update(data))
                {
                    std::string wrn(__FUNCTION__);
                    m_logger->warn(wrn + "Data packet didn't math its expected length. dropping");
                    return;
                }
                m_flight_data_msg_data_mgr.SetFlightData(m_flight_data_processor);
            }
            catch (const std::out_of_range &e)
            {
                std::string wrn(__FUNCTION__);
                m_logger->error(wrn + e.what());
                return;
            }
        }
    }

    TelloFlightDataMsgObserver::TelloFlightDataMsgObserver(ISubject &telemetry, IFlightDataMsgDataManager &flight_data_msg_data_mgr, std::shared_ptr<spdlog::logger> logger, spdlog::level::level_enum lvl)
        : m_telemetry(telemetry),
          m_flight_data_msg_data_mgr(flight_data_msg_data_mgr),
          m_logger(logger)
    {
        m_logger->set_level(lvl);
        m_logger->info(m_logger->name() + " Initiated.");

        m_flight_data_processor = std::make_shared<tello_protocol::FlightData>(spdlog::stdout_color_mt("FlightData"));

        this->m_telemetry.Attach(this);
    }

    TelloFlightDataMsgObserver::~TelloFlightDataMsgObserver()
    {
        m_logger->info(m_logger->name() + " Destructed.");
    }
} // namespace tello_protocol

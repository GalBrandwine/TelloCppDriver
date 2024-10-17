#include "TelloWifiMsgObserver.hpp"

namespace tello_protocol
{
    void TelloWifiMsgObserver::Update(const std::vector<unsigned char> &message_from_subject)
    {

        unsigned short cmd;
        std::memcpy(&cmd, &message_from_subject[5], sizeof(unsigned short));

        if (cmd == tello_protocol::WIFI_MSG)
        {
            unsigned char wifi_strength;
            std::memcpy(&wifi_strength, &message_from_subject[9], sizeof(unsigned char));
            m_logger->debug("WIFI_MSG received: {}", wifi_strength);
            m_wifi_msg_data_mgr.SetWifiMsg(wifi_strength);
        }
    }

    TelloWifiMsgObserver::TelloWifiMsgObserver(ISubject &telemetry, IWifiMsgDataManager &wifi_msg_data_mgr, std::shared_ptr<spdlog::logger> logger, spdlog::level::level_enum lvl)
        : m_wifi_msg_data_mgr(wifi_msg_data_mgr),
          m_logger(logger)
    {
        m_logger->set_level(lvl);
        m_logger->info(m_logger->name() + " Initiated.");

        telemetry.Attach(this);
    }

    TelloWifiMsgObserver::~TelloWifiMsgObserver()
    {
        m_logger->info(m_logger->name() + " Destructed.");
    }
} // namespace tello_protocol

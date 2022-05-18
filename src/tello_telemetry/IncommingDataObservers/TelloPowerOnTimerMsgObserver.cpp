#include "TelloPowerOnTimerMsgObserver.hpp"

namespace tello_protocol
{
    void TelloPowerOnTimerMsgObserver::Update(const std::vector<unsigned char> &message_from_subject)
    {

        unsigned short cmd;
        std::memcpy(&cmd, &message_from_subject[5], sizeof(unsigned short));

        if (cmd == tello_protocol::POWER_ON_TIMER_MSG)
        {
            /**
             * @brief PowerOnTimer message.
             * * Data[7:8]:short   - Seems to be POWER_ON timer.
             * * Data[9]:bool      - Unclear:
             * * * It becomes true upon first takeoff since POWER_ON, remains TRUE while ON_AIR for first flight since POWER_ON. \n
             * 
             */
            PowerOnTimerInfo poti;

            std::memcpy(&poti.power_on_timer, &message_from_subject[7], sizeof(short));
            m_logger->debug("Power ON timer: {}", poti.power_on_timer);

            std::memcpy(&poti.undocumented_bool, &message_from_subject[9], sizeof(bool));
            m_logger->debug("First takeoff since power on: {}", poti.undocumented_bool);
            m_power_on_timer_msg_data_mgr.SetPowerOnTimer(poti);
        }
    }

    TelloPowerOnTimerMsgObserver::TelloPowerOnTimerMsgObserver(ISubject &telemetry, IPowerOnTimerMsgDataManager &power_on_timer_msg_data_mgr, std::shared_ptr<spdlog::logger> logger, spdlog::level::level_enum lvl)
        : m_telemetry(telemetry),
          m_power_on_timer_msg_data_mgr(power_on_timer_msg_data_mgr),
          m_logger(logger)
    {
        m_logger->set_level(lvl);
        m_logger->info(m_logger->name() + " Initiated.");

        this->m_telemetry.Attach(this);
    }

    TelloPowerOnTimerMsgObserver::~TelloPowerOnTimerMsgObserver()
    {
        m_logger->info(m_logger->name() + " Destructed.");
    }
} // namespace tello_protocol

#include "TelloStickCommandsObserver.hpp"

namespace tello_protocol
{
    void TelloStickCommandsObserver::Update(const std::vector<unsigned char> &message_from_subject)
    {
        if (m_connect_info_data_mgr.GetConnectionInformation().IsConnected)
        {
            m_commander.SendStickCommands();
        }
    }

    TelloStickCommandsObserver::TelloStickCommandsObserver(
        TelloTelemetry &telemetry,
        ISendStickCommands &commander,
        ILogDataConnectionInformationSupply &connect_info_data_mgr,
        std::shared_ptr<spdlog::logger> logger,
        spdlog::level::level_enum lvl)
        : m_commander(commander),
          m_connect_info_data_mgr(connect_info_data_mgr),
          m_logger(logger)
    {
        m_logger->set_level(lvl);
        m_logger->info(m_logger->name() + " Initiated.");

        telemetry.Attach(this);
    }

    TelloStickCommandsObserver::~TelloStickCommandsObserver()
    {
        m_logger->info(m_logger->name() + " Destructed.");
    }
} // namespace tello_protocol

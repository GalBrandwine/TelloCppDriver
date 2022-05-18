#include "TelloLogDataMsgObserver.hpp"

namespace tello_protocol
{
    void TelloLogDataMsgObserver::Update(const std::vector<unsigned char> &message_from_subject)
    {

        unsigned short cmd;
        std::memcpy(&cmd, &message_from_subject[5], sizeof(unsigned short));

        if (cmd == tello_protocol::LOG_DATA_MSG)
        {
            try
            {
                std::vector<unsigned char> trimmed(message_from_subject.begin() + 10, message_from_subject.end());
                m_log_data_processor->Update(trimmed);
            }
            catch (const std::out_of_range &e)
            {
                m_logger->error(e.what());
                return;
            }

            m_log_data_msg_data_mgr.SetLogData(m_log_data_processor);
        }
    }

    TelloLogDataMsgObserver::TelloLogDataMsgObserver(ISubject &telemetry, ILogDataMsgDataManager &log_data_msg_data_mgr, std::shared_ptr<spdlog::logger> logger, spdlog::level::level_enum lvl)
        : m_telemetry(telemetry),
          m_log_data_msg_data_mgr(log_data_msg_data_mgr),
          m_logger(logger)
    {
        m_logger->set_level(lvl);
        m_logger->info(m_logger->name() + " Initiated.");

        m_log_data_processor = std::make_shared<tello_protocol::LogData>(spdlog::stdout_color_mt("LogData"));

        this->m_telemetry.Attach(this);
    }

    TelloLogDataMsgObserver::~TelloLogDataMsgObserver()
    {
        m_logger->info(m_logger->name() + " Destructed.");
    }
} // namespace tello_protocol

#pragma once
#include "utils/tello_observer/IObserver.hpp"
#include "utils/tello_observer/ISubject.hpp"
#include "utils/data_manager/ILogHeaderMsgDataManager.hpp"
#include "protocol.hpp"
#include <memory>

#include "spdlog/spdlog.h"
#include "spdlog/fmt/bin_to_hex.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace tello_protocol
{
    /**
     * @brief Observe for LOG header information. \n
     * 
     * Apon new data arrival, Isubject updates its attached observers, and pass them the new data. \n
     * This IObserver parse the following data, and store in TelloDataManager: \n
     * * Log version
     * * Log build date
     * * Current Log ID (setting this data in TelloDataManager should trigger attached sendAckLogId(id) function).
     * 
     * Next, DataManager will notify its own observer apon new data:
     * * One of them will be SendAckLogObserver
     */
    class TelloLogHeaderMsgObserver : public IObserver
    {
    public:
        void Update(const std::vector<unsigned char> &message_from_subject) override;
        TelloLogHeaderMsgObserver(ISubject &telemetry, ILogHeaderMsgDataManager &log_header_msg_data_mgr, std::shared_ptr<spdlog::logger> logger, spdlog::level::level_enum lvl = spdlog::level::info);
        virtual ~TelloLogHeaderMsgObserver();

    private:
        ISubject &m_telemetry;
        ILogHeaderMsgDataManager &m_log_header_msg_data_mgr;
        std::shared_ptr<spdlog::logger> m_logger;
    };
} // namespace tello_protocol

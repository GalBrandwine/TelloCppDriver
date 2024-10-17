#pragma once
#include "utils/tello_observer/IDataMgrSubject.hpp"
// #include "utils/tello_observer/ISubject.hpp"
#include "utils/data_manager/ILogHeaderMsgDataManager.hpp"
#include "utils/ISendAckLog.hpp"
#include "protocol.hpp"
#include <memory>

#include "spdlog/spdlog.h"
#include "spdlog/fmt/bin_to_hex.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace tello_protocol
{
    /**
     * @brief Observe for LOG header information.
     * Upon receiving new data, store the following in the TelloDataManager:
     * * Log version
     * * Log build date
     * * Current Log ID (setting this data in TelloDataManager should trigger attached sendAckLogId(id) function).
     * 
     * Next, DataManager will notify its own observer upon new data:
     * * One of them will be SendAckLogObserver
     */
    class TelloAckLogHeaderIdSenderObserver : public IObserver
    {
    public:
        /**
         * @brief Update is callback for receiving new **log_header_id** message into TelloDataManager
         * 
         * @param message_from_subject  - std::vector<unsigned char> filled with one value only - AckLog ID.
         */
        void Update(const std::vector<unsigned char> &message_from_subject) override;
        TelloAckLogHeaderIdSenderObserver(ISendAckLog &commander, IDataMgrSubject &log_header_msg_data_mgr, std::shared_ptr<spdlog::logger> logger, spdlog::level::level_enum lvl = spdlog::level::info);
        virtual ~TelloAckLogHeaderIdSenderObserver();

    private:
        ISendAckLog &m_commander;
        IDataMgrSubject &m_data_mgr_subject;
        std::shared_ptr<spdlog::logger> m_logger;
    };
} // namespace tello_protocol

#pragma once
#include "protocol.hpp"
#include "LogNewMvoFeedback.hpp"
#include "LogNewImuAttiFeedback.hpp"
#include "ILogDataGetter.hpp"
#include "ILogDataUpdater.hpp"

#include <mutex>
#define ID_NEW_MVO_FEEDBACK 29
#define ID_IMU_ATTI 2048

namespace tello_protocol
{
    /**
     * @brief LogData class preprocess raw data pefore extracting movements information out of it.
     * Based on the [TelloPy](https://github.com/hanyazou/TelloPy/blob/develop-0.7.0/tellopy/_internal/protocol.py#L316) preprocess methods.
     */
    class LogData
        : public ILogDataUpdater,
          public ILogDataGetter
    {
    public:
        /**
         * @brief Get the Log Imu Atti object
         * 
         * @return LogImuAtti& 
         */
        LogImuAtti &GetLogImuAtti() override;

        /**
         * @brief Get the Log Mvo object
         * 
         * @return LogNewMvoFeedback& 
         */
        LogNewMvoFeedback &GetLogMvo() override;

        /**
         * @brief Preprocess raw data before updating inner data containers (LogNewMvoFeedback and LogImuAtti)
         * Based on the [TelloPy](https://github.com/hanyazou/TelloPy/blob/develop-0.7.0/tellopy/_internal/protocol.py#L316) preprocess methods.
         * 
         * @param data filled with raw bytes received in IReceiver socket.
         */
        void Update(const std::vector<unsigned char> &data);

        LogData(std::shared_ptr<spdlog::logger>);
        ~LogData();
        // friend std::ostream &operator<<(std::ostream &os, const LogData &dt)
        // {
        //     os << "LogMvo: " << dt.m_LogMvoFeedback << '\n';
        //     os << "LogImuAtti: " << dt.m_LogImuAtti << '\n';
        //     return os;
        // }

    private:
        std::mutex m_log_data_mutex;
        std::shared_ptr<spdlog::logger> m_logger;
        int m_count = 0;
        std::vector<unsigned short> m_UnknownIDs;
        LogNewMvoFeedback m_LogMvoFeedback;
        LogImuAtti m_LogImuAtti;
    };
} // namespace tello_protocol
#pragma once
#include "protocol.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/bin_to_hex.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "utils/telemetry_data/TelemetryData.hpp"
namespace tello_protocol
{
    /**
     * @brief Supply Position and velocity log:
     * Class for extracting Position and velocity information from the raw bytestream.
     * Based on the [TelloPy](https://github.com/hanyazou/TelloPy/blob/develop-0.7.0/tellopy/_internal/protocol.py#L353) extracting methods.
     * 
     */
    class LogNewMvoFeedback
    {
    public:
        // const int GetUpdateCounter() const { return m_count; };
        LogNewMvoFeedback();
        ~LogNewMvoFeedback();

        /**
         * @brief Parse movement data from preprocessed [still raw] data.
         * 
         * @param[in] data preprocessed data, stripped from un-important bytes. Contain raw information about pos and vel.
         * @param[in] count amount of preprocessed data. Update function expects the preprocessed data to be in curtain size.
         */
        void Update(const std::vector<unsigned char> &data, int count = 0);

        /**
         * @brief Get Pose and velocity, if updated.
         * 
         * @param[out] posVelOut 
         * @return true - if successfully updated the velOut
         * @return false - otherwise
         */
        bool GetPosVelIfUpdated(PoseVelData &posVelOut);

    private:
        const std::string m_data;
        int m_count = 0;
        bool m_is_pos_vel_updated = false;
        PoseVelData m_PoseVel;
    };

} // namespace tello_protocol
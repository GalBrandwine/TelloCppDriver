#include "log_data/LogNewMvoFeedback.hpp"

namespace tello_protocol
{

    LogNewMvoFeedback::LogNewMvoFeedback()
    {
    }

    LogNewMvoFeedback::~LogNewMvoFeedback()
    {
    }
    void LogNewMvoFeedback::Update(const std::vector<unsigned char> &data, int count)
    {
        m_count = count;
        short tempS;
        std::memcpy(&tempS, &data[2], sizeof(short));
        m_PoseVel.vel.x = float(tempS) / 100.0f;

        std::memcpy(&tempS, &data[4], sizeof(short));
        m_PoseVel.vel.y = float(tempS) / 100.0f;

        std::memcpy(&tempS, &data[6], sizeof(short));
        m_PoseVel.vel.z = float(tempS) / 100.0f;

        float tempF;
        std::memcpy(&tempF, &data[8], sizeof(float));
        m_PoseVel.pose.x = tempF;

        std::memcpy(&tempF, &data[12], sizeof(float));
        m_PoseVel.pose.y = tempF;

        std::memcpy(&tempF, &data[16], sizeof(float));
        m_PoseVel.pose.z = tempF;
        m_is_pos_vel_updated = true;
    }

    bool LogNewMvoFeedback::GetPosVelIfUpdated(PoseVelData &posVelOut)
    {
        if (m_is_pos_vel_updated)
        {
            posVelOut = m_PoseVel;
            m_is_pos_vel_updated = false;
            return true;
        }
        return false;
    }

} // namespace tello_protocol

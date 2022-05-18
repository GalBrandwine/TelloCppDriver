#include "log_data/LogNewImuAttiFeedback.hpp"

namespace tello_protocol
{

    LogImuAtti::LogImuAtti()
    {
    }

    LogImuAtti::~LogImuAtti()
    {
    }

    void LogImuAtti::Update(const std::vector<unsigned char> &data, int count)
    {
        m_count = count;

        // (self.acc_x, self.acc_y, self.acc_z) = struct.unpack_from('fff', data, 20)
        std::memcpy(&m_imuAtti.acc.x, &data[20], sizeof(float));
        std::memcpy(&m_imuAtti.acc.y, &data[24], sizeof(float));
        std::memcpy(&m_imuAtti.acc.z, &data[28], sizeof(float));

        // (self.gyro_x, self.gyro_y, self.gyro_z) = struct.unpack_from('fff', data, 32)
        std::memcpy(&m_imuAtti.gyro.x, &data[32], sizeof(float));
        std::memcpy(&m_imuAtti.gyro.y, &data[36], sizeof(float));
        std::memcpy(&m_imuAtti.gyro.z, &data[40], sizeof(float));

        // (self.q0, self.q1, self.q2, self.q3) = struct.unpack_from('ffff', data, 48)
        std::memcpy(&m_imuAtti.quat.x, &data[48], sizeof(float));
        std::memcpy(&m_imuAtti.quat.y, &data[52], sizeof(float));
        std::memcpy(&m_imuAtti.quat.z, &data[56], sizeof(float));
        std::memcpy(&m_imuAtti.quat.w, &data[60], sizeof(float));

        // (self.vg_x, self.vg_y, self.vg_z) = struct.unpack_from('fff', data, 76)
        std::memcpy(&m_imuAtti.vg.x, &data[76], sizeof(float));
        std::memcpy(&m_imuAtti.vg.y, &data[80], sizeof(float));
        std::memcpy(&m_imuAtti.vg.z, &data[84], sizeof(float));
        m_is_imu_atti_updated = true;
    }

    bool LogImuAtti::GetImuAttiIfUpdated(ImuAttitudeData &imuAttiOut)
    {
        if (m_is_imu_atti_updated)
        {
            imuAttiOut = m_imuAtti;
            m_is_imu_atti_updated = false;
            return true;
        }
        return false;
    }
} // namespace tello_protocol

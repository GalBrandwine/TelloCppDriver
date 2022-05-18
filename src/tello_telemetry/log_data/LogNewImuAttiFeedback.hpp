#pragma once
#include "protocol.hpp"
#include "utils/telemetry_data/TelemetryData.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/bin_to_hex.h"
#include "spdlog/sinks/stdout_color_sinks.h"
namespace tello_protocol
{

    class LogImuAtti
    {
    public:
        bool GetImuAttiIfUpdated(ImuAttitudeData &imuAttiOut);
        void Update(const std::vector<unsigned char> &data, int count = 0);
        LogImuAtti();
        ~LogImuAtti();

    private:
        // std::shared_ptr<spdlog::logger> m_logger;
        int m_count = 0;
        ImuAttitudeData m_imuAtti;
        bool m_is_imu_atti_updated = false;
    };

} // namespace tello_protocol

/* 
def __init__(self, log = None, data = None):
        self.log = log
        self.count = 0
        self.vel_x = 0.0
        self.vel_y = 0.0
        self.vel_z = 0.0
        self.pos_x = 0.0
        self.pos_y = 0.0
        self.pos_z = 0.0
        if (data != None):
            self.update(data, count)

    def __str__(self):
        return (
            ("VEL: %5.2f %5.2f %5.2f" % (self.vel_x, self.vel_y, self.vel_z))+
            (" POS: %5.2f %5.2f %5.2f" % (self.pos_x, self.pos_y, self.pos_z))+
            "")

    def format_cvs(self):
        return (
            ("%f,%f,%f" % (self.vel_x, self.vel_y, self.vel_z))+
            (",%f,%f,%f" % (self.pos_x, self.pos_y, self.pos_z))+
            "")

    def format_cvs_header(self):
        return (
            "mvo.vel_x,mvo.vel_y,mvo.vel_z" + 
            ",mvo.pos_x,mvo.pos_y,mvo.pos_z" +
            "")

    def update(self, data, count = 0):
        self.log.debug('LogNewMvoFeedback: length=%d %s' % (len(data), byte_to_hexstring(data)))
        self.count = count
        (self.vel_x, self.vel_y, self.vel_z) = struct.unpack_from('<hhh', data, 2)
        self.vel_x /= 100.0
        self.vel_y /= 100.0
        self.vel_z /= 100.0
        (self.pos_x, self.pos_y, self.pos_z) = struct.unpack_from('fff', data, 8)
        self.log.debug('LogNewMvoFeedback: ' + str(self))
 */
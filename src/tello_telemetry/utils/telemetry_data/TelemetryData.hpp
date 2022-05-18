#pragma once
#include <vector>
namespace tello_protocol
{
    struct Vec3
    {
        float x, y, z;
    };
    struct Vec4
    {
        float x, y, z, w;
    };

    struct PoseVelData
    {
        Vec3 pose, vel;
    };

    struct ImuAttitudeData
    {
        Vec3 acc, gyro, vg;
        Vec4 quat;
    };

    struct PowerOnTimerInfo
    {
        short power_on_timer = 0;
        bool undocumented_bool = false;
    };

    struct FlightDataFrontInfo
    {
        bool front_in = false;
        bool front_out = false;
        bool front_lsc = false;
    };

    struct FlightDataExtras
    {
        bool em_sky = false;
        bool em_ground = false;
        bool em_open = false;
        bool drone_hover = false;
        bool outage_recording = false;
        bool battery_low = false;
        bool battery_lower = false;
        bool factory_mode = false;
    };

    struct FlightDataStateFlags
    {
        bool imu_state = false;
        bool pressure_state = false;
        bool down_visual_state = false;
        bool power_state = false;
        bool battery_state = false;
        bool gravity_state = false;
        bool wind_state = false;
    };

    /**
     * @brief FlightData contains all data relevant for flight conditions.
     * E.G wifi strength, battery left, flight mode.
     * 
     * @note Some of these data are not documented nor received from drone. If you know better, feel free to pull request :)
     * 
     */
    struct FlightDataStruct
    {

        char battery_percentage = -1;
        char camera_state = -1;
        short drone_battery_left = -1;
        short drone_fly_time_left = -1;
        float east_speed = -1;
        char electrical_machinery_state = -1;
        char fly_mode = -1;
        double fly_speed = -1;
        float fly_time = -1;
        float ground_speed = -1;
        short height = -1;
        double imu_calibration_state = -1;
        double light_strength = -1;
        float north_speed = -1;
        double smart_video_exit_mode = -1;
        double temperature_height = -1;
        char throw_fly_timer = -1;
        double wifi_disturb = -1;
        short wifi_strength = -1;
        short alt_limit = -1;
        float attitude_limit = -1;
        short low_battery_threshold = -1;
        FlightDataStateFlags flight_data_states;
        FlightDataExtras flight_data_extras;
        FlightDataFrontInfo front_info;
        PowerOnTimerInfo power_on_timer_info;
    };
    /**
     * @brief 
     * 
     */
    struct ConnectionInformation
    {
        bool IsConnected = false;
    };

    /**
     * @brief Store LogHeaderMsg information
     * 
     */
    struct LogHeaderInformation
    {
        std::vector<unsigned char> BuildDate, DJILogVersion, LogId;
    };

    /**
    * @brief Fix stick ranges when using SetRoll, SetThrottle, SetPitch, SetYaw
    * 
    * @param val float - the desired amount
    * @param min 
    * @param max 
    * @return float 
    */
    static const float FixRange(float val, float min = -1.0, float max = 1.0)
    {
        if (val < min)
            return min;
        if (val > max)
            return max;
        return val;
    }

} // namespace tello_protocol

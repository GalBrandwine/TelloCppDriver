#include "flight_data/FlightData.hpp"

namespace tello_protocol
{

    void FlightData::GetFlightData(tello_protocol::FlightDataStruct &flight_data_out)
    {
        // flight_data_out.alt_limit;       // Dont copy, this data is received elsewhere.
        // flight_data_out.wifi_strength;   // Dont copy, this data is received elsewhere.
        // flight_data_out.attitude_limit = m_flight_data.attitude_limit;  // Dont copy, this data is received elsewhere.
        // flight_data_out.low_battery_threshold = m_flight_data.low_battery_threshold; // Dont copy, this data is received elsewhere.
        flight_data_out.battery_percentage = m_flight_data.battery_percentage;
        flight_data_out.camera_state = m_flight_data.camera_state;
        flight_data_out.drone_battery_left = m_flight_data.drone_battery_left;
        flight_data_out.drone_fly_time_left = m_flight_data.drone_fly_time_left;
        flight_data_out.east_speed = m_flight_data.east_speed;
        flight_data_out.electrical_machinery_state = m_flight_data.electrical_machinery_state;
        flight_data_out.flight_data_extras = m_flight_data.flight_data_extras;
        flight_data_out.flight_data_states = m_flight_data.flight_data_states;
        flight_data_out.fly_mode = m_flight_data.fly_mode;
        flight_data_out.fly_speed = m_flight_data.fly_speed;
        flight_data_out.fly_time = m_flight_data.fly_time;
        flight_data_out.front_info = m_flight_data.front_info;
        flight_data_out.ground_speed = m_flight_data.ground_speed;
        flight_data_out.height = m_flight_data.height;
        flight_data_out.imu_calibration_state = m_flight_data.imu_calibration_state;
        flight_data_out.light_strength = m_flight_data.light_strength;
        flight_data_out.north_speed = m_flight_data.north_speed;
        flight_data_out.smart_video_exit_mode = m_flight_data.smart_video_exit_mode;
        flight_data_out.temperature_height = m_flight_data.temperature_height;
        flight_data_out.throw_fly_timer = m_flight_data.throw_fly_timer;
        flight_data_out.wifi_disturb = m_flight_data.wifi_disturb;
    }

    bool FlightData::Update(const std::vector<unsigned char> &data)
    {
        if (data.size() < 24)
        {
            if (m_logger)
                m_logger->warn("data length didn't match.");
            return false;
        }

        std::memcpy(&m_flight_data.height, &data[0], sizeof(short));

        // NED speeds
        std::memcpy(&m_flight_data.north_speed, &data[2], sizeof(short));
        // m_flight_data.north_speed = temp;
        // m_north_speed = int16(data[2], data[3]);

        std::memcpy(&m_flight_data.east_speed, &data[4], sizeof(short));
        // m_flight_data.east_speed = temp;
        // m_east_speed = int16(data[4], data[5]);
        // m_logger->warn("east_speed {}", m_east_speed);

        // Other method for storing data from buffer to reference:
        // short temp;
        // std::memcpy(&temp, &data[6], sizeof(short));

        std::memcpy(&m_flight_data.ground_speed, &data[6], sizeof(short));
        // m_flight_data.ground_speed = temp;
        // m_ground_speed = int16(data[6], data[7]);
        // m_logger->warn("ground_speed {}", m_ground_speed);

        std::memcpy(&m_flight_data.fly_time, &data[8], sizeof(short));
        // m_logger->warn("fly_time {}", temp);

        // m_fly_time = int16(data[8], data[9]);
        // m_logger->warn("fly_time {}", temp);

        // std::memcpy(&temp, &data[10], sizeof(char));
        // m_logger->warn("fly_time {}", temp);
        // m_flight_data.fly_time = temp;

        // m_imu_state = ((data[10] >> 0) & 0x1);
        m_flight_data.flight_data_states.imu_state = bool((data[10] >> 0) & 0x1);
        // m_logger->warn("imu_state {}", m_flight_data.flight_data_states.imu_state);
        // m_logger->warn("m_imu_state {}", m_imu_state);
        // m_logger->warn("");

        // m_pressure_state = ((data[10] >> 1) & 0x1);
        m_flight_data.flight_data_states.pressure_state = bool((data[10] >> 1) & 0x1);
        // m_logger->warn("pressure_state {}", m_flight_data.flight_data_states.pressure_state);
        // m_logger->warn("m_pressure_state {}", m_imu_state);
        // m_logger->warn("");

        // m_down_visual_state = ((data[10] >> 2) & 0x1);
        m_flight_data.flight_data_states.down_visual_state = bool((data[10] >> 2) & 0x1);
        // m_logger->warn("down_visual_state {}", m_flight_data.flight_data_states.down_visual_state);
        // m_logger->warn("m_down_visual_state {}", m_down_visual_state);
        // m_logger->warn("");

        // m_power_state = ((data[10] >> 3) & 0x1);
        m_flight_data.flight_data_states.power_state = bool((data[10] >> 3) & 0x1);
        // m_logger->warn("power_state {}", m_flight_data.flight_data_states.power_state);
        // m_logger->warn("m_power_state {}", m_power_state);
        // m_logger->warn("");

        // m_battery_state = ((data[10] >> 4) & 0x1);
        m_flight_data.flight_data_states.battery_state = bool((data[10] >> 4) & 0x1);
        // m_logger->warn("battery_state {}", m_flight_data.flight_data_states.battery_state);
        // m_logger->warn("m_battery_state {}", m_battery_state);
        // m_logger->warn("");

        // m_gravity_state = ((data[10] >> 5) & 0x1);
        m_flight_data.flight_data_states.gravity_state = bool((data[10] >> 5) & 0x1);
        // m_logger->warn("gravity_state {}", m_flight_data.flight_data_states.gravity_state);
        // m_logger->warn("m_gravity_state {}", m_gravity_state);
        // m_logger->warn("");

        // m_wind_state = ((data[10] >> 7) & 0x1);
        m_flight_data.flight_data_states.wind_state = bool((data[10] >> 7) & 0x1);
        // m_logger->warn("wind_state {}", m_flight_data.flight_data_states.wind_state);
        // m_logger->warn("m_wind_state {}", m_wind_state);
        // m_logger->warn("");

        // m_imu_calibration_state = data[11];
        m_flight_data.camera_state = data[11];
        // m_battery_percentage = data[12];
        m_flight_data.battery_percentage = data[12];

        // m_drone_battery_left = int16(data[13], data[14]);
        std::memcpy(&m_flight_data.drone_battery_left, &data[13], sizeof(short));
        // m_drone_fly_time_left = int16(data[15], data[16]);
        std::memcpy(&m_flight_data.drone_fly_time_left, &data[15], sizeof(short));

        // m_em_sky = ((data[17] >> 0) & 0x1);
        m_flight_data.flight_data_extras.em_sky = bool((data[17] >> 0) & 0x1);
        // m_em_ground = ((data[17] >> 1) & 0x1);
        m_flight_data.flight_data_extras.em_ground = bool((data[17] >> 1) & 0x1);
        // m_em_open = ((data[17] >> 2) & 0x1);
        m_flight_data.flight_data_extras.em_open = bool((data[17] >> 2) & 0x1);
        // m_drone_hover = ((data[17] >> 3) & 0x1);
        m_flight_data.flight_data_extras.drone_hover = bool((data[17] >> 3) & 0x1);
        // m_outage_recording = ((data[17] >> 4) & 0x1);
        m_flight_data.flight_data_extras.outage_recording = bool((data[17] >> 4) & 0x1);
        // m_battery_low = ((data[17] >> 5) & 0x1);
        m_flight_data.flight_data_extras.battery_low = bool((data[17] >> 5) & 0x1);
        // m_battery_lower = ((data[17] >> 6) & 0x1);
        m_flight_data.flight_data_extras.battery_lower = bool((data[17] >> 6) & 0x1);
        // m_factory_mode = ((data[17] >> 7) & 0x1);
        m_flight_data.flight_data_extras.factory_mode = bool((data[17] >> 7) & 0x1);

        // m_fly_mode = data[18];
        m_flight_data.fly_mode = data[18];
        // m_logger->warn("fly_mode: {}", std::to_string(m_flight_data.fly_mode));
        // m_throw_fly_timer = data[19];
        m_flight_data.throw_fly_timer = data[19];
        // m_camera_state = data[20];
        m_flight_data.camera_state = data[20];
        // m_electrical_machinery_state = data[21];
        m_flight_data.electrical_machinery_state = data[21];
        // m_front_in = ((data[22] >> 0) & 0x1);
        m_flight_data.front_info.front_in = bool((data[22] >> 0) & 0x1);
        // m_front_out = ((data[22] >> 1) & 0x1);
        m_flight_data.front_info.front_out = bool((data[22] >> 1) & 0x1);
        // m_front_lsc = ((data[22] >> 2) & 0x1);
        m_flight_data.front_info.front_lsc = bool((data[22] >> 3) & 0x1);
        m_flight_data.temperature_height = bool((data[23] >> 0) & 0x1);
        // m_temperature_height = ((data[23] >> 0) & 0x1);
        return true;
    };

    FlightData::FlightData(std::shared_ptr<spdlog::logger> logger)
        : m_logger(logger)
    {
    }
    FlightData::~FlightData()
    {
        m_logger->info(m_logger->name() + " Destructed.");
    };
} // namespace tello_protocol
#pragma once
#include "TelloDriver/TelloDriver.hpp"

class Dummy : public IFlightDataObserver
{
private:
    tello_protocol::FlightDataStruct m_flight_data;

public:
    void Update(const tello_protocol::FlightDataStruct &flight_data) override
    {
        m_flight_data = flight_data;
        auto fly_mode_name = tello_protocol::FlyModes::ToName(m_flight_data.fly_mode);
        std::stringstream ss;
        ss << "fly_mode_name: " << fly_mode_name << "\n"
           << "fly_mode: " << std::to_string(m_flight_data.fly_mode) << "\n"
           //    << "fly_speed: " << m_flight_data.fly_speed << "\n"
           //    << "east_speed: " << m_flight_data.east_speed << "\n"
           //    << "north_speed: " << m_flight_data.north_speed << "\n"
           //    << "ground_speed: " << m_flight_data.ground_speed << "\n"
           << "drone_hover: " << m_flight_data.flight_data_extras.drone_hover << "\n"
           << "down_visual_state: " << m_flight_data.flight_data_states.down_visual_state << "\n"
           << "height: " << m_flight_data.height << "\n"
           << "battery_percentage: " << std::to_string(m_flight_data.battery_percentage) << "\n\n";

        std::cout << ss.str();
    };
    void Update(const std::vector<unsigned char> &message_from_subject) override{};
    const tello_protocol::FlightDataStruct &GetFlightData() const { return m_flight_data; };
    Dummy(/* args */){};
    ~Dummy() { std::cout << "\nDestructing dummy\n"; };
};
#include "TelloDriver/TelloDriver.hpp"
#include <iostream>

/**
 * @brief This is an example for creating user observer and attaching it to TelloDriver.
 * This class can be attached to:
 * * a specific IPositionVelocityObserver, for getting new Pose and velocity
 * * a specific IImuAttitudeObserver, for getting new Imu gyre attitude ans such.
 * * a specific IFlightDataObserver, for getting new flight data like battery %, and some flags like "flight_data_extras.battery_low".
 * * and to a generic IObserver, for getting standart std::vector<unsigned char> and interprating it the user way.
 */
class PosObserver : public IPositionVelocityObserver,
                    public IFlightDataObserver,
                    public IImuAttitudeObserver
{
public:
    /**
     * @brief Must be overriden.
     * 
     * @param[in] message_from_subject - data filled with information relenavt to this observer.
     */
    void Update(const tello_protocol::ImuAttitudeData &imu_attitude) override;
    void Update(const tello_protocol::FlightDataStruct &flight_data) override;
    void Update(const tello_protocol::PoseVelData &pos_vel) override;
    void Update(const std::vector<unsigned char> &message_from_subject) override;
    PosObserver(/* args */);
    ~PosObserver();

    const tello_protocol::FlightDataStruct &GetFlightData() const { return m_flight_data; };
    const tello_protocol::ImuAttitudeData &GetImuAttitudeData() const { return m_imu_attitude_data; };
    const tello_protocol::PoseVelData &GetPosVelData() const { return m_pos_vel_data; };

private:
    tello_protocol::FlightDataStruct m_flight_data;
    tello_protocol::ImuAttitudeData m_imu_attitude_data;
    tello_protocol::PoseVelData m_pos_vel_data;
};

PosObserver::PosObserver(/* args */)
{
}

PosObserver::~PosObserver()
{
}

void PosObserver::Update(const tello_protocol::ImuAttitudeData &imu_attitude)
{
    m_imu_attitude_data = imu_attitude;
}

void PosObserver::Update(const tello_protocol::FlightDataStruct &flight_data)
{
    m_flight_data = flight_data;
}

void PosObserver::Update(const tello_protocol::PoseVelData &pos_vel)
{
    m_pos_vel_data = pos_vel;
}

void PosObserver::Update(const std::vector<unsigned char> &message_from_subject)
{
    std::string s(message_from_subject.begin(), message_from_subject.end());
    // std::cout << "received tello current log version: " << s << "\n";
}

int main()
{

    TelloDriver tello(spdlog::level::info);
    tello.Connect();
    
    std::cout << "Attaching user observer\n";
    PosObserver pos_vel_obs;

    tello.Attach(OBSERVERS::POSITION_VELOCITY_LOG, &pos_vel_obs);
    tello.Attach(OBSERVERS::DJI_LOG_VERSION, &pos_vel_obs);
    tello.Attach(OBSERVERS::FLIGHT_DATA_MSG, &pos_vel_obs);

    std::cout << "Starting simple connection\n";
    if (!tello.WaitForConnection(10))
    {
        tello.GetLogger()->error("Connection error. exiting!");
        exit(1);
    }

    while (1)
    {
        if (!pos_vel_obs.GetFlightData().flight_data_extras.battery_low)
        {
            // tello.GetLogger()->info("Battery: {}", std::to_string(pos_vel_obs.GetFlightData().battery_percentage));
        }
        else if (!pos_vel_obs.GetFlightData().flight_data_extras.battery_lower)
        {
            tello.GetLogger()->warn("Battery too low!");
        }
        else
        {
            tello.GetLogger()->error("Battery too lowest!");
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
/** \example simple_connection.cpp
 * Adding this footer will make sure that doxygen adds this source file to examples
*/
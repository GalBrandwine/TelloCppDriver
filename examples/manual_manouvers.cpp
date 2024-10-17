#include "TelloDriver/TelloDriver.hpp"
#include <iostream>

/*******************************************
 * 
 * @brief This is Manual maneuvers example.
 * It is another example for the usage of the TelloDriver.
 * 
 * @note This example uses close-loop operation. \n
 * - Takeoff \n 
 *   - wait until mode is tello_protocol::FlyModes::HOLDING_POSITION \n
 * - SetRoll in a range from 0 -> 0.5 -> 0 -> -0.5 \n
 * - Land \n
 *   - wait until mode is tello_protocol::FlyModes::LANDING \n
 * 
 * *******************************************/

class Examples : public IPositionVelocityObserver, IFlightDataObserver
{
public:
    /**
     * @brief Must be overriden.
     * 
     */
    void Update(const tello_protocol::FlightDataStruct &flight_data) override { m_FlightData = flight_data; };
    void Update(const tello_protocol::PoseVelData &pos_vel) override { m_PoseVel = pos_vel; };
    void Update(const std::vector<unsigned char> &message_from_subject) override{};

    const tello_protocol::FlightDataStruct &GetExampleFlightData() { return m_FlightData; };
    const tello_protocol::PoseVelData &GetExamplePoseVelData() { return m_PoseVel; };

    Examples(/* args */){};
    ~Examples(){};

private:
    tello_protocol::FlightDataStruct m_FlightData;
    tello_protocol::PoseVelData m_PoseVel;
};

int main()
{
    // Setup
    TelloDriver tello(spdlog::level::info);
    Examples examples;
    tello.Attach(OBSERVERS::FLIGHT_DATA_MSG, &examples);
    tello.Attach(OBSERVERS::POSITION_VELOCITY_LOG, &examples);
    // tello.Attach(OBSERVERS::DJI_LOG_VERSION, this); /** @brief It is not mandatory to attach every overriden observer. */

    /**
     * @section Tello ManualManeuvers
     * @brief Manual takeoff, then perform some roll operations.
     * 
     * @note This is a manual example for movements. One can use the HighLevel commands TelloDriver::Left(amount) or TelloDriver::Right(amount)
     * 
     */

    tello.Connect();
    tello.GetLogger()->info("Starting: " + std::string(__PRETTY_FUNCTION__));

    if (!tello.WaitForConnection(10))
    {
        tello.GetLogger()->error("Connection error. exiting!");
        return 1;
    }

    tello.GetLogger()->info("Current fly_mode: {} [{}]", tello_protocol::FlyModes::ToName(examples.GetExampleFlightData().fly_mode), std::to_string(examples.GetExampleFlightData().fly_mode));
    tello.GetLogger()->info("Sending Takeoff command!");

    tello.Takeoff();

    while (examples.GetExampleFlightData().fly_mode < tello_protocol::FlyModes::TAKING_OFF)
    {
        tello.GetLogger()->info("Waiting for takeoff. Current fly_mode: {} [{}]", tello_protocol::FlyModes::ToName(examples.GetExampleFlightData().fly_mode), std::to_string(examples.GetExampleFlightData().fly_mode));
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    while (examples.GetExampleFlightData().fly_mode == tello_protocol::FlyModes::TAKING_OFF)
    {
        tello.GetLogger()->info("Takeoff. Current fly_mode: {} [{}]", tello_protocol::FlyModes::ToName(examples.GetExampleFlightData().fly_mode), std::to_string(examples.GetExampleFlightData().fly_mode));
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    tello.GetLogger()->info("{} reached", tello_protocol::FlyModes::ToName(examples.GetExampleFlightData().fly_mode));

    float roll_amount = 0;
    int timer_ms = 150;
    while (roll_amount <= 0.5)
    {
        tello.GetLogger()->info("Sending roll {}", roll_amount);
        tello.SetRoll(roll_amount);
        roll_amount += 0.1;
        std::this_thread::sleep_for(std::chrono::milliseconds(timer_ms));
    }

    while (roll_amount >= -0.5)
    {
        tello.GetLogger()->info("Sending roll {}", roll_amount);
        tello.SetRoll(roll_amount);
        roll_amount -= 0.1;
        std::this_thread::sleep_for(std::chrono::milliseconds(timer_ms));
    }

    tello.SetRoll(0);

    tello.GetLogger()->info("Sending Land command!");
    tello.Land();

    while (examples.GetExampleFlightData().fly_mode < tello_protocol::FlyModes::LANDING)
    {
        tello.GetLogger()->info("Waiting for Landing. Current fly_mode: {} [{}]", tello_protocol::FlyModes::ToName(examples.GetExampleFlightData().fly_mode), std::to_string(examples.GetExampleFlightData().fly_mode));
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    while (examples.GetExampleFlightData().fly_mode == tello_protocol::FlyModes::LANDING)
    {
        tello.GetLogger()->info("Landing. Current fly_mode: {} [{}]", tello_protocol::FlyModes::ToName(examples.GetExampleFlightData().fly_mode), std::to_string(examples.GetExampleFlightData().fly_mode));
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    int empty_waiting_timer = 10;

    while (empty_waiting_timer-- > 0)
    {
        tello.GetLogger()->info("Done Landing. Current fly_mode: {} [{}]", tello_protocol::FlyModes::ToName(examples.GetExampleFlightData().fly_mode), std::to_string(examples.GetExampleFlightData().fly_mode));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    tello.GetLogger()->info("Example is over");
    return 0;
}

/** \example manual_maneuvers.cpp
 * Adding this footer will make sure that doxygen adds this source file to examples
*/
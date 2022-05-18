#include "TelloDriver/TelloDriver.hpp"
#include <iostream>

/*******************************************
 * @brief In this example I show how to create an object the contains TelloDriver, and attach itself to TelloDriver.
 * 
 * *******************************************/

class Examples : public IFlightDataObserver
{
public:
    /**
     * @brief Must be overriden.
     * 
     */
    void Update(const tello_protocol::FlightDataStruct &flight_data) override;
    void Update(const std::vector<unsigned char> &message_from_subject){};
    /**
     * @brief With close-loop operation. \n
     * - ManualTakeoff \n 
     *   - This will make the props rotate slowly.
     *   - SendThrottle in ascending amount.
     *   - wait until mode is tello_protocol::FlyModes::HOLDING_POSITION \n
     * - SetRoll in a range from 0 -> 0.5 -> 0 -> -0.5 \n
     * - Land \n
     *   - wait until mode is tello_protocol::FlyModes::LANDING \n
     * 
     */
    void StartScenario();
    Examples(/* args */);
    ~Examples();

private:
    TelloDriver m_tello;
    tello_protocol::FlightDataStruct m_FlightData;
};

Examples::Examples()
    : m_tello(spdlog::level::info)
{
    m_tello.Attach(OBSERVERS::FLIGHT_DATA_MSG, this);
}

void Examples::StartScenario()
{
    int timer_ms = 500;
    float min_height_for_manual_takeoff = 3; //30cm

    m_tello.Connect();
    m_tello.GetLogger()->info("Starting: " + std::string(__PRETTY_FUNCTION__));
    if (!m_tello.WaitForConnection(10))
    {
        m_tello.GetLogger()->error("Connection error. exiting!");
        return;
    }

    m_tello.GetLogger()->info("Sending ManualTakeoff command!");
    m_tello.ManualTakeoff();
    while (m_FlightData.fly_mode < tello_protocol::FlyModes::MANUAL)
    {
        m_tello.GetLogger()->info("Waiting for entering MANUAL mode. Current fly_mode: {}", std::to_string(m_FlightData.fly_mode));
        std::this_thread::sleep_for(std::chrono::milliseconds(timer_ms));
    }

    m_tello.GetLogger()->info("Entered to manual mode. Resetting all sticks");
    m_tello.SetPitch(0);
    m_tello.SetYaw(0);
    m_tello.SetThrottle(0);
    m_tello.SetRoll(0);
   
    float throttle_amount = 0;

    while (throttle_amount <= 0.5 and fabs(m_FlightData.height) < min_height_for_manual_takeoff)
    {
        m_tello.GetLogger()->info("Current height {}, Sending thrust {}", m_FlightData.height, throttle_amount);
        m_tello.SetThrottle(throttle_amount);
        throttle_amount += 0.1;
        std::this_thread::sleep_for(std::chrono::milliseconds(timer_ms));
    }
    m_tello.GetLogger()->info("After while: Current height {}, Sending thrust {}", m_FlightData.height, throttle_amount);

    m_tello.GetLogger()->info("Sending Land command!");
    m_tello.Land();
    while (m_FlightData.fly_mode < tello_protocol::FlyModes::LANDING)
    {
        m_tello.GetLogger()->info("Waiting for landing. Current fly_mode: {}", std::to_string(m_FlightData.fly_mode));
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    while (m_FlightData.fly_mode == tello_protocol::FlyModes::LANDING)
    {
        m_tello.GetLogger()->info("Landing. Current fly_mode: {}", std::to_string(m_FlightData.fly_mode));
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    m_tello.GetLogger()->info(std::string(__PRETTY_FUNCTION__) + " over!");
}

Examples::~Examples()
{
}

void Examples::Update(const tello_protocol::FlightDataStruct &flight_data)
{
    m_FlightData = flight_data;
    m_tello.GetLogger()->info("FlightMode: {} [{}]", tello_protocol::FlyModes::ToName(m_FlightData.fly_mode), std::to_string(m_FlightData.fly_mode));
}

int main()
{
    Examples examples;
    examples.StartScenario();
}

/** \example manual_takeoff.cpp
 * Adding this footer will make sure that doxygen adds this source file to examples
*/
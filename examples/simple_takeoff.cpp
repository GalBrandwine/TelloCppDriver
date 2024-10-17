#include "TelloDriver/TelloDriver.hpp"
#include <iostream>

/*******************************************
 * @brief In this example I show how to create an object the contains TelloDriver, and attach itself to TelloDriver.
 * 
 * *******************************************/

class Examples : public IPositionVelocityObserver, IFlightDataObserver
{
public:
    /**
     * @brief Must be overriden.
     * 
     */
    void Update(const tello_protocol::FlightDataStruct &flight_data) override;
    void Update(const tello_protocol::PoseVelData &pos_vel) override;
    void Update(const std::vector<unsigned char> &message_from_subject) override;

    /**
     * @brief Scenario1 - No close loop operation. \n
     * 1. Move backward %1 for 1 sec \n
     * 2. Move forward %1 for 1 sec \n 
     * 3. Land
     * 
     */
    void StartScenario1();

    /**
     * @brief Scenario2 - No close loop operation. \n
     * 1. Takeoff \n 
     * 2. Turn Clockwise 30% \n
     * 3. Turn CounterClockwise 30% \n
     * 3. Land
     * 
     */
    void StartScenario2();

    /**
     * @brief Scenario3 - No close loop operation. \n
     * 1. Takeoff \n 
     * 2. Move Left 1% \n
     * 3. Move Right 1% \n
     * 3. Land
     * 
     */
    void StartScenario3();

    /**
     * @brief With close-loop operation. \n
     * - Takeoff \n 
     *   - wait until mode is tello_protocol::FlyModes::HOLDING_POSITION \n
     * - SetRoll in a range from \f$0 \rightarrow 0.5 \rightarrow 0 \rightarrow -0.5\f$ \n
     *   Rolling amount measured in \f$ \text{stick}\% \f$
     * - Land \n
     *   - wait until mode is tello_protocol::FlyModes::LANDING \n
     * 
     */
    void StartScenario4();
    void StartTakeoffAndLandScenario();
    void StartTakeoffMoveBackwardAndLandScenario();
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
    m_tello.Attach(OBSERVERS::POSITION_VELOCITY_LOG, this);
    m_tello.Attach(OBSERVERS::DJI_LOG_VERSION, this);
}

void Examples::StartTakeoffMoveBackwardAndLandScenario()
{
    m_tello.Connect();
    m_tello.GetLogger()->info("Starting simple connection");
    if (!m_tello.WaitForConnection(10))
    {
        m_tello.GetLogger()->error("Connection error. exiting!");
        return;
    }

    m_tello.GetLogger()->info("Sending Takeoff command!");
    m_tello.Takeoff();
    std::this_thread::sleep_for(std::chrono::seconds(5));
    m_tello.Backward(1);
    m_tello.GetLogger()->info("Sending Backward 1% command!");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    m_tello.GetLogger()->info("Sending Backward 0% command!");
    m_tello.Backward(0);
    m_tello.GetLogger()->info("Sending Land command!");
    m_tello.Land();
    std::this_thread::sleep_for(std::chrono::seconds(5));
    m_tello.GetLogger()->info("Scenario over!");
}

void Examples::StartScenario1()
{

    m_tello.Connect();
    m_tello.GetLogger()->info("Starting simple connection");
    if (!m_tello.WaitForConnection(10))
    {
        m_tello.GetLogger()->error("Connection error. exiting!");
        return;
    }
    int stick = 1;
    m_tello.GetLogger()->info("Sending Takeoff command!");
    m_tello.Takeoff();
    std::this_thread::sleep_for(std::chrono::seconds(5));

    m_tello.GetLogger()->info("Sending Backward {}% command!", stick);
    m_tello.Backward(stick);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    stick = 0;
    m_tello.GetLogger()->info("Sending Backward {}stick% command!", stick);
    m_tello.Backward(stick);

    stick = 5;
    m_tello.GetLogger()->info("Sending Forward {}% command!", stick);
    m_tello.Forward(stick);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    stick = 0;
    m_tello.GetLogger()->info("Sending Forward {}% command!", stick);
    m_tello.Forward(stick);

    m_tello.GetLogger()->info("Sending Land command!");
    m_tello.Land();
    std::this_thread::sleep_for(std::chrono::seconds(5));

    m_tello.GetLogger()->info("Scenario1 over!");
}
void Examples::StartScenario2()
{
    m_tello.Connect();
    m_tello.GetLogger()->info("Starting simple connection");
    if (!m_tello.WaitForConnection(10))
    {
        m_tello.GetLogger()->error("Connection error. exiting!");
        return;
    }

    m_tello.GetLogger()->info("Sending Takeoff command!");
    m_tello.Takeoff();
    std::this_thread::sleep_for(std::chrono::seconds(5));

    int stick = 30;
    m_tello.GetLogger()->info("Sending Clockwise {}% command!", stick);
    m_tello.Clockwise(stick);
    std::this_thread::sleep_for(std::chrono::seconds(5));

    stick = 0;
    m_tello.GetLogger()->info("Sending Clockwise {}% command!", stick);
    m_tello.Clockwise(stick);

    std::this_thread::sleep_for(std::chrono::seconds(5));

    stick = 30;
    m_tello.GetLogger()->info("Sending Counter-Clockwise {}% command!", stick);
    m_tello.CounterClockwise(stick);
    std::this_thread::sleep_for(std::chrono::seconds(5));

    stick = 0;
    m_tello.GetLogger()->info("Sending Counter-Clockwise {}% command!", stick);
    m_tello.CounterClockwise(stick);
    std::this_thread::sleep_for(std::chrono::seconds(5));

    m_tello.GetLogger()->info("Sending Land command!");
    m_tello.Land();
    std::this_thread::sleep_for(std::chrono::seconds(5));

    m_tello.GetLogger()->info("Scenario2 over!");
}
void Examples::StartScenario3()
{
    m_tello.Connect();
    m_tello.GetLogger()->info("Starting simple connection");
    if (!m_tello.WaitForConnection(10))
    {
        m_tello.GetLogger()->error("Connection error. exiting!");
        return;
    }

    m_tello.GetLogger()->info("Sending Takeoff command!");
    m_tello.Takeoff();
    std::this_thread::sleep_for(std::chrono::seconds(5));

    int stick = 5;
    m_tello.GetLogger()->info("Sending Left {}% command!", stick);
    m_tello.Left(stick);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // stick = 0;
    // m_tello.GetLogger()->info("Sending Clockwise {}% command!", stick);
    // m_tello.Clockwise(stick);

    // std::this_thread::sleep_for(std::chrono::seconds(5));

    // stick = 30;
    m_tello.GetLogger()->info("Sending Right {}% command!", stick);
    m_tello.Right(stick);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    stick = 0;
    m_tello.GetLogger()->info("Sending Right {}% command!", stick);
    m_tello.Right(stick);
    // std::this_thread::sleep_for(std::chrono::seconds(5));

    m_tello.GetLogger()->info("Sending Land command!");
    m_tello.Land();
    std::this_thread::sleep_for(std::chrono::seconds(5));

    m_tello.GetLogger()->info("Scenario3 over!");
}
void Examples::StartScenario4()
{
    m_tello.Connect();
    m_tello.GetLogger()->info("Starting: " + std::string(__PRETTY_FUNCTION__));
    if (!m_tello.WaitForConnection(10))
    {
        m_tello.GetLogger()->error("Connection error. exiting!");
        return;
    }

    m_tello.GetLogger()->info("Sending Takeoff command!");
    m_tello.Takeoff();
    while (m_FlightData.fly_mode < tello_protocol::FlyModes::TAKING_OFF)
    {
        m_tello.GetLogger()->info("Waiting for takeoff. Current fly_mode: {}", std::to_string(m_FlightData.fly_mode));
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    while (m_FlightData.fly_mode == tello_protocol::FlyModes::TAKING_OFF)
    {
        m_tello.GetLogger()->info("Takeoff. Current fly_mode: {}", std::to_string(m_FlightData.fly_mode));
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    m_tello.GetLogger()->info("{} reached", tello_protocol::FlyModes::ToName(m_FlightData.fly_mode));

    float roll_amount = 0;
    int timer_ms = 100;
    while (roll_amount <= 0.5)
    {
        m_tello.GetLogger()->info("Sending roll {}", roll_amount);
        m_tello.SetRoll(roll_amount);
        roll_amount += 0.1;
        std::this_thread::sleep_for(std::chrono::milliseconds(timer_ms));
    }

    while (roll_amount >= -0.5)
    {
        m_tello.GetLogger()->info("Sending roll {}", roll_amount);
        m_tello.SetRoll(roll_amount);
        roll_amount -= 0.1;
        std::this_thread::sleep_for(std::chrono::milliseconds(timer_ms));
    }
    m_tello.SetRoll(0);

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

void Examples::StartTakeoffAndLandScenario()
{
    m_tello.Connect();
    m_tello.GetLogger()->info("Starting simple connection");
    if (!m_tello.WaitForConnection(10))
    {
        m_tello.GetLogger()->error("Connection error. exiting!");
        return;
    }

    m_tello.GetLogger()->info("Sending Takeoff command!");
    m_tello.Takeoff();
    std::this_thread::sleep_for(std::chrono::seconds(10));
    m_tello.GetLogger()->info("Sending Land command!");
    m_tello.Land();
    std::this_thread::sleep_for(std::chrono::seconds(5));
    m_tello.GetLogger()->info("Scenario over!");
}

Examples::~Examples()
{
}

void Examples::Update(const tello_protocol::FlightDataStruct &flight_data)
{
    m_FlightData = flight_data;
    m_tello.GetLogger()->info("FlightMode: {} [{}]", tello_protocol::FlyModes::ToName(m_FlightData.fly_mode), std::to_string(m_FlightData.fly_mode));
}
void Examples::Update(const tello_protocol::PoseVelData &pos_vel)
{
    auto new_pos_vel = pos_vel;
    // m_tello.GetLogger()->info("new_pos_vel {} {} {}", new_pos_vel.pose.x, new_pos_vel.pose.y, new_pos_vel.pose.z);
}

void Examples::Update(const std::vector<unsigned char> &message_from_subject)
{
    std::string s(message_from_subject.begin(), message_from_subject.end());
    m_tello.GetLogger()->info("received tello current log version: " + s);
}

int main()
{
    Examples examples;

    // examples.StartScenario1();
    // examples.StartScenario2();
    // examples.StartScenario3();
    examples.StartScenario4();
    // examples.StartTakeoffAndLandScenario();
    // examples.StartTakeoffMoveBackwardAndLandScenario();
}

/** \example simple_takeoff.cpp
 * Adding this footer will make sure that doxygen adds this source file to examples
*/
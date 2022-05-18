#pragma once

#include <memory>
#include <chrono>

#include "spdlog/spdlog.h"
#include "spdlog/fmt/bin_to_hex.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include "utils/TelloSocket.hpp"
#include "TelloTelemetry.hpp"
#include "TelloCommander.hpp"
#include "utils/data_manager/DataManager.hpp"

#include "TelloPowerOnTimerMsgObserver.hpp"
#include "TelloLowBatThreshMsgObserver.hpp"
#include "TelloAttLimitMsgObserver.hpp"
#include "TelloAltLimitMsgObserver.hpp"
#include "TelloWifiMsgObserver.hpp"
#include "TelloConnAckMsgObserver.hpp"
#include "TelloLogDataMsgObserver.hpp"
#include "TelloFlightDataMsgObserver.hpp"
#include "TelloLogHeaderMsgObserver.hpp"
#include "TelloStickCommandsObserver.hpp"
#include "TelloAckLogHeaderIdSenderObserver.hpp"
#include "TelloDriverConfig.hpp"

#include <cstdlib>
#include <iostream>
#include "asio.hpp"
#include <thread>

/**
 * @file TelloDriver.hpp
 * @author your name (gal080592@gmail.com)
 * @brief The unofficial SDK Tello driver.
 * @date 2021-01-24
 * 
 * @copyright Copyright (c) 2021
 * 
 */

using namespace std::chrono_literals;
using asio::ip::udp;

#define CONNECTION_TIMEOUT 50
class TelloDriver
{
public:
    /**
    * @brief Movements commands
    **/
    void CounterClockwise(int amount);
    void Clockwise(int amount);
    void Down(int amount);
    void Up(int amount);
    void Right(int amount);
    void Left(int amount);
    void Backward(int amount);
    void Forward(int amount);
    /**
     * \section Movements-section
     * @brief Lowlevel movements, involves simple body related movement's
     * 
     * @brief Controls the vertical up and down motion of the drone.
     * 
     * @param throttle float from -1.0 ~ 1.0. (positive value means upward) 
     */
    void SetThrottle(float throttle);
    /**
     * @brief Controls the left and right rotation of the drone.
     * 
     * @param yaw float from -1.0 ~ 1.0. (positive value will make the drone turn to the right)
     */
    void SetYaw(float yaw);
    /**
     * @brief Controls the forward and backward tilt of the drone.
     * 
     * @param pitch float from -1.0 ~ 1.0. (positive value will make the drone move forward)
     */
    void SetPitch(float pitch);
    /**
     * @brief Controls the the side to side tilt of the drone.
     * 
     * @param roll float from -1.0 ~ 1.0. (positive value will make the drone move to the right)
     */
    void SetRoll(float roll);

    /**
     * @brief Hold max 'yaw' and min 'pitch', 'roll', 'throttle' for several seconds
     */
    void ManualTakeoff();

    /**
     * @section High level commands
     * @brief Send Flip commands to the drone in 8 directions.
     * DJI calls the 8D flipping, wasted BUZZ word in my opinion.
     * 
     * @note Drone will ignore Flip commands if battery below FlightDataStruct::low_battery_threshold.
     * 
     * @param direction 
     */
    void Flip(tello_protocol::FlipDirections direction);
    void ThrowAndGo();
    void PalmLand();
    void Takeoff();
    void Land();
    void Emergency();

    bool SetFastMode(bool fastMode);
    void SetAltLimitReq(int alt_limit);
    void SetAttLimitReq(float alt_limit);
    void SetBatThreshReq(int bat_thresh);

    std::shared_ptr<spdlog::logger> GetLogger() { return m_BaseLogger; };

    TelloDriver(spdlog::level::level_enum lvl = spdlog::level::info);
    ~TelloDriver();

    void Attach(OBSERVERS observer_type, IObserver *observer);
    void Connect();
    bool WaitForConnection(int);

private:
    tello_protocol::DataManager m_DataManager;

    /*************************************
     * 
     * @section Incoming data observers
     * 
    *************************************/

    /**
     * @brief TelloPowerOnTimerMsgObserver attached to TelloTelemetry via ISubject interface.
     * Observer for power on timer message.
     * 
     */
    std::shared_ptr<IObserver> m_TelloPowerOnTimerMsgObserver;

    /**
     * @brief TelloLowBatThreshMsgObserver attached to TelloTelemetry via ISubject interface.
     * Observe for LOW_BAT_THRESHOLD_MSG sent back from drone.
     */
    std::shared_ptr<IObserver> m_TelloLowBatThreshMsgObserver;

    /**
     * @brief TelloAttLimitMsgObserver attached to TelloTelemetry via ISubject interface.
     * Observe for AttLimitMsg sent back from drone.
     * 
     * Should contain the new attitude limit.
     */
    std::shared_ptr<IObserver> m_TelloAttLimitMsgObserver;

    /**
     * @brief TelloAltLimitMsgObserver attached to TelloTelemetry via ISubject interface.
     * Observe for AltLimitMsg sent back from drone.
     * Received only as an acknowledgment for a SetAltLimitCmd sent by the user.
     * 
     * Should contain the new altitude limit.
     */
    std::shared_ptr<IObserver> m_TelloAltLimitMsgObserver;

    /**
     * @brief TelloFlightDataMsgObserver attached to TelloTelemetry via ISubject interface.
     * Observe for Flight_Data.
     * If received, calls SetFlightData member function via IFlightDataMsgDataManager interface, supplied by DataManager.
     * 
     */
    std::shared_ptr<IObserver> m_TelloFlightDataMsgObserver;

    /**
     * @brief TelloWifiMsgObserver attached to TelloTelemetry via ISubject interface.
     * Observe for WIFI_MSG's.
     * If received, calls SetWifiMsg member function via IWifiMsgDataManager interface, supplied by DataManager.
     * 
     */
    std::shared_ptr<IObserver> m_TelloWifiMsgObserver;

    /**
     * @brief TelloLogHeaderMsgObserver attached to telloTelemetry via ISubject interface.
     * It being callback upon new data arival to TelloTelemtry. 
     * Then parse this new data, and insert it to TelloDataManager through ILogHeaderMsgDataManager interface.
     */
    std::shared_ptr<IObserver> m_TelloLogHeaderMsgObserver;

    /**
     * @brief TelloLogDataMsgObserver attached to TelloTelemetry via ISubject interface.
     * It being callback upon new data arival to TelloTelemtry. 
     * 
     * 1. Upon callback, it creates LogData object, then pass raw_data into this LogData.
     * 2. Pass this LogData (filled with processed log data E.G position, velocity etc) into TelloDataManager:
     * via SetLogData() that is available due to ILogDataMsgDataManager interface.
     * 
     */
    std::shared_ptr<IObserver> m_TelloLogDataMsgObserver;

    /**
     * @brief TelloConnAckMsgObserver attached to TelloTelemetry via ISubject interface.
     * Upon callback, this observer will check if raw_data contain \'conn_ack\'. 
     * If so this observer will call DataManager::SetConnReqAck()
     * 
     */
    std::shared_ptr<IObserver> m_TelloConnAckMsgObserver;

    /*************************************
     * 
     * @section Outgoing commands observers
     * 
    *************************************/

    /**
     * @brief TelloAckLogHeaderIdSenderObserver attached to TelloDataManager via IDataMgrSubject interface.
     * TelloDataManager trigger a callback upon receiving new AckLogHeaderMsg, send ack commands according to attached OBSERVER::ACK_MSG.
     */
    std::shared_ptr<IObserver> m_TelloAckLogHeaderIdSenderObserver;

    /**
     * @brief TelloStickCommandsObserver attached to telloTelemetry via ISubject interface.
     * It being callback upon new data arival to TelloTelemtry. And send stick commands
     * 
     * @note this callback acts as a heartbeat back to the drone, if not sent the drone will stand still and blink on ORANGE.
     */
    std::shared_ptr<IObserver> m_TelloStickCommandsObserver;

    tello_protocol::TelloCommander m_TelloCommander;
    tello_protocol::TelloTelemetry m_TelloTelemetry;
    std::shared_ptr<spdlog::logger> m_BaseLogger;
};

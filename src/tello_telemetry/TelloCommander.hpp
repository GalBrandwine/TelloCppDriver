#pragma once
#include "utils/movement_commands/MovementCommandsManager.hpp"
#include "utils/movement_commands/MovementsToPacketConverter.hpp"
#include "protocol.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/bin_to_hex.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "utils/ISender.hpp"
#include "utils/ISendAckLog.hpp"
#include "utils/ISendStickCommands.hpp"
namespace tello_protocol
{
    class TelloCommander : public ISendAckLog, public ISendStickCommands
    {
    public:
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
         * \section Flip-section
         * \brief This section expose high level flip commands, as implemented in [TelloPy](https://github.com/hanyazou/TelloPy/blob/develop-0.7.0/tellopy/_internal/tello.py#L328)
         * 
         */
        void Flip(tello_protocol::FlipDirections direction);

        /**
         * \section Movements-section
         * \brief This section expose movements commands, as implemented in [TelloPy](https://github.com/hanyazou/TelloPy/blob/develop-0.7.0/tellopy/_internal/tello.py#L328)
         * 
        **/
        void Down(int);
        void Up(int);
        void Clockwise(int);
        void CounterClockwise(int);
        void Left(int);
        void Right(int);
        void Backward(int);
        void Forward(int);
        bool SetFastMode(bool);

        /**
         * @brief Send stick commands.
         * This works like a *keep-alive* for the drone.
         * If not sent for too long. The TelloDrone will assume that companion (E.G remote, other controller) lost connection.
         * Thus drone will hover in place.
         * 
         * 1. Create empty packet with STICK_CMD header.
         * 2. Get movements from MovementCommandsManager
         * 3. Convert them stickCommands into packet data.
         * 4. Send to drone.
         * 
         * @return true if packet converter successfully
         * @return false otherwise
         */
        bool SendStickCommands() override;

        void SendLandReq();

        /**
         * @brief Sends EMERGENCY_CMD to the drone. 
         * @warning This will kill all motors at once.
         * 
         */
        void SendEmergencyCmd();
        /**
         * @brief Send LOW_BAT_THRESHOLD_CMD to drone, with new threshold.
         * 
         * @param threshold - new threshold to be set.
         */
        void SetLowBatThreshold(int threshold);

        /**
         * @brief Send to drone LOW_BAT_THRESHOLD_MSG.
         * In response the drone send back stored LowBatThresh
         * 
         */
        void GetLowBatThreshold();

        /**
         * @brief Send ATT_LIMIT_CMD to drone, with new LIMIT.
         * Call GetAttLimitReq(), for making sure the drone received new LIMIT.
         * 
         * @param limit - new limit to set. Must be above 31, or the drone will ignore this data, and return its default = 32 (I dont know why).
         */
        void SetAttLimitReq(int limit);

        /**
         * @brief Send ATT_LIMIT_MSG to drone.
         * In response the drone will send back ATT_LIMIT_MSG with att_limit data.
         * 
         */
        void GetAttLimitReq();

        /**
         * @brief Sen SetAltLimit command, with new limit.
         * After sending the packed, call GetAltLimitReq().
         * So drone will send back as acknowledge the new limit.
         * 
         * @param limit - new altitude limit.
         */
        void SetAltLimitReq(int limit);

        /**
         * @brief Send GetAltLimit request.
         * This will result with an incoming data from drone with id tello_protocol::ALT_LIMIT_MSG.
         * 
         * @todo Add observer on this incoming data to TelloTelemetry.
         * 
         */
        void GetAltLimitReq();

        /**
         * @brief Throw_and_go starts a throw and go sequence
         * 
         */
        void ThrowAndGo();

        /**
         * @brief Tells the drone to wait for a hand underneath it and then land
         * 
         */
        void PalmLand();

        /**
         * @section Video setting commands
         * @brief Set_exposure sets the drone camera exposure level. Valid levels are 0, 1, and 2.
         * 
         * @param exposure int.
         */
        void SendExposure(int exposure = 0);

        /**
         * @section Video setting commands
         * @brief Set_video_encoder_rate sets the drone video encoder rate
         * 
         */
        void SendVideoEncoderRate(int video_encoder_rate = 4);

        /**
         * @section Video setting commands
         * @brief Tell the drone whether to capture 960x720 4:3 video, or 1280x720 16:9 zoomed video. \n
         * 4:3 has a wider field of view (both vertically and horizontally), 16:9 is crisper. 
         * 
         * @param zoom bool zoom or not.
        */
        void SendVideoZoomMode(bool zoom);
        /**
         * @section Video setting commands
         * @brief Start streaming command.
         * 
         */
        void SendStartVideo();
        void SendTakeoffReq();

        /**
         * @brief Send connection request
         * With request to port 0x9617
         * 
         */
        void SendConnReq();

        /**
         * @brief LOG HEADER MSG is the biggest log message.
         * Drone will continue to send log_header connection requests with incremental ID.
         * Until sending back to drone 'ack_conn'.
         * 
         * @param id - same ID that drone sent to client. We must return *ack* with this ID ASAP,
         * in order to start LOGGING session.
         */
        void SendAckLog(const int id) override;

        /**
         * @brief Set the Socket object
         * 
         */
        void SetSocket(std::shared_ptr<ISender>);

        /**
         * @brief Send TimeCmd to the drone.
         * Send this time command upon connection.
         */
        void SendTimeCommand();

        TelloCommander(std::shared_ptr<spdlog::logger>, spdlog::level::level_enum lvl = spdlog::level::info);
        ~TelloCommander();

    private:
        std::shared_ptr<spdlog::logger> m_logger;
        std::shared_ptr<ISender> m_socket;
        MovementCommandsManager m_MovementCommandsManager;
        MovementsToPacketConverter m_MovementsToPacketConverter;
    };

} // namespace tello_protocol

#include "TelloCommander.hpp"
namespace tello_protocol
{
    void TelloCommander::ManualTakeoff()
    {
        m_logger->debug("Sending {}", std::string(__PRETTY_FUNCTION__));

        SetPitch(-1);
        SetRoll(-1);
        SetYaw(1);
        SetThrottle(-1);
        SetFastMode(false);
    }
    void TelloCommander::SetThrottle(float throttle)
    {
        m_MovementCommandsManager.SetAttitude(tello_protocol::AttitudeMovements::THROTTLE, throttle);
    }
    void TelloCommander::SetYaw(float yaw)
    {
        m_MovementCommandsManager.SetAttitude(tello_protocol::AttitudeMovements::YAW, yaw);
    }
    void TelloCommander::SetPitch(float pitch)
    {
        m_MovementCommandsManager.SetAttitude(tello_protocol::AttitudeMovements::PITCH, pitch);
    }
    void TelloCommander::SetRoll(float roll)
    {
        m_MovementCommandsManager.SetAttitude(tello_protocol::AttitudeMovements::ROLL, roll);
    }
    void TelloCommander::Flip(tello_protocol::FlipDirections direction)
    {

        // m_logger->info("Sending Flip {} command throw_and_go (cmd=0x{:x} seq=0x{:x})", tello_protocol::flip_direction_to_string(direction), direction, 0x48);
        auto pkt = tello_protocol::Packet(FLIP_CMD, 0x70);

        pkt.AddByte(direction);

        pkt.Fixup();
        m_socket->Send(pkt.GetBuffer());
    }

    bool TelloCommander::SetFastMode(bool fastMode)
    {
        return m_MovementCommandsManager.SetFastMode(fastMode);
    }

    void TelloCommander::Down(int amount)
    {
        assert(amount >= 0 && amount <= 100 && "Amount must be within limits.");
        if (!m_MovementCommandsManager.SetMovementCommand(tello_protocol::DOWN, amount))
        {
            m_logger->warn("Could not set " + std::string(__FUNCTION__) + "(" + std::to_string(amount) + ")");
        }
    }
    void TelloCommander::Up(int amount)
    {
        assert(amount >= 0 && amount <= 100 && "Amount must be within limits.");
        if (!m_MovementCommandsManager.SetMovementCommand(tello_protocol::UP, amount))
        {
            m_logger->warn("Could not set " + std::string(__FUNCTION__) + "(" + std::to_string(amount) + ")");
        }
    }
    void TelloCommander::CounterClockwise(int amount)
    {
        assert(amount >= 0 && amount <= 100 && "Amount must be within limits.");
        if (!m_MovementCommandsManager.SetMovementCommand(tello_protocol::COUNTER_CLOCKWISE, amount))
        {
            m_logger->warn("Could not set " + std::string(__FUNCTION__) + "(" + std::to_string(amount) + ")");
        }
    }
    void TelloCommander::Clockwise(int amount)
    {
        assert(amount >= 0 && amount <= 100 && "Amount must be within limits.");
        if (!m_MovementCommandsManager.SetMovementCommand(tello_protocol::CLOCKWISE, amount))
        {
            m_logger->warn("Could not set " + std::string(__FUNCTION__) + "(" + std::to_string(amount) + ")");
        }
    }
    void TelloCommander::Left(int amount)
    {
        assert(amount >= 0 && amount <= 100 && "Amount must be within limits.");
        if (!m_MovementCommandsManager.SetMovementCommand(tello_protocol::LEFT, amount))
        {
            m_logger->warn("Could not set " + std::string(__FUNCTION__) + "(" + std::to_string(amount) + ")");
        }
    }
    void TelloCommander::Right(int amount)
    {
        assert(amount >= 0 && amount <= 100 && "Amount must be within limits.");
        if (!m_MovementCommandsManager.SetMovementCommand(tello_protocol::RIGHT, amount))
        {
            m_logger->warn("Could not set " + std::string(__FUNCTION__) + "(" + std::to_string(amount) + ")");
        }
    }
    void TelloCommander::Backward(int amount)
    {
        assert(amount >= 0 && amount <= 100 && "Amount must be within limits.");
        if (!m_MovementCommandsManager.SetMovementCommand(tello_protocol::BACKWARD, amount))
        {
            m_logger->warn("Could not set " + std::string(__FUNCTION__) + "(" + std::to_string(amount) + ")");
        }
    }
    void TelloCommander::Forward(int amount)
    {
        assert(amount >= 0 && amount <= 100 && "Amount must be within limits.");
        if (!m_MovementCommandsManager.SetMovementCommand(tello_protocol::FORWARD, amount))
        {
            m_logger->warn("Could not set " + std::string(__FUNCTION__) + "(" + std::to_string(amount) + ")");
        }
    }

    bool TelloCommander::SendStickCommands()
    {
        auto pkt = tello_protocol::Packet(tello_protocol::STICK_CMD, 0x60);
        auto &mvmnts = m_MovementCommandsManager.GetStickMovements();
        if (!m_MovementsToPacketConverter.Convert(mvmnts, pkt))
        {
            m_logger->error("Could not convert stick movements! Something went wrong.");
            return false;
        }
        m_socket->Send(pkt.GetBuffer());
        return true;
    }

    void TelloCommander::ThrowAndGo()
    {
        m_logger->info("Sending throw_and_go (cmd=0x{:x} seq=0x{:x})", tello_protocol::THROW_AND_GO_CMD, 0x48);
        auto pkt = Packet(tello_protocol::THROW_AND_GO_CMD, 0x48);
        pkt.AddByte(0x00);
        pkt.AddTime();
        pkt.Fixup();
        m_socket->Send(pkt.GetBuffer());
    }
    void TelloCommander::PalmLand()
    {
        m_logger->info("Sending PalmLand to the drone (cmd=0x{:x})", tello_protocol::PALM_LAND_CMD);
        auto pkt = Packet(tello_protocol::PALM_LAND_CMD);
        pkt.AddByte(0x00);
        pkt.Fixup();
        m_socket->Send(pkt.GetBuffer());
    }

    void TelloCommander::SendExposure(int exposure)
    {
        assert(-1 < exposure && exposure < 3 && "Valid levels are 0, 1, and 2");

        auto pkt = tello_protocol::Packet(tello_protocol::EXPOSURE_CMD, 0x48);
        pkt.AddByte(0x00);
        pkt.Fixup();
        m_socket->Send(pkt.GetBuffer());
    }
    void TelloCommander::SendVideoEncoderRate(int video_encoder_rate)
    {
        auto pkt = tello_protocol::Packet(tello_protocol::VIDEO_ENCODER_RATE_CMD, 0x68);
        pkt.AddByte(video_encoder_rate);
        pkt.Fixup();
        m_socket->Send(pkt.GetBuffer());
    }
    void TelloCommander::SendVideoZoomMode(bool zoom)
    {
        auto pkt = tello_protocol::Packet(tello_protocol::VIDEO_MODE_CMD);
        pkt.AddByte(int(zoom));
        m_socket->Send(pkt.GetBuffer());
    }
    void TelloCommander::SendStartVideo()
    {
        auto pkt = tello_protocol::Packet(tello_protocol::VIDEO_START_CMD, 0x60);
        pkt.Fixup();
        m_socket->Send(pkt.GetBuffer());
    }

    void TelloCommander::SendTimeCommand()
    {
        m_logger->info("Sending TimeCmd to the drone (cmd=0x{:x} seq=0x{:x})", tello_protocol::TIME_CMD, 0x50);
        auto pkt = Packet(tello_protocol::TIME_CMD, 0x50);
        pkt.AddByte(0x00);
        pkt.AddTime();
        pkt.Fixup();
        m_socket->Send(pkt.GetBuffer());
    }
    void TelloCommander::GetLowBatThreshold()
    {
        m_logger->info("Get low battery threshold (cmd=0x{:x} seq=0x{:x})", tello_protocol::LOW_BAT_THRESHOLD_MSG, 0x01e4);
        auto pkt = Packet(tello_protocol::LOW_BAT_THRESHOLD_MSG);
        pkt.Fixup();
        m_socket->Send(pkt.GetBuffer());
    }
    void TelloCommander::SetLowBatThreshold(int threshold)
    {
        m_logger->info("Set low battery threshold={} (cmd=0x{:x} seq=0x{:x})", threshold, tello_protocol::LOW_BAT_THRESHOLD_CMD, 0x01e4);
        auto pkt = Packet(tello_protocol::LOW_BAT_THRESHOLD_CMD);
        pkt.AddByte(threshold);
        pkt.Fixup();
        m_socket->Send(pkt.GetBuffer());

        GetLowBatThreshold();
    }

    void TelloCommander::SetAttLimitReq(int limit)
    {
        assert(limit > 31 && "limit must be above 31.");

        m_logger->info("Set attitude limit={} (cmd=0x{:x} seq=0x{:x})", limit, tello_protocol::ATT_LIMIT_CMD, 0x01e4);

        unsigned char ch[4];
        std::memcpy(ch, &limit, sizeof(int));

        auto pkt = tello_protocol::Packet(tello_protocol::ATT_LIMIT_CMD);
        pkt.AddByte(0x00);
        pkt.AddByte(0x00);
        pkt.AddByte(ch[0]); /**<  'attitude limit' formatted in int */
        pkt.AddByte(0x41);
        pkt.Fixup();
        m_socket->Send(pkt.GetBuffer());

        GetAttLimitReq();
    }
    void TelloCommander::GetAttLimitReq()
    {
        m_logger->debug("Sending GetAttLimitReq request. (cmd=0x{:x} seq=0x{:x})", tello_protocol::ATT_LIMIT_MSG, 0x01e4);
        auto pkt = tello_protocol::Packet(tello_protocol::ATT_LIMIT_MSG);
        pkt.Fixup();
        m_socket->Send(pkt.GetBuffer());
    }

    void TelloCommander::SetAltLimitReq(int limit)
    {
        m_logger->debug("Set altitude limit={} (cmd=0x{:x} seq=0x{:x})", limit, tello_protocol::SET_ALT_LIMIT_CMD, 0x01e4);

        auto pkt = tello_protocol::Packet(tello_protocol::SET_ALT_LIMIT_CMD);
        pkt.AddByte(limit);
        pkt.AddByte(0x00);
        pkt.Fixup();
        m_socket->Send(pkt.GetBuffer());

        GetAltLimitReq();
    }
    void TelloCommander::GetAltLimitReq()
    {
        m_logger->debug("Sending GetAltitudeLimit request. (cmd=0x{:x} seq=0x{:x})", tello_protocol::ALT_LIMIT_MSG, 0x01e4);
        auto pkt = tello_protocol::Packet(tello_protocol::ALT_LIMIT_MSG);
        pkt.Fixup();
        m_socket->Send(pkt.GetBuffer());
    }

    void TelloCommander::SendTakeoffReq()
    {
        m_logger->debug("Sending takeoff");
        SetAltLimitReq(30);

        auto pkt = tello_protocol::Packet(tello_protocol::TAKEOFF_CMD);
        pkt.Fixup();
        m_socket->Send(pkt.GetBuffer());
    }
    void TelloCommander::SendLandReq()
    {
        m_logger->debug("Sending land");

        auto pkt = tello_protocol::Packet(tello_protocol::LAND_CMD);
        pkt.AddByte(0x00);
        pkt.Fixup();
        m_socket->Send(pkt.GetBuffer());
    }

    void TelloCommander::SendEmergencyCmd()
    {
        m_logger->info("Sending EMERGENCY_CMD to the drone");
        auto pkt = tello_protocol::Packet(tello_protocol::EMERGENCY_CMD);
        pkt.AddByte(0x00);
        pkt.Fixup();
        m_socket->Send(pkt.GetBuffer());
    }

    void TelloCommander::SendAckLog(const int id)
    {
        auto pkt = tello_protocol::Packet(tello_protocol::LOG_HEADER_MSG, 0x50);
        pkt.AddByte(0x00);
        Byte byte = le16(id);
        pkt.AddByte(byte.LeftNibble);
        pkt.AddByte(byte.RightNibble);
        pkt.Fixup();
        m_logger->info("Sending conn_ack msg: {}", spdlog::to_hex(pkt.GetBuffer()));
        m_socket->Send(pkt.GetBuffer());
    }

    void TelloCommander::SendConnReq()
    {
        auto conn_req = tello_protocol::Packet("conn_req:\x96\x17");
        m_socket->Send(conn_req.GetBuffer());
    }

    void TelloCommander::SetSocket(std::shared_ptr<ISender> socket)
    {
        m_socket = socket;
    }
    TelloCommander::TelloCommander(std::shared_ptr<spdlog::logger> logger, spdlog::level::level_enum lvl)
        : m_logger(logger)
    {
        m_logger->info(m_logger->name() + " Initiated.");
        m_logger->set_level(lvl);
    }
    TelloCommander::~TelloCommander()
    {
        m_logger->info(m_logger->name() + " Destructing.");
    }
} // namespace tello_protocol

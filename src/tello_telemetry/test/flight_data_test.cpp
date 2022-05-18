#include "flight_data_test.hpp"
#include "helper.hpp"

using namespace std::chrono_literals;
TEST(TelloCommandTest, SendConnReq)
{
    using asio::ip::udp;

    asio::io_service io_service_; // Manages IO for this socket
    unsigned short port = 9000;
    /*
    Packets from the drone suppose to arrive at this (address,port): ('',9000)
    Packets are being send from client to drone to this (address,port) = ('192.168.10.1', 8889)
    */
    unsigned short drone_port = 8889;
    std::string drone_ip = "192.168.10.1";
    udp::endpoint remote_endpoint_(asio::ip::address_v4::from_string(drone_ip), drone_port);

    udp::socket tello_socket(io_service_, udp::endpoint(udp::v4(), port)); // The socket

    //Run
    auto pkt = tello_protocol::Packet("conn_req:\x96\x17");
    std::cout << pkt << std::endl;
    tello_socket.send_to(asio::buffer(pkt.GetBuffer()), remote_endpoint_);

    // Test
    using namespace std::chrono_literals;
    bool keep_sending = true;
    auto buffer_ = std::vector<unsigned char>(1024);
    // It is known that conn_req answer is in size 11 bytes
    size_t r = 0;

    tello_socket.send_to(asio::buffer(pkt.GetBuffer()), remote_endpoint_);
    std::this_thread::sleep_for(0.5s);

    while (keep_sending)
    {
        r = tello_socket.receive(asio::buffer(buffer_));
        if (r == 11)
            keep_sending = false;
    }

    // "conn_ack:\x96\x17" (Sniffed using wireshark)
    std::vector<unsigned char> conn_ack_from_drone = {0x63, 0x6f, 0x6e, 0x6e, 0x5f, 0x61, 0x63, 0x6b, 0x3a, 0x96, 0x17};

    int index = 0;
    for (auto &letter : conn_ack_from_drone)
    {
        ASSERT_EQ(buffer_[index], letter);
        index++;
    }
    TearDownTestCase();
}
TEST(TelloTelemetryTest, ReceiveLogHeaderOnce)
{
    /* 
    LOG_DATA session with the Tello drone is triggered with '"conn_req:\x96\x17"' request.
    Once this request has been sent, the client will receive "ack_req:\x96\x17".
    And a LOG_DATA session will start.

    CMD will be 'LOG_HEADER_MSG' only once, at the time of receiveing the ACK.
    If cmd is 'LOG_DATA_MSG', it means that a LOG_DATA session has been established. So this test is over.
    */

    // Setup
    using asio::ip::udp;

    // Input socket
    asio::io_service io_service_; // Manages IO for this socket
    unsigned short port = 9000;
    udp::socket tello_socket(io_service_, udp::endpoint(udp::v4(), port)); // The socket

    // Output socket
    unsigned short drone_port = 8889;
    std::string drone_ip = "192.168.10.1";
    udp::endpoint remote_endpoint_(asio::ip::address_v4::from_string(drone_ip), drone_port);

    using namespace std::chrono_literals;
    bool keep_receiving = true, is_log_header_received = false;
    int received_counter;
    auto buffer_ = std::vector<unsigned char>(1024);
    size_t r = 0;
    tello_protocol::TelloTelemetry telloTelemetry(spdlog::stdout_color_mt("telemetry"));
    //Run

    // Connect
    auto conn_req = tello_protocol::Packet("conn_req:\x96\x17");
    tello_socket.send_to(asio::buffer(conn_req.GetBuffer()), remote_endpoint_);
    bool connected_successfully = false;

    while (!connected_successfully)
    {
        r = tello_socket.receive(asio::buffer(buffer_));

        std::vector<unsigned char> data(buffer_.begin(), buffer_.begin() + r);
        auto received = tello_protocol::Packet(data);
        if (received.GetBuffer().find("conn_ack") == 0)
        {
            connected_successfully = true;
        }
    }

    auto log_header_logger = spdlog::stdout_color_mt("log_header_message");

    auto send_ack_log = [&tello_socket, &remote_endpoint_, &log_header_logger](const uint16_t id) {
        auto pkt = tello_protocol::Packet(tello_protocol::LOG_HEADER_MSG, 0x50);
        pkt.AddByte(0x00);
        Byte byte = le16(id);
        pkt.AddByte(byte.LeftNibble);
        pkt.AddByte(byte.RightNibble);
        pkt.Fixup();
        log_header_logger->info("Sending conn_ack msg: {}", spdlog::to_hex(pkt.GetBuffer()));
        tello_socket.send_to(asio::buffer(pkt.GetBuffer()), remote_endpoint_);
    };

    while (keep_receiving && !is_log_header_received)
    {
        r = tello_socket.receive(asio::buffer(buffer_));

        std::vector<unsigned char> data(buffer_.begin(), buffer_.begin() + r);
        auto received = tello_protocol::Packet(data);

        auto cmd = uint16(received.GetBuffer()[5], received.GetBuffer()[6]);
        if (cmd == tello_protocol::LOG_HEADER_MSG)
        {
            // LOG HEADER MSG is the biggest log message.
            // Drone will continue to send log_header connection requests with incremental ID.
            // Until sending back to drone 'ack_conn'.
            auto id = uint16(data[9], data[10]);

            // telloTelemetry.SetBuildDate(received.GetBuffer().substr(28, 26));
            // DJI LOG VERSION something like this: DJI_LOG_V3I��Rc
            // telloTelemetry.SetDJILogVersion(received.GetBuffer().substr(245, 6));

            // After sending back ack. the drone will not sent LOG_HEADER_MSG anymore.
            send_ack_log(id);
            is_log_header_received = true;
        }
        else if (cmd == tello_protocol::LOG_DATA_MSG)
        {
            /* 
            Is cmd  is LOG_DATA_MSG.
            It means that a LOG_DATA connections has been already assured. 
            So this test is pseudo-passed.
            */
            is_log_header_received = true;
            keep_receiving = false;
        }

        std::fill(buffer_.begin(), buffer_.end(), 0);
    }
    ASSERT_TRUE(is_log_header_received);

    TearDownTestCase();
}
TEST(TelloTelemetryTest, ReceiveLogDataMvoMsg)
{

    // Setup
    using asio::ip::udp;

    // Input socket
    asio::io_service io_service_; // Manages IO for this socket
    unsigned short port = 9000;
    udp::socket tello_socket(io_service_, udp::endpoint(udp::v4(), port)); // The socket

    // Output socket
    unsigned short drone_port = 8889;
    std::string drone_ip = "192.168.10.1";
    udp::endpoint remote_endpoint_(asio::ip::address_v4::from_string(drone_ip), drone_port);

    using namespace std::chrono_literals;
    bool keep_receiving = true, is_log_header_received = false;
    int log_data_msg_counter = 0;
    auto buffer_ = std::vector<unsigned char>(1024);
    size_t r = 0;
    // Create TelloTelemetry instance.
    tello_protocol::TelloTelemetry telloTelemetry(spdlog::stdout_color_mt("telemetry"));
    auto logdata = std::make_shared<tello_protocol::LogData>(spdlog::stdout_color_mt("logdata"));
    // telloTelemetry.SetLogData(logdata);

    //Run

    // Connect
    auto conn_req = tello_protocol::Packet("conn_req:\x96\x17");
    tello_socket.send_to(asio::buffer(conn_req.GetBuffer()), remote_endpoint_);
    bool connected_successfully = false;

    while (!connected_successfully)
    {
        r = tello_socket.receive(asio::buffer(buffer_));

        std::vector<unsigned char> data(buffer_.begin(), buffer_.begin() + r);
        auto received = tello_protocol::Packet(data);
        if (received.GetBuffer().find("conn_ack") == 0)
        {
            connected_successfully = true;
        }
    }

    auto log_msg_logger = spdlog::stdout_color_mt("log_msg_logger");

    auto send_ack = [&tello_socket, &remote_endpoint_, &log_msg_logger](const uint16_t id) {
        auto pkt = tello_protocol::Packet(tello_protocol::LOG_HEADER_MSG, 0x50);
        pkt.AddByte(0x00);
        Byte byte = le16(id);
        pkt.AddByte(byte.LeftNibble);
        pkt.AddByte(byte.RightNibble);
        pkt.Fixup();

        tello_socket.send_to(asio::buffer(pkt.GetBuffer()), remote_endpoint_);
    };

    while (keep_receiving && log_data_msg_counter < 100)
    {
        r = tello_socket.receive(asio::buffer(buffer_));

        std::vector<unsigned char> data(buffer_.begin(), buffer_.begin() + r);
        auto received = tello_protocol::Packet(data);

        auto cmd = uint16(received.GetBuffer()[5], received.GetBuffer()[6]);
        if (cmd == tello_protocol::LOG_HEADER_MSG)
        {
            /* 
            This part is covered in previous 'ReceiveLogHeaderOnce' test case.
            It is necessary if drone hasn't started LOG_DATA session.
            */
            auto id = uint16(data[9], data[10]);
            send_ack(id);
        }

        else if (cmd == tello_protocol::LOG_DATA_MSG)
        {

            std::vector<unsigned char> trimmed(data.begin() + 10, data.end());
            // telloTelemetry.GetLogData()->Update(trimmed);
            log_data_msg_counter++;
        }

        std::fill(buffer_.begin(), buffer_.end(), 0);
    }

    // ASSERT_GT(telloTelemetry.GetLogData()->GetLogMvo().GetUpdateCounter(), 0);
    TearDownTestCase();
}
TEST(TelloTelemetryTest, ReceiveLogDataImuMsg)
{

    // Setup
    using asio::ip::udp;

    // Input socket
    asio::io_service io_service_; // Manages IO for this socket
    unsigned short port = 9000;
    udp::socket tello_socket(io_service_, udp::endpoint(udp::v4(), port)); // The socket

    // Output socket
    unsigned short drone_port = 8889;
    std::string drone_ip = "192.168.10.1";
    udp::endpoint remote_endpoint_(asio::ip::address_v4::from_string(drone_ip), drone_port);

    using namespace std::chrono_literals;
    bool keep_receiving = true, is_log_header_received = false;
    int log_data_msg_counter = 0;
    auto buffer_ = std::vector<unsigned char>(1024);

    // Create TelloTelemetry instance.
    tello_protocol::TelloTelemetry telloTelemetry(spdlog::stdout_color_mt("telemetry"));
    auto logdata = std::make_shared<tello_protocol::LogData>(spdlog::stdout_color_mt("logdata"));
    // telloTelemetry.SetLogData(logdata);

    //Run

    // Connect
    auto pkt = tello_protocol::Packet("conn_req:\x96\x17");
    tello_socket.send_to(asio::buffer(pkt.GetBuffer()), remote_endpoint_);

    size_t r = 0;
    auto log_msg_logger = spdlog::stdout_color_mt("log_msg_logger");

    auto send_ack = [&tello_socket, &remote_endpoint_, &log_msg_logger](const uint16_t id) {
        auto pkt = tello_protocol::Packet(tello_protocol::LOG_HEADER_MSG, 0x50);
        pkt.AddByte(0x00);
        Byte byte = le16(id);
        pkt.AddByte(byte.LeftNibble);
        pkt.AddByte(byte.RightNibble);
        pkt.Fixup();

        tello_socket.send_to(asio::buffer(pkt.GetBuffer()), remote_endpoint_);
    };

    while (keep_receiving && log_data_msg_counter < 100)
    {
        r = tello_socket.receive(asio::buffer(buffer_));

        std::vector<unsigned char> data(buffer_.begin(), buffer_.begin() + r);
        auto received = tello_protocol::Packet(data);

        auto cmd = uint16(received.GetBuffer()[5], received.GetBuffer()[6]);
        if (cmd == tello_protocol::LOG_HEADER_MSG)
        {
            /* 
            This part is covered in previous 'ReceiveLogHeaderOnce' test case.
            It is necessary if drone hasn't started LOG_DATA session.
            */
            auto id = uint16(data[9], data[10]);
            send_ack(id);
        }

        else if (cmd == tello_protocol::LOG_DATA_MSG)
        {
            std::vector<unsigned char> trimmed(data.begin() + 10, data.end());
            // telloTelemetry.GetLogData()->Update(trimmed);
            log_data_msg_counter++;
        }

        std::fill(buffer_.begin(), buffer_.end(), 0);
    }

    // ASSERT_GT(telloTelemetry.GetLogData()->GetLogImuAtti().GetUpdateCounter(), 0);
    TearDownTestCase();
}
TEST(TelloTelemetryTest, ReceiveFlightData)
{
    /* 
    Test if FlightData being received and parsed correctly.
    Test pass if FlightMode == 6;
     */

    // Setup
    using asio::ip::udp;
    asio::io_service io_service_; // Manages IO for this socket
    unsigned short port = 9000;
    unsigned short drone_port = 8889;
    std::string drone_ip = "192.168.10.1";
    udp::endpoint remote_endpoint_(asio::ip::address_v4::from_string(drone_ip), drone_port);
    udp::socket tello_socket(io_service_, udp::endpoint(udp::v4(), port)); // The socket
    using namespace std::chrono_literals;
    bool keep_receiving = true;
    auto buffer_ = std::vector<unsigned char>(1024);
    //Run

    // Connect
    auto pkt = tello_protocol::Packet("conn_req:\x96\x17");
    tello_socket.send_to(asio::buffer(pkt.GetBuffer()), remote_endpoint_);

    size_t r = 0;
    tello_socket.send_to(asio::buffer(pkt.GetBuffer()), remote_endpoint_);
    std::this_thread::sleep_for(0.5s);

    auto flight_data = tello_protocol::FlightData(spdlog::stdout_color_mt("flight_data"));
    while (keep_receiving)
    {
        r = tello_socket.receive(asio::buffer(buffer_));
        std::cout << "r: " << r << '\n';

        // Create function
        std::vector<unsigned char> data(buffer_.begin(), buffer_.begin() + r); // Strip last 2 bytes. they are CRC16

        auto received = tello_protocol::Packet(data);
        auto cmd = uint16(received.GetBuffer()[5], received.GetBuffer()[6]);

        if (cmd == tello_protocol::FLIGHT_MSG)
        {

            // if (flight_data.SetData(received.GetData()))
            // {
            //     ASSERT_GE(flight_data.GetFlightMode(), 0);
            //     keep_receiving = false;
            // }
        }
        std::fill(buffer_.begin(), buffer_.end(), 0);
        std::this_thread::sleep_for(0.5s);
    }
    TearDownTestCase();
}
TEST(TelloTelemetryTest, ReceiveWifiStrength)
{

    // Setup
    using asio::ip::udp;
    asio::io_service io_service_; // Manages IO for this socket
    unsigned short port = 9000;
    unsigned short drone_port = 8889;
    std::string drone_ip = "192.168.10.1";
    udp::endpoint remote_endpoint_(asio::ip::address_v4::from_string(drone_ip), drone_port);
    udp::socket tello_socket(io_service_, udp::endpoint(udp::v4(), port)); // The socket
    using namespace std::chrono_literals;
    bool keep_receiving = true;
    auto buffer_ = std::vector<unsigned char>(1024);
    //Run

    // Connect
    auto pkt = tello_protocol::Packet("conn_req:\x96\x17");
    tello_socket.send_to(asio::buffer(pkt.GetBuffer()), remote_endpoint_);

    size_t r = 0;
    tello_socket.send_to(asio::buffer(pkt.GetBuffer()), remote_endpoint_);
    std::this_thread::sleep_for(0.5s);

    tello_protocol::TelloTelemetry tello_telemetry(spdlog::stdout_color_mt("tello_telemetry"));
    auto flight_data = std::make_shared<tello_protocol::FlightData>(spdlog::stdout_color_mt("flight_data"));
    // tello_telemetry.SetFlightData(flight_data);

    while (keep_receiving)
    {
        r = tello_socket.receive(asio::buffer(buffer_));

        std::vector<unsigned char> data(buffer_.begin(), buffer_.begin() + r); // Strip last 2 bytes. they are CRC16

        auto received = tello_protocol::Packet(data);
        auto cmd = uint16(received.GetBuffer()[5], received.GetBuffer()[6]);
        if (cmd == tello_protocol::WIFI_MSG)
        {
            // tello_telemetry.GetFlightData()->SetWifiStrength(data[9]);
            // std::cout << "WifiStrength: " << tello_telemetry.GetFlightData()->GetWifiStrength() << '\n';
            // ASSERT_GE(tello_telemetry.GetFlightData()->GetWifiStrength(), 0);
            keep_receiving = false;
        }
        std::fill(buffer_.begin(), buffer_.end(), 0);
    }
    TearDownTestCase();
}

TEST(TelloTelemetryTest, GET_ALT_LIMIT_MSG)
{

    // Setup
    using asio::ip::udp;
    asio::io_service io_service_; // Manages IO for this socket
    unsigned short port = 9000;
    unsigned short drone_port = 8889;
    std::string drone_ip = "192.168.10.1";
    udp::endpoint remote_endpoint_(asio::ip::address_v4::from_string(drone_ip), drone_port);
    udp::socket tello_socket(io_service_, udp::endpoint(udp::v4(), port)); // The socket
    using namespace std::chrono_literals;
    bool keep_receiving = true;
    auto buffer_ = std::vector<unsigned char>(1024);
    //Run

    // Connect
    auto pkt = tello_protocol::Packet("conn_req:\x96\x17");
    tello_socket.send_to(asio::buffer(pkt.GetBuffer()), remote_endpoint_);

    size_t r = 0;
    tello_socket.send_to(asio::buffer(pkt.GetBuffer()), remote_endpoint_);
    std::this_thread::sleep_for(0.5s);

    auto test_logger = spdlog::stdout_color_mt("test_logger");

    auto send_pkt = [&tello_socket, &remote_endpoint_, &test_logger](const tello_protocol::Packet &pkt) {
        tello_socket.send_to(asio::buffer(pkt.GetBuffer()), remote_endpoint_);
    };

    auto get_alt_limit = [&send_pkt, &test_logger]() {
        test_logger->info("get altitude (cmd=0x{:x} seq=0x{:x})", tello_protocol::ALT_LIMIT_MSG, 0x01e4);
        auto pkt = tello_protocol::Packet(tello_protocol::ALT_LIMIT_MSG);
        pkt.Fixup();
        send_pkt(pkt);
    };

    auto flight_data = tello_protocol::FlightData(spdlog::stdout_color_mt("flight_data"));

    while (keep_receiving)
    {
        get_alt_limit();
        r = tello_socket.receive(asio::buffer(buffer_));

        std::vector<unsigned char> data(buffer_.begin(), buffer_.begin() + r); // Strip last 2 bytes. they are CRC16

        auto received = tello_protocol::Packet(data);
        auto cmd = uint16(received.GetBuffer()[5], received.GetBuffer()[6]);

        if (cmd == tello_protocol::ALT_LIMIT_MSG)
        {
            std::cout << received.GetData();
            // flight_data.SetAltLimit(received.GetData());
            // ASSERT_GE(flight_data.GetAltLimit(), 0) << "Default value is 10[m]";
            keep_receiving = false;
        }
        std::fill(buffer_.begin(), buffer_.end(), 0);
        std::this_thread::sleep_for(0.05s);
    }
    TearDownTestCase();
}

TEST(TelloTelemetryTest, SET_ALT_LIMIT_MSG)
{

    // Setup
    using asio::ip::udp;
    asio::io_service io_service_; // Manages IO for this socket
    unsigned short port = 9000;
    unsigned short drone_port = 8889;
    std::string drone_ip = "192.168.10.1";
    udp::endpoint remote_endpoint_(asio::ip::address_v4::from_string(drone_ip), drone_port);
    udp::socket tello_socket(io_service_, udp::endpoint(udp::v4(), port)); // The socket
    using namespace std::chrono_literals;
    bool keep_receiving = true;
    auto buffer_ = std::vector<unsigned char>(1024);
    //Run

    // Connect
    auto pkt = tello_protocol::Packet("conn_req:\x96\x17");
    tello_socket.send_to(asio::buffer(pkt.GetBuffer()), remote_endpoint_);

    size_t r = 0;
    tello_socket.send_to(asio::buffer(pkt.GetBuffer()), remote_endpoint_);
    std::this_thread::sleep_for(0.5s);

    auto test_logger = spdlog::stdout_color_mt("test_logger");

    auto send_pkt = [&tello_socket, &remote_endpoint_, &test_logger](const tello_protocol::Packet &pkt) {
        tello_socket.send_to(asio::buffer(pkt.GetBuffer()), remote_endpoint_);
    };

    auto set_alt_limit = [&send_pkt, &test_logger](int limit) {
        test_logger->info("set altitude limit={} (cmd=0x{:x} seq=0x{:x})", limit, tello_protocol::SET_ALT_LIMIT_CMD, 0x01e4);
        auto pkt = tello_protocol::Packet(tello_protocol::SET_ALT_LIMIT_CMD);
        pkt.AddByte(limit);
        pkt.AddByte(0x00);
        pkt.Fixup();
        send_pkt(pkt);
    };

    auto get_alt_limit = [&send_pkt, &test_logger]() {
        test_logger->info("get altitude (cmd=0x{:x} seq=0x{:x})", tello_protocol::ALT_LIMIT_MSG, 0x01e4);
        auto pkt = tello_protocol::Packet(tello_protocol::ALT_LIMIT_MSG);
        pkt.Fixup();
        send_pkt(pkt);
    };

    tello_protocol::TelloTelemetry tello_telemetry(spdlog::stdout_color_mt("tello_telemetry"));
    auto flight_data = std::make_shared<tello_protocol::FlightData>(spdlog::stdout_color_mt("flight_data"));
    // tello_telemetry.SetFlightData(flight_data);

    // Test
    int limit = 50;
    set_alt_limit(limit);
    while (keep_receiving)
    {
        // set_alt_limit(10);
        get_alt_limit();
        r = tello_socket.receive(asio::buffer(buffer_));

        std::vector<unsigned char> data(buffer_.begin(), buffer_.begin() + r); // Strip last 2 bytes. they are CRC16

        auto received = tello_protocol::Packet(data);
        auto cmd = uint16(received.GetBuffer()[5], received.GetBuffer()[6]);

        if (cmd == tello_protocol::ALT_LIMIT_MSG)
        {
            std::cout << received.GetData();
            // tello_telemetry.GetFlightData()->SetAltLimit(received.GetData());
            // ASSERT_EQ(tello_telemetry.GetFlightData()->GetAltLimit(), limit);
            keep_receiving = false;
        }
        std::fill(buffer_.begin(), buffer_.end(), 0);
        std::this_thread::sleep_for(0.05s);
    }
    TearDownTestCase();
}

TEST(TelloTelemetryTest, GET_ATT_LIMIT_MSG)
{

    // Setup
    using asio::ip::udp;
    asio::io_service io_service_; // Manages IO for this socket
    unsigned short port = 9000;
    unsigned short drone_port = 8889;
    std::string drone_ip = "192.168.10.1";
    udp::endpoint remote_endpoint_(asio::ip::address_v4::from_string(drone_ip), drone_port);
    udp::socket tello_socket(io_service_, udp::endpoint(udp::v4(), port)); // The socket
    using namespace std::chrono_literals;
    bool keep_receiving = true;
    auto buffer_ = std::vector<unsigned char>(1024);
    //Run

    // Connect
    auto pkt = tello_protocol::Packet("conn_req:\x96\x17");
    tello_socket.send_to(asio::buffer(pkt.GetBuffer()), remote_endpoint_);

    size_t r = 0;
    tello_socket.send_to(asio::buffer(pkt.GetBuffer()), remote_endpoint_);
    std::this_thread::sleep_for(0.5s);

    auto test_logger = spdlog::stdout_color_mt("test_logger");

    auto send_pkt = [&tello_socket, &remote_endpoint_, &test_logger](const tello_protocol::Packet &pkt) {
        tello_socket.send_to(asio::buffer(pkt.GetBuffer()), remote_endpoint_);
    };

    auto get_att_limit = [&send_pkt, &test_logger]() {
        test_logger->info("get attitude (cmd=0x{:x} seq=0x{:x})", tello_protocol::ATT_LIMIT_MSG, 0x01e4);
        auto pkt = tello_protocol::Packet(tello_protocol::ATT_LIMIT_MSG);
        pkt.Fixup();
        send_pkt(pkt);
    };

    tello_protocol::TelloTelemetry tello_telemetry(spdlog::stdout_color_mt("tello_telemetry"));
    auto flight_data = std::make_shared<tello_protocol::FlightData>(spdlog::stdout_color_mt("flight_data"));
    // tello_telemetry.SetFlightData(flight_data);

    while (keep_receiving)
    {
        get_att_limit();
        r = tello_socket.receive(asio::buffer(buffer_));
        std::vector<unsigned char> data(buffer_.begin(), buffer_.begin() + r); // Strip last 2 bytes. they are CRC16
        auto received = tello_protocol::Packet(data);

        auto cmd = uint16(received.GetBuffer()[5], received.GetBuffer()[6]);
        if (cmd == tello_protocol::ATT_LIMIT_MSG)
        {
            // tello_telemetry.GetFlightData()->SetAttLimit(received.GetData());
            // ASSERT_GE(tello_telemetry.GetFlightData()->GetAttLimit(), 0) << "Default value is 10[deg]";
            keep_receiving = false;
        }
        std::fill(buffer_.begin(), buffer_.end(), 0);
        std::this_thread::sleep_for(0.05s);
    }
    TearDownTestCase();
}

TEST(TelloTelemetryTest, SET_ATT_LIMIT_MSG)
{

    // Setup
    using asio::ip::udp;
    asio::io_service io_service_; // Manages IO for this socket
    unsigned short port = 9000;
    unsigned short drone_port = 8889;
    std::string drone_ip = "192.168.10.1";
    udp::endpoint remote_endpoint_(asio::ip::address_v4::from_string(drone_ip), drone_port); // Socket for sending CMD to drone
    udp::socket tello_socket(io_service_, udp::endpoint(udp::v4(), port));                   // The socket for receiving data
    using namespace std::chrono_literals;
    bool keep_receiving = true;
    auto buffer_ = std::vector<unsigned char>(1024);
    //Run

    // Connect
    auto pkt = tello_protocol::Packet("conn_req:\x96\x17");
    tello_socket.send_to(asio::buffer(pkt.GetBuffer()), remote_endpoint_);

    size_t r = 0;
    tello_socket.send_to(asio::buffer(pkt.GetBuffer()), remote_endpoint_);
    std::this_thread::sleep_for(0.5s);

    auto test_logger = spdlog::stdout_color_mt("test_logger");

    auto send_pkt = [&tello_socket, &remote_endpoint_, &test_logger](const tello_protocol::Packet &pkt) {
        tello_socket.send_to(asio::buffer(pkt.GetBuffer()), remote_endpoint_);
    };

    auto set_att_limit = [&send_pkt, &test_logger](float limit) {
        /* 
            Info: set attitude limit=10 (cmd=0x1058 seq=0x01e4)
            limit 10 == 0x41200000
            bytearray(b'\xcc\x00\x00\x00hX\x10\x00\x00\x00\x00 A')
            b'\x00\x00\x00 A\xff\x8b'
            '00 00 00 20 41'

            GetAtt
            b'\xcc\x80\x00R\xb0Y\x10\x00\x00\x00\x00\x00 A\xff\x8b'
            data[9:] = b'\x00\x00\x00 A\xff\x8b'
        */
        test_logger->info("set attitude limit={} (cmd=0x{:x} seq=0x{:x})", limit, tello_protocol::ATT_LIMIT_CMD, 0x01e4);
        auto pkt = tello_protocol::Packet(tello_protocol::ATT_LIMIT_CMD);

        // // Float to hex
        // unsigned char ch[4];
        // memcpy(ch, &limit, sizeof(float));
        // test_logger->info("{:x}{:x}{:x}{:x}", ch[3], ch[2], ch[1], ch[0]);

        // pkt.AddByte(ch[0]);
        // pkt.AddByte(ch[1]);
        // pkt.AddByte(ch[2]);
        // pkt.AddByte(ch[3]);
        pkt.AddFloat(limit);
        test_logger->info("{}", spdlog::to_hex(pkt.GetBuffer()));
        pkt.Fixup();
        send_pkt(pkt);
    };

    auto get_att_limit = [&send_pkt, &test_logger]() {
        test_logger->info("get attitude (cmd=0x{:x} seq=0x{:x})", tello_protocol::ATT_LIMIT_MSG, 0x01e4);
        auto pkt = tello_protocol::Packet(tello_protocol::ATT_LIMIT_MSG);
        pkt.Fixup();
        send_pkt(pkt);
    };

    tello_protocol::TelloTelemetry tello_telemetry(spdlog::stdout_color_mt("tello_telemetry"));
    auto flight_data = std::make_shared<tello_protocol::FlightData>(spdlog::stdout_color_mt("flight_data"));
    // tello_telemetry.SetFlightData(flight_data);

    // Test
    float limit = 15.6;
    set_att_limit(limit);
    while (keep_receiving)
    {
        get_att_limit();
        r = tello_socket.receive(asio::buffer(buffer_));

        std::vector<unsigned char> data(buffer_.begin(), buffer_.begin() + r); // Strip last 2 bytes. they are CRC16

        auto received = tello_protocol::Packet(data);
        auto cmd = uint16(received.GetBuffer()[5], received.GetBuffer()[6]);

        if (cmd == tello_protocol::ATT_LIMIT_MSG)
        {
            std::cout << received.GetData();
            // tello_telemetry.GetFlightData()->SetAttLimit(received.GetData());
            // ASSERT_EQ(tello_telemetry.GetFlightData()->GetAttLimit(), limit);
            keep_receiving = false;
        }
        std::fill(buffer_.begin(), buffer_.end(), 0);
        std::this_thread::sleep_for(0.05s);
    }
    TearDownTestCase();
}

TEST(TelloTelemetryTest, GET_LOW_BAT_THRESHOLD)
{

    // Setup
    using asio::ip::udp;
    asio::io_service io_service_; // Manages IO for this socket
    unsigned short port = 9000;
    unsigned short drone_port = 8889;
    std::string drone_ip = "192.168.10.1";
    udp::endpoint remote_endpoint_(asio::ip::address_v4::from_string(drone_ip), drone_port); // Socket for sending CMD to drone
    udp::socket tello_socket(io_service_, udp::endpoint(udp::v4(), port));                   // The socket for receiving data
    using namespace std::chrono_literals;
    bool keep_receiving = true;
    auto buffer_ = std::vector<unsigned char>(1024);
    //Run

    // Connect
    auto pkt = tello_protocol::Packet("conn_req:\x96\x17");
    tello_socket.send_to(asio::buffer(pkt.GetBuffer()), remote_endpoint_);

    size_t r = 0;
    tello_socket.send_to(asio::buffer(pkt.GetBuffer()), remote_endpoint_);
    std::this_thread::sleep_for(0.5s);

    auto test_logger = spdlog::stdout_color_mt("test_logger");

    auto send_pkt = [&tello_socket, &remote_endpoint_, &test_logger](const tello_protocol::Packet &pkt) {
        tello_socket.send_to(asio::buffer(pkt.GetBuffer()), remote_endpoint_);
    };

    auto get_low_bat_threshold = [&send_pkt, &test_logger]() {
        test_logger->info("get attitude (cmd=0x{:x} seq=0x{:x})", tello_protocol::LOW_BAT_THRESHOLD_MSG, 0x01e4);
        auto pkt = tello_protocol::Packet(tello_protocol::LOW_BAT_THRESHOLD_MSG);
        pkt.Fixup();
        send_pkt(pkt);
    };

    tello_protocol::TelloTelemetry tello_telemetry(spdlog::stdout_color_mt("tello_telemetry"));
    auto flight_data = std::make_shared<tello_protocol::FlightData>(spdlog::stdout_color_mt("flight_data"));
    // tello_telemetry.SetFlightData(flight_data);

    // Test
    while (keep_receiving)
    {
        get_low_bat_threshold();
        r = tello_socket.receive(asio::buffer(buffer_));

        std::vector<unsigned char> data(buffer_.begin(), buffer_.begin() + r); // Strip last 2 bytes. they are CRC16

        auto received = tello_protocol::Packet(data);
        auto cmd = uint16(received.GetBuffer()[5], received.GetBuffer()[6]);

        if (cmd == tello_protocol::LOW_BAT_THRESHOLD_MSG)
        {
            std::cout << received.GetData();
            // tello_telemetry.GetFlightData()->SetLowBatThreshold(received.GetData());
            // ASSERT_GE(tello_telemetry.GetFlightData()->GetLowBatThreshold(), 0);
            keep_receiving = false;
        }
        std::fill(buffer_.begin(), buffer_.end(), 0);
        std::this_thread::sleep_for(0.05s);
    }
    TearDownTestCase();
}

TEST(TelloTelemetryTest, SET_LOW_BAT_THRESHOLD)
{

    // Setup
    using asio::ip::udp;
    asio::io_service io_service_; // Manages IO for this socket
    unsigned short port = 9000;
    unsigned short drone_port = 8889;
    std::string drone_ip = "192.168.10.1";
    udp::endpoint remote_endpoint_(asio::ip::address_v4::from_string(drone_ip), drone_port); // Socket for sending CMD to drone
    udp::socket tello_socket(io_service_, udp::endpoint(udp::v4(), port));                   // The socket for receiving data
    using namespace std::chrono_literals;
    bool keep_receiving = true;
    auto buffer_ = std::vector<unsigned char>(1024);
    //Run

    // Connect
    auto pkt = tello_protocol::Packet("conn_req:\x96\x17");
    tello_socket.send_to(asio::buffer(pkt.GetBuffer()), remote_endpoint_);

    size_t r = 0;
    tello_socket.send_to(asio::buffer(pkt.GetBuffer()), remote_endpoint_);
    std::this_thread::sleep_for(0.5s);

    auto test_logger = spdlog::stdout_color_mt("test_logger");

    auto send_pkt = [&tello_socket, &remote_endpoint_, &test_logger](const tello_protocol::Packet &pkt) {
        tello_socket.send_to(asio::buffer(pkt.GetBuffer()), remote_endpoint_);
    };

    auto set_low_bat_threshold = [&send_pkt, &test_logger](int limit) {
        test_logger->info("set attitude limit={} (cmd=0x{:x} seq=0x{:x})", limit, tello_protocol::LOW_BAT_THRESHOLD_CMD, 0x01e4);
        auto pkt = tello_protocol::Packet(tello_protocol::LOW_BAT_THRESHOLD_CMD);

        pkt.AddInt(limit);
        test_logger->info("{}", spdlog::to_hex(pkt.GetBuffer()));
        pkt.Fixup();
        send_pkt(pkt);
    };

    auto get_low_bat_threshold = [&send_pkt, &test_logger]() {
        test_logger->info("get attitude (cmd=0x{:x} seq=0x{:x})", tello_protocol::LOW_BAT_THRESHOLD_MSG, 0x01e4);
        auto pkt = tello_protocol::Packet(tello_protocol::LOW_BAT_THRESHOLD_MSG);
        pkt.Fixup();
        send_pkt(pkt);
    };

    tello_protocol::TelloTelemetry tello_telemetry(spdlog::stdout_color_mt("tello_telemetry"), spdlog::level::debug);
    auto flight_data = std::make_shared<tello_protocol::FlightData>(spdlog::stdout_color_mt("flight_data"));
    // tello_telemetry.SetFlightData(flight_data);

    // Test min:
    int limit = 15;
    set_low_bat_threshold(limit);
    while (keep_receiving)
    {
        get_low_bat_threshold();
        r = tello_socket.receive(asio::buffer(buffer_));

        std::vector<unsigned char> data(buffer_.begin(), buffer_.begin() + r); // Strip last 2 bytes. they are CRC16

        auto received = tello_protocol::Packet(data);
        auto cmd = uint16(received.GetBuffer()[5], received.GetBuffer()[6]);

        if (cmd == tello_protocol::LOW_BAT_THRESHOLD_MSG)
        {
            // tello_telemetry.GetFlightData()->SetLowBatThreshold(received.GetData());
            // ASSERT_EQ(tello_telemetry.GetFlightData()->GetLowBatThreshold(), limit);
            keep_receiving = false;
        }
        std::fill(buffer_.begin(), buffer_.end(), 0);
        std::this_thread::sleep_for(0.05s);
    }
    TearDownTestCase();
}

/**
 * @test Keep track of incoming undocumented data msg of type 0x0035
 * Setup:
 * * Create Tello instance.
 * * Attach to FlightData.
 * * Connect to drone.
 * Run:
 * * Sleep for a while. For gaining some PowerOn time.
 * Test:
 * * power_on_timer GREATER or EQUAL to time slept.
 * 
 * @note Notes on message 0x0035
 * * This is a short message: length of 11 bytes (data starts from byte number 7, last 2 are CRC)
 * * Data[7:8]:short   - Seems to be POWER_ON timer.
 * * Data[9]:bool      - <p>Unclear, it becomes true upon first takeoff since poweroff, \n
 *                       and remains TRUE while ON_AIR for first flight sinc POWER_ON.</p>
**/
TEST(WetTelloUndoccumentedDataTest, UndoccumentedDataMsg_0x0035_power_on_timer)
{
    // Setup
    TelloDriver tello(spdlog::level::info);
    Dummy dummy;
    tello.Attach(OBSERVERS::FLIGHT_DATA_MSG, &dummy);

    // Run
    tello.Connect();
    if (!tello.WaitForConnection(10))
    {
        tello.GetLogger()->error("Couldn't Connect to drone.");
        ASSERT_TRUE(false);
    }

    int sleep = 5;
    std::this_thread::sleep_for(std::chrono::seconds(sleep));

    // Test
    ASSERT_GE(dummy.GetFlightData().power_on_timer_info.power_on_timer, sleep);

    std::cout << "Done " << testing::UnitTest::GetInstance()->current_test_info()->name() << std::endl;
}

/**
 * @test Keep track of incoming undocumented data msg of type 0x0035
 * Preconditions:
 * * Drone didn't flew since turned on.
 * 
 * Setup:
 * * Create Tello instance.
 * * Attach to FlightData.
 * * Connect to drone.
 * Run:
 * Test1:
 * * undocumented_bool equal to FALSE.
 * Run:
 * * Takeoff()
 * Test2:
 * * undocumented_bool equal to TRUE.
 * 
 * @note Notes on message 0x0035
 * * This is a short message: length of 11 bytes (data starts from byte number 7, last 2 are CRC)
 * * Data[7:8]:short   - Seems to be POWER_ON timer.
 * * Data[9]:bool      - <p>Unclear, it becomes true upon first takeoff since poweroff, \n
 *                       and remains TRUE while ON_AIR for first flight sinc POWER_ON.</p>
**/
TEST(WetTelloUndoccumentedDataTest, UndoccumentedDataMsg_0x0035_has_flown_since_power_on)
{
    // Setup
    TelloDriver tello(spdlog::level::info);
    Dummy dummy;
    tello.Attach(OBSERVERS::FLIGHT_DATA_MSG, &dummy);
    int sleep = 5;

    // Run
    tello.Connect();
    if (!tello.WaitForConnection(10))
    {
        tello.GetLogger()->error("Couldn't Connect to drone.");
        ASSERT_TRUE(false);
    }

    // Test1
    EXPECT_FALSE(dummy.GetFlightData().power_on_timer_info.undocumented_bool);

    tello.Takeoff();
    std::this_thread::sleep_for(std::chrono::seconds(sleep));

    // Test2
    EXPECT_TRUE(dummy.GetFlightData().power_on_timer_info.undocumented_bool);

    tello.Land();
    std::this_thread::sleep_for(std::chrono::seconds(sleep));

    std::cout << "Done " << testing::UnitTest::GetInstance()->current_test_info()->name() << std::endl;
}
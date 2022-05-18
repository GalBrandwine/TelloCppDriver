// #include "commands_test.hpp"
// #include "helper.hpp"
// #include "TelloDriver/TelloDriver.hpp"
#include <gtest/gtest.h>
#include "TelloVisSocket.hpp"

#include "TelloTelemetry.hpp"
#include "TelloCommander.hpp"
#include "utils/TelloSocket.hpp"
#include "h264decoder/h264decoder.hpp"
#include <libavutil/frame.h>
#include <opencv2/highgui.hpp>
/**
 * @test The most basic test
*/
TEST(TelloVisSocketBasicTest, SimpleFirstInstantiation)
{
    TelloVisSocket tello_vis_socket();
    std::cout << "Done " << testing::UnitTest::GetInstance()->current_test_info()->name() << std::endl;
}

/**
 * @test See if trying to received when drone is offline working as expected.
 * 
*/
TEST(TelloVisSocketBasicTest, ReceiveWhenNotConnectedTest)
{
    std::unique_ptr<IVisReceiver> tello_vis_socket = std::make_unique<TelloVisSocket>();
    std::vector<unsigned char> data;
    auto bytes_received = tello_vis_socket->Receive(data);
    ASSERT_EQ(bytes_received, 0);

    std::cout << "Done " << testing::UnitTest::GetInstance()->current_test_info()->name() << std::endl;
}

/**
 * @test See if bytes received working propely, when connected to the drone.
 * 
 * SETUP:
 * * Connect to the drone using TelloSocket.
 * * Initiate a TelloVisSocket.
 * RUN:
 * * Send the command initiates a video stream.
 * * Call TelloVisSocket->Receive(data).
 * TEST:
 * * If any bytes received.
 */
TEST(TelloVisSocketBasicTest, ReceiveWhenConnectedTest)
{
    // Setup
    auto lvl = spdlog::level::debug;
    tello_protocol::TelloCommander tello_commander(spdlog::stdout_color_mt("tello_commander"), lvl);
    tello_protocol::TelloTelemetry tello_telemetry(spdlog::stdout_color_mt("tello_telemetry"), lvl);

    auto shared_socket = std::make_shared<TelloSocket>("192.168.10.1", 8889, 9000);
    tello_commander.SetSocket(shared_socket);
    tello_telemetry.SetSocket(shared_socket);

    bool keep_alive = true;
    std::unique_ptr<IVisReceiver> tello_vis_socket = std::make_unique<TelloVisSocket>();

    // Run
    tello_commander.SendConnReq();
    tello_telemetry.StartListening();

    auto simple_keep_alive = std::thread([&]() {
        while (keep_alive)
        {
            tello_commander.SendStickCommands();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    });

    //Test
    int times = 1000;
    std::vector<unsigned char> data(2000);
    tello_commander.SendStartVideo();
    int bytes_received = 0;

    while (bytes_received == 0)
    {
        bytes_received = tello_vis_socket->Receive(data);
        std::cout << "\n"
                  << bytes_received << "\n";
        tello_commander.SendStartVideo();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    std::cout << "bytes_received: " << bytes_received << "\n";
    ASSERT_NE(bytes_received, 0);

    keep_alive = false;
    simple_keep_alive.join();
    std::cout << "Done " << testing::UnitTest::GetInstance()->current_test_info()->name() << std::endl;
}

/**
 * @test Send a start_streaming request. And use ffmpeg to decipher the incoming data.
 * 
 * @note This command should show video stream: `ffplay udp://192.168.10.1:6038`
 * 
 * SETUP:
 * * Connect to the drone using TelloSocket.
 * * Initiate a TelloVisSocket.
 * RUN:
 * * Send the command initiates a video stream.
 * * Call TelloVisSocket->Receive(data).
 * TEST:
 * * If any bytes received.
 */
TEST(TelloVisSocketBasicTest, SimpleStartStreamingRequestTest)
{
    // Setup
    auto lvl = spdlog::level::debug;
    tello_protocol::TelloCommander tello_commander(spdlog::stdout_color_mt("tello_commander"), lvl);

    auto shared_socket = std::make_shared<TelloSocket>("192.168.10.1", 8889, 9000);
    tello_commander.SetSocket(shared_socket);

    bool keep_alive = true;
    // std::unique_ptr<IVisReceiver> tello_vis_socket = std::make_unique<TelloVisSocket>();

    H264Decoder decoder_;      // Decodes h264
    ConverterRGB24 converter_; // Converts pixels from YUV420P to BGR24

    auto buffer_ = std::vector<unsigned char>(2048);
    auto seq_buffer_ = std::vector<unsigned char>(65536);
    int seq_buffer_next_ = 0, seq_buffer_num_packets_ = 0;

    // Run
    tello_commander.SendConnReq();

    auto simple_keep_alive = std::thread([&]() {
        while (keep_alive)
        {
            // tello_commander.SendStickCommands();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    });

    
    //Test
    tello_commander.SendStartVideo();
    int bytes_received = 0;

    while (1)
    {
        tello_commander.SendStartVideo();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "bytes_received: " << bytes_received << "\n";
    ASSERT_NE(bytes_received, 0);

    keep_alive = false;
    simple_keep_alive.join();
    std::cout << "Done " << testing::UnitTest::GetInstance()->current_test_info()->name() << std::endl;
}

/**
 * @test Try to successfully create an image out of a stream of bytes.
 * 
 * SETUP:
 * * Connect to the drone using TelloSocket.
 * * Initiate a TelloVisSocket.
 * RUN:
 * * Send the command initiates a video stream.
 * * Call TelloVisSocket->Receive(data).
 * TEST:
 * * If any bytes received.
 */
TEST(TelloVisSocketBasicTest, CreatingCVmatOutOfPacketStream)
{
    // Setup
    auto lvl = spdlog::level::debug;
    tello_protocol::TelloCommander tello_commander(spdlog::stdout_color_mt("tello_commander"), lvl);
    tello_protocol::TelloTelemetry tello_telemetry(spdlog::stdout_color_mt("tello_telemetry"), lvl);

    auto shared_socket = std::make_shared<TelloSocket>("192.168.10.1", 8889, 9000);
    tello_commander.SetSocket(shared_socket);
    tello_telemetry.SetSocket(shared_socket);

    bool keep_alive = true;
    std::unique_ptr<IVisReceiver> tello_vis_socket = std::make_unique<TelloVisSocket>();

    H264Decoder decoder_;      // Decodes h264
    ConverterRGB24 converter_; // Converts pixels from YUV420P to BGR24

    auto buffer_ = std::vector<unsigned char>(2048);
    auto seq_buffer_ = std::vector<unsigned char>(65536);
    int seq_buffer_next_ = 0, seq_buffer_num_packets_ = 0;

    // Run
    tello_commander.SendConnReq();
    tello_telemetry.StartListening();

    auto simple_keep_alive = std::thread([&]() {
        while (keep_alive)
        {
            tello_commander.SendStickCommands();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    });

    auto decode_frames = [&]() {
        size_t next = 0;

        try
        {
            while (next < seq_buffer_next_)
            {
                // Parse h264
                ssize_t consumed = decoder_.parse(seq_buffer_.data() + next, seq_buffer_next_ - next);

                // Is a frame available?
                if (decoder_.is_frame_available())
                {
                    // Decode the frame
                    const AVFrame &frame = decoder_.decode_frame();

                    // Convert pixels from YUV420P to BGR24
                    int size = converter_.predict_size(frame.width, frame.height);
                    unsigned char bgr24[size];
                    converter_.convert(frame, bgr24);

                    // Convert to cv::Mat
                    cv::Mat mat{frame.height, frame.width, CV_8UC3, bgr24};

                    // Display
                    cv::imshow("frame", mat);
                    cv::waitKey(1);

                    // Synchronize ROS messages
                    // auto stamp = driver_->now();

                    // if (driver_->count_subscribers(driver_->image_pub_->get_topic_name()) > 0)
                    // {
                    //     std_msgs::msg::Header header{};
                    //     header.frame_id = "camera_frame";
                    //     header.stamp = stamp;
                    //     cv_bridge::CvImage cv_image{header, sensor_msgs::image_encodings::BGR8, mat};
                    //     sensor_msgs::msg::Image sensor_image_msg;
                    //     cv_image.toImageMsg(sensor_image_msg);
                    //     driver_->image_pub_->publish(sensor_image_msg);
                    // }

                    // if (driver_->count_subscribers(driver_->camera_info_pub_->get_topic_name()) > 0)
                    // {
                    //     camera_info_msg_.header.stamp = stamp;
                    //     driver_->camera_info_pub_->publish(camera_info_msg_);
                    // }
                }

                next += consumed;
            }
            // buffer_.clear();
        }
        catch (std::runtime_error e)
        {
            std::cerr << e.what();
            // RCLCPP_ERROR(driver_->get_logger(), e.what());
        }
    };

    //Test
    tello_commander.SendExposure(0);
    tello_commander.SendVideoEncoderRate();
    tello_commander.SendStartVideo();
    int bytes_received = 0;

    while (1)
    {
        bytes_received = tello_vis_socket->Receive(buffer_);
        std::cout << "\n"
                  << bytes_received << "\n";
        tello_commander.SendStartVideo();

        // std::this_thread::sleep_for(std::chrono::milliseconds(50));

        // if (!receiving_)
        // {
        //     // First packet
        //     RCLCPP_INFO(driver_->get_logger(), "Receiving video");
        //     receiving_ = true;
        //     seq_buffer_next_ = 0;
        //     seq_buffer_num_packets_ = 0;
        // }
        if (bytes_received > 0)
        {
            if (seq_buffer_next_ + bytes_received >= seq_buffer_.size())
            {
                // RCLCPP_ERROR(driver_->get_logger(), "Video buffer overflow, dropping sequence");
                std::cerr << "Video buffer overflow, dropping sequence \n";
                seq_buffer_next_ = 0;
                seq_buffer_num_packets_ = 0;
                // return;
                break;
            }

            std::copy(buffer_.begin(), buffer_.begin() + bytes_received, seq_buffer_.begin() + seq_buffer_next_);
            seq_buffer_next_ += bytes_received;
            seq_buffer_num_packets_++;

            // If the packet is < 1460 bytes then it's the last packet in the sequence
            if (bytes_received < 1460)
            {
                decode_frames();

                seq_buffer_next_ = 0;
                seq_buffer_num_packets_ = 0;
            }
        }
        // std::this_thread::sleep_for(std::chrono::milliseconds(10));
        bytes_received = 0;
    }

    std::cout << "bytes_received: " << bytes_received << "\n";
    ASSERT_NE(bytes_received, 0);

    keep_alive = false;
    simple_keep_alive.join();
    std::cout << "Done " << testing::UnitTest::GetInstance()->current_test_info()->name() << std::endl;
}

// /**
//  * @test Test if Emergency command works.
//  * @warning Sending EMERGENCY_CMD will kill all motors at once!
//  *
//  * Setup:
//  * * Create Tello instance.
//  * * Attach to FlightData.
//  * Run:
//  * * Connect to drone.
//  * * Send Takeoff() command
//  * Test:
//  * * Send Emergency() command
//  * * See if received EmergencyAck in TelloTelemtry.
//  */
// TEST(WetTelloCommandTests, EmergencyCommandTest)
// {
//     // Setup
//     TelloDriver tello(spdlog::level::info);
//     Dummy dummy;
//     tello.Attach(OBSERVERS::FLIGHT_DATA_MSG, &dummy);

//     // Run
//     tello.Connect();
//     if (!tello.WaitForConnection(10))
//     {
//         tello.GetLogger()->error("Couldn't Connect to drone.");
//         ASSERT_TRUE(false);
//     }

//     tello.Takeoff();
//     while (dummy.GetFlightData().height <= 8)
//     {

//         tello.GetLogger()->info("Takingoff, Waiting for drone to reach  height of 80 [cm]");
//         tello.GetLogger()->info("Current height: {}", dummy.GetFlightData().height);
//         std::this_thread::sleep_for(std::chrono::milliseconds(50));
//     }

//     /**
//      * @brief Simple physical notification to the user.
//      * This notify that test is proceeding to next stage.
//      */
//     tello.Down(50);
//     std::this_thread::sleep_for(std::chrono::milliseconds(500));
//     tello.Down(0);
//     std::this_thread::sleep_for(std::chrono::milliseconds(500));

//     // Test
//     int num_of_tries = 100;
//     while (num_of_tries >= 0)
//     {
//         auto fly_mode = dummy.GetFlightData().fly_mode;
//         tello.GetLogger()->info("Current fly mode: {}", std::to_string(fly_mode));
//         tello.GetLogger()->info("bat: {}", std::to_string(dummy.GetFlightData().battery_percentage));
//         tello.GetLogger()->info("drone_battery_left: {}", std::to_string(dummy.GetFlightData().drone_battery_left));
//         tello.GetLogger()->info("fly_time: {}", std::to_string(dummy.GetFlightData().fly_time));
//         tello.GetLogger()->info("drone_fly_time_left: {}\n", std::to_string(dummy.GetFlightData().drone_fly_time_left));
//         num_of_tries--;
//         std::this_thread::sleep_for(std::chrono::milliseconds(50));
//     }

//     tello.GetLogger()->info("*************** Sending EmergencyCmd! ***************");
//     tello.Emergency();
//     tello.GetLogger()->info("*************** After Sending EmergencyCmd! ***************");

//     num_of_tries = 100;
//     while (num_of_tries-- >= 0 && dummy.GetFlightData().fly_mode != 1)
//     {
//         auto fly_mode = dummy.GetFlightData().fly_mode;
//         tello.GetLogger()->info("Current fly mode: {}", std::to_string(fly_mode));
//         tello.GetLogger()->info("bat: {}", std::to_string(dummy.GetFlightData().battery_percentage));
//         tello.GetLogger()->info("drone_battery_left: {}", std::to_string(dummy.GetFlightData().drone_battery_left));
//         tello.GetLogger()->info("fly_time: {}", std::to_string(dummy.GetFlightData().fly_time));
//         tello.GetLogger()->info("drone_fly_time_left: {}\n", std::to_string(dummy.GetFlightData().drone_fly_time_left));
//         std::this_thread::sleep_for(std::chrono::milliseconds(50));
//     }
//     ASSERT_TRUE(true);

//     std::cout << "Done " << testing::UnitTest::GetInstance()->current_test_info()->name() << std::endl;
// }

// /**
//  * @test Test if ThrowAndGoCommand works.
//  * @warning This test require drone movements!
//  *
//  * Setup:
//  * * Create Tello instance.
//  * * Attach to FlightData.
//  * Run:
//  * * Connect to drone.
//  * * Send GetLowBatThresh() command
//  * Test:
//  * * See if Received LowBatThresh and stored corresclty.
//  */
// TEST(WetTelloCommandTests, ThrowAndGoTest)
// {
//     // Setup
//     TelloDriver tello(spdlog::level::info);
//     Dummy dummy;
//     tello.Attach(OBSERVERS::FLIGHT_DATA_MSG, &dummy);

//     // Run
//     tello.Connect();
//     if (!tello.WaitForConnection(10))
//     {
//         tello.GetLogger()->error("Couldn't Connect to drone.");
//         ASSERT_TRUE(false);
//     }

//     // Test
//     tello.ThrowAndGo();
//     tello.GetLogger()->info("Now the user should throw the drone away..\nWaiting for 5 secs.");
//     std::this_thread::sleep_for(std::chrono::seconds(5));

//     bool condition_met = false;
//     int num_of_tries = 1000;
//     while (num_of_tries >= 0)
//     {
//         auto fly_mode = dummy.GetFlightData().fly_mode;
//         tello.GetLogger()->info("Current fly mode: {}", std::to_string(fly_mode));
//         tello.GetLogger()->info("bat: {}", std::to_string(dummy.GetFlightData().battery_percentage));
//         tello.GetLogger()->info("drone_battery_left: {}", std::to_string(dummy.GetFlightData().drone_battery_left));
//         tello.GetLogger()->info("fly_time: {}", std::to_string(dummy.GetFlightData().fly_time));
//         tello.GetLogger()->info("drone_fly_time_left: {}\n", std::to_string(dummy.GetFlightData().drone_fly_time_left));
//         num_of_tries--;
//         std::this_thread::sleep_for(std::chrono::milliseconds(50));
//     }

//     tello.GetLogger()->info("*************** Landing ***************");
//     tello.Land();

//     num_of_tries = 1000;
//     while (num_of_tries >= 0)
//     {
//         auto fly_mode = dummy.GetFlightData().fly_mode;
//         tello.GetLogger()->info("Current fly mode: {}", fly_mode);
//         num_of_tries--;
//         std::this_thread::sleep_for(std::chrono::milliseconds(50));
//     }
//     ASSERT_TRUE(condition_met);

//     std::cout << "Done " << testing::UnitTest::GetInstance()->current_test_info()->name() << std::endl;
// }

// /**
//  * @test Test if PalmLand works.
//  * @warning This test require drone movements!
//  * @warning This test require human hands involvement's
//  *
//  * Setup:
//  * * Create Tello instance.
//  * * Attach to FlightData.
//  * Run:
//  * * Connect to drone.
//  * * Send Takeoff
//  * * Send PalmLand() command
//  * Test:
//  * * See if drone landed
//  */
// TEST(WetTelloCommandTests, PalmLandTest)
// {
//     // Setup
//     TelloDriver tello(spdlog::level::info);
//     Dummy dummy;
//     tello.Attach(OBSERVERS::FLIGHT_DATA_MSG, &dummy);

//     // Run
//     tello.Connect();
//     if (!tello.WaitForConnection(10))
//     {
//         tello.GetLogger()->error("Couldn't Connect to drone.");
//         ASSERT_TRUE(false);
//     }

//     // Test
//     tello.Takeoff();
//     while (dummy.GetFlightData().height <= 8)
//     {

//         tello.GetLogger()->info("Takingoff, Waiting for drone to reach  height of 80 [cm]");
//         tello.GetLogger()->info("Current height: {}", dummy.GetFlightData().height);
//         std::this_thread::sleep_for(std::chrono::milliseconds(50));
//     }

//     bool condition_met = false;
//     int num_of_tries = 100;
//     while (num_of_tries >= 0)
//     {
//         auto fly_mode = dummy.GetFlightData().fly_mode;
//         tello.GetLogger()->info("Current fly mode: {}", std::to_string(fly_mode));
//         tello.GetLogger()->info("bat: {}", std::to_string(dummy.GetFlightData().battery_percentage));
//         tello.GetLogger()->info("drone_battery_left: {}", std::to_string(dummy.GetFlightData().drone_battery_left));
//         tello.GetLogger()->info("fly_time: {}", std::to_string(dummy.GetFlightData().fly_time));
//         tello.GetLogger()->info("drone_fly_time_left: {}\n", std::to_string(dummy.GetFlightData().drone_fly_time_left));
//         num_of_tries--;
//         std::this_thread::sleep_for(std::chrono::milliseconds(50));
//     }

//     tello.Down(50);
//     std::this_thread::sleep_for(std::chrono::milliseconds(500));
//     tello.Down(0);
//     std::this_thread::sleep_for(std::chrono::milliseconds(500));

//     tello.GetLogger()->info("*************** PalmLanding ***************");
//     tello.PalmLand();
//     tello.GetLogger()->info("*************** After PalmLanding ***************");

//     num_of_tries = 100;
//     while (num_of_tries-- >= 0 && dummy.GetFlightData().fly_mode != 1)
//     {
//         auto fly_mode = dummy.GetFlightData().fly_mode;
//         tello.GetLogger()->info("Current fly mode: {}", std::to_string(fly_mode));
//         tello.GetLogger()->info("bat: {}", std::to_string(dummy.GetFlightData().battery_percentage));
//         tello.GetLogger()->info("drone_battery_left: {}", std::to_string(dummy.GetFlightData().drone_battery_left));
//         tello.GetLogger()->info("fly_time: {}", std::to_string(dummy.GetFlightData().fly_time));
//         tello.GetLogger()->info("drone_fly_time_left: {}\n", std::to_string(dummy.GetFlightData().drone_fly_time_left));
//         std::this_thread::sleep_for(std::chrono::milliseconds(50));
//     }
//     ASSERT_TRUE(1);

//     std::cout << "Done " << testing::UnitTest::GetInstance()->current_test_info()->name() << std::endl;
// }

// /**
//  * @test Test if SetFastMode working properly.
//  *
//  * @warning: this test evolve drone movements!
//  *
//  * Setup:
//  * Create Tello instance.
//  * Attach to FlightData.
//  * Connect to drone.
//  * Run:
//  * * Takeoff()
//  * Test:
//  * * Send Flip commands to all possible directions.
//  * * Land()
// **/
// TEST(WetTelloCommandTests, SendFlipCommands)
// {
//     std::cout << "Start " << testing::UnitTest::GetInstance()->current_test_info()->name() << std::endl;
//     // Setup
//     TelloDriver tello(spdlog::level::info);
//     Dummy dummy;
//     tello.Attach(OBSERVERS::FLIGHT_DATA_MSG, &dummy);

//     // Run
//     tello.Connect();
//     if (!tello.WaitForConnection(10))
//     {
//         tello.GetLogger()->error("Couldn't Connect to drone.");
//         ASSERT_TRUE(false);
//     }

//     // Test
//     tello.GetLogger()->info("Takeoff");
//     tello.Takeoff();

//     while (dummy.GetFlightData().fly_mode < 11)
//     {
//         tello.GetLogger()->info("Waiting for takeoff");
//         std::this_thread::sleep_for(std::chrono::seconds(1));
//     }

//     while (dummy.GetFlightData().fly_mode == 11)
//     {
//         tello.GetLogger()->info("Takingoff");
//         std::this_thread::sleep_for(std::chrono::seconds(1));
//     }

//     tello.GetLogger()->info(tello_protocol::flip_direction_to_string(tello_protocol::FlipDirections::FlipFront));
//     tello.Flip(tello_protocol::FlipDirections::FlipFront);
//     while (dummy.GetFlightData().fly_mode < 34)
//     {

//         tello.GetLogger()->info("In Holding position fly_mode");
//         std::this_thread::sleep_for(std::chrono::seconds(1));
//     }

//     while (dummy.GetFlightData().fly_mode == 34)
//     {
//         tello.GetLogger()->info("In Acrobat fly_mode");
//         std::this_thread::sleep_for(std::chrono::seconds(1));
//     }

//     tello.GetLogger()->info(tello_protocol::flip_direction_to_string(tello_protocol::FlipDirections::FlipBack));
//     tello.Flip(tello_protocol::FlipDirections::FlipBack);

//     while (dummy.GetFlightData().fly_mode <= 6)
//     {
//         tello.GetLogger()->info("In Holding position fly_mode");
//         std::this_thread::sleep_for(std::chrono::seconds(1));
//     }

//     while (dummy.GetFlightData().fly_mode == 34)
//     {
//         tello.GetLogger()->info("In Acrobat fly_mode");
//         std::this_thread::sleep_for(std::chrono::seconds(1));
//     }

//     tello.GetLogger()->info(tello_protocol::flip_direction_to_string(tello_protocol::FlipDirections::FlipBackLeft));
//     tello.Flip(tello_protocol::FlipDirections::FlipBackLeft);

//     while (dummy.GetFlightData().fly_mode <= 6)
//     {
//         tello.GetLogger()->info("In Holding position fly_mode");
//         std::this_thread::sleep_for(std::chrono::seconds(1));
//     }

//     while (dummy.GetFlightData().fly_mode == 34)
//     {
//         tello.GetLogger()->info("In Acrobat fly_mode");
//         std::this_thread::sleep_for(std::chrono::seconds(1));
//     }

//     tello.GetLogger()->info(tello_protocol::flip_direction_to_string(tello_protocol::FlipDirections::FlipBackRight));
//     tello.Flip(tello_protocol::FlipDirections::FlipBackRight);

//     while (dummy.GetFlightData().fly_mode <= 6)
//     {
//         tello.GetLogger()->info("In Holding position fly_mode");
//         std::this_thread::sleep_for(std::chrono::seconds(1));
//     }

//     while (dummy.GetFlightData().fly_mode == 34)
//     {
//         tello.GetLogger()->info("In Acrobat fly_mode");
//         std::this_thread::sleep_for(std::chrono::seconds(1));
//     }

//     tello.GetLogger()->info(tello_protocol::flip_direction_to_string(tello_protocol::FlipDirections::FlipForwardLeft));
//     tello.Flip(tello_protocol::FlipDirections::FlipForwardLeft);

//     while (dummy.GetFlightData().fly_mode <= 6)
//     {
//         tello.GetLogger()->info("In Holding position fly_mode");
//         std::this_thread::sleep_for(std::chrono::seconds(1));
//     }

//     while (dummy.GetFlightData().fly_mode == 34)
//     {
//         tello.GetLogger()->info("In Acrobat fly_mode");
//         std::this_thread::sleep_for(std::chrono::seconds(1));
//     }

//     tello.GetLogger()->info(tello_protocol::flip_direction_to_string(tello_protocol::FlipDirections::FlipForwardRight));
//     tello.Flip(tello_protocol::FlipDirections::FlipForwardRight);

//     while (dummy.GetFlightData().fly_mode <= 6)
//     {
//         tello.GetLogger()->info("In Holding position fly_mode");
//         std::this_thread::sleep_for(std::chrono::seconds(1));
//     }

//     while (dummy.GetFlightData().fly_mode == 34)
//     {
//         tello.GetLogger()->info("In Acrobat fly_mode");
//         std::this_thread::sleep_for(std::chrono::seconds(1));
//     }

//     tello.GetLogger()->info(tello_protocol::flip_direction_to_string(tello_protocol::FlipDirections::FlipLeft));
//     tello.Flip(tello_protocol::FlipDirections::FlipLeft);

//     while (dummy.GetFlightData().fly_mode <= 6)
//     {
//         tello.GetLogger()->info("In Holding position fly_mode");
//         std::this_thread::sleep_for(std::chrono::seconds(1));
//     }

//     while (dummy.GetFlightData().fly_mode == 34)
//     {
//         tello.GetLogger()->info("In Acrobat fly_mode");
//         std::this_thread::sleep_for(std::chrono::seconds(1));
//     }

//     tello.GetLogger()->info(tello_protocol::flip_direction_to_string(tello_protocol::FlipDirections::FlipRight));
//     tello.Flip(tello_protocol::FlipDirections::FlipRight);

//     while (dummy.GetFlightData().fly_mode <= 6)
//     {
//         tello.GetLogger()->info("In Holding position fly_mode");
//         std::this_thread::sleep_for(std::chrono::seconds(1));
//     }

//     while (dummy.GetFlightData().fly_mode == 34)
//     {
//         tello.GetLogger()->info("In Acrobat fly_mode");
//         std::this_thread::sleep_for(std::chrono::seconds(1));
//     }

//     tello.GetLogger()->info("Sending Land command");
//     tello.Land();
//     while (dummy.GetFlightData().fly_mode <= 6)
//     {
//         tello.GetLogger()->info("In Holding position fly_mode");
//         std::this_thread::sleep_for(std::chrono::seconds(1));
//     }

//     while (dummy.GetFlightData().fly_mode == 12)
//     {
//         tello.GetLogger()->info("Landing");
//         std::this_thread::sleep_for(std::chrono::seconds(1));
//     }

//     // while (dummy.GetFlightData().fly_mode <= 6)
//     // {
//     //     tello.GetLogger()->info("In Holding position fly_mode");
//     //     std::this_thread::sleep_for(std::chrono::seconds(1));
//     // }

//     // while (dummy.GetFlightData().fly_mode == 1)
//     // {
//     //     tello.GetLogger()->info("On ground");
//     //     std::this_thread::sleep_for(std::chrono::seconds(1));
//     //     break;
//     // }
//     std::cout << "Done " << testing::UnitTest::GetInstance()->current_test_info()->name() << std::endl;
// }

// /**
// * @test Test if driver allert about drone disconnection.
// * Manually turn off the drone after connection
// **/
// TEST(WetTelloSticksCommandTest, TestDisconnectionAllert)
// {
//     // Setup
//     TelloDriver tello(spdlog::level::debug);

//     // Run
//     tello.Connect();
//     if (!tello.WaitForConnection(10))
//     {
//         tello.GetLogger()->error("Couldn't Connect to drone.");
//         ASSERT_TRUE(false);
//     }

//     // Test
//     int counter = 100;
//     bool res = true;
//     while (counter-- > 0 && res == true)
//     {
//         // res = tello.GetTelloTelemetry().IsDroneConnected();
//         std::this_thread::sleep_for(std::chrono::milliseconds(100));
//     }

//     ASSERT_TRUE(res);

//     std::cout << "Done " << testing::UnitTest::GetInstance()->current_test_info()->name() << std::endl;
// }

// /**
//  * @test Test if drone receives neutral stick commands.
//  * @example:
//  *  No movements. Just keep alive heartbeat.
//  *  If everything work properly. Drone's LED suppose to blink with GREEN
//  */
// TEST(WetTelloSticksCommandTest, SendNeutralStickCommand)
// {
//     // Setup
//     TelloDriver tello(spdlog::level::debug);

//     // Run
//     tello.Connect();
//     if (!tello.WaitForConnection(10))
//     {
//         tello.GetLogger()->error("Couldn't Connect to drone.");
//         ASSERT_TRUE(false);
//     }

//     // Test
//     int counter = 100;
//     while (counter-- > 0)
//     {

//         std::this_thread::sleep_for(std::chrono::milliseconds(100));
//     }

//     std::cout << "Done " << testing::UnitTest::GetInstance()->current_test_info()->name() << std::endl;
// }

// /**
//  * @test Movements
//  *
//  * @warning: this test evolve drone movements!
//  * 1. Takeoff
//  * 2. Move backward 1% stick, for 1 sec.
//  * 3. Land
// **/
// TEST(WetTelloSticksCommandTest, SendBackwardStickCommand)
// {
//     // Setup
//     TelloDriver tello(spdlog::level::info);

//     // Run
//     tello.Connect();
//     if (!tello.WaitForConnection(10))
//     {
//         tello.GetLogger()->error("Couldn't Connect to drone.");
//         ASSERT_TRUE(false);
//     }

//     // Test
//     tello.Takeoff();
//     std::this_thread::sleep_for(std::chrono::seconds(5));
//     tello.Backward(1);

//     int counter = 10;
//     while (counter-- > 0)
//     {
//         // ASSERT_TRUE(tello.GetTelloTelemetry().IsDroneConnected());
//         // auto droneMode = tello.GetTelloTelemetry().GetFlightData()->GetFlightMode();
//         // tello.GetLogger()->info("DroneMode: " + std::to_string(droneMode));

//         // auto pos = tello.GetPos();
//         // tello.GetLogger()->info("********************************pos: " + std::to_string(pos.x) + "," + std::to_string(pos.y) + "," + std::to_string(pos.z));

//         std::this_thread::sleep_for(std::chrono::milliseconds(100));
//     }

//     tello.Backward(0);
//     std::this_thread::sleep_for(std::chrono::seconds(5));
//     tello.Land();
//     std::this_thread::sleep_for(std::chrono::seconds(5));

//     std::cout << "Done " << testing::UnitTest::GetInstance()->current_test_info()->name() << std::endl;
// }

// /**
//  * @test Test SetAttLimit command
//  * Setup:
//  * * Create Tello instance.
//  * * Attach to FlightData.
//  * * Connect to drone.
//  * Run:
//  * * Send SetAttLimitReq(new_limit)
//  * Test:
//  * * See If implemented observer being called with new AttLimit.
// **/
// TEST(WetTelloAttitudeTest, SendSetAttLimitTest)
// {
//     // Setup
//     TelloDriver tello(spdlog::level::info);
//     Dummy dummy;
//     tello.Attach(OBSERVERS::FLIGHT_DATA_MSG, &dummy);

//     // Run
//     tello.Connect();
//     if (!tello.WaitForConnection(10))
//     {
//         tello.GetLogger()->error("Couldn't Connect to drone.");
//         ASSERT_TRUE(false);
//     }

//     int limit = 34;
//     tello.SetAttLimitReq(limit);

//     // Wait for Drone to reply.
//     bool condition_met = false;
//     int num_of_tries = 100;

//     while (!condition_met and num_of_tries >= 0)
//     {
//         condition_met = dummy.GetFlightData().attitude_limit == limit;
//         num_of_tries--;
//         std::this_thread::sleep_for(std::chrono::milliseconds(100));
//     }

//     // Test
//     ASSERT_TRUE(condition_met);

//     std::cout << "Done " << testing::UnitTest::GetInstance()->current_test_info()->name() << std::endl;
// }

// /**
//  * @test Test If LowBatThreshMsg received and stored correctly.
//  * Setup:
//  * * Create Tello instance.
//  * * Attach to FlightData.
//  * * Connect to drone.
//  * Run:
//  * * Send GetLowBatThresh() command
//  * Test:
//  * * See if Received LowBatThresh and stored corresclty.
// **/
// TEST(WetTelloBatteryTest, GetLowBatThreshMsgTest)
// {
//     // Setup
//     TelloDriver tello(spdlog::level::info);
//     Dummy dummy;
//     tello.Attach(OBSERVERS::FLIGHT_DATA_MSG, &dummy);

//     // Run
//     tello.Connect();
//     if (!tello.WaitForConnection(10))
//     {
//         tello.GetLogger()->error("Couldn't Connect to drone.");
//         ASSERT_TRUE(false);
//     }

//     // Test
//     bool condition_met = false;
//     int num_of_tries = 100;
//     while (!condition_met and num_of_tries >= 0)
//     {
//         condition_met = dummy.GetFlightData().low_battery_threshold != -1;
//         num_of_tries--;
//         std::this_thread::sleep_for(std::chrono::milliseconds(100));
//     }

//     ASSERT_TRUE(condition_met);

//     std::cout << "Done " << testing::UnitTest::GetInstance()->current_test_info()->name() << std::endl;
// }

// /**
//  * @test Test If LOW_BAT_THRESHOLD_CMD command working properly.
//  * Setup:
//  * * Create Tello instance.
//  * * Attach to FlightData.
//  * * Connect to drone.
//  * Run:
//  * * Send SetLowBatThresh(new_thresh) command
//  * Test:
//  * * See if received LowBatThresh has desired new thresh.
// **/
// TEST(WetTelloBatteryTest, SetLowBatThreshMsgTest)
// {
//     // Setup
//     TelloDriver tello(spdlog::level::info);
//     Dummy dummy;
//     tello.Attach(OBSERVERS::FLIGHT_DATA_MSG, &dummy);

//     // Run
//     tello.Connect();
//     if (!tello.WaitForConnection(10))
//     {
//         tello.GetLogger()->error("Couldn't Connect to drone.");
//         ASSERT_TRUE(false);
//     }

//     int new_thresh = 50;
//     tello.SetBatThreshReq(new_thresh);

//     // Test
//     bool condition_met = false;
//     int num_of_tries = 100;
//     while (!condition_met and num_of_tries >= 0)
//     {
//         condition_met = dummy.GetFlightData().low_battery_threshold == new_thresh;
//         num_of_tries--;
//         std::this_thread::sleep_for(std::chrono::milliseconds(100));
//     }

//     ASSERT_TRUE(condition_met);

//     std::cout << "Done " << testing::UnitTest::GetInstance()->current_test_info()->name() << std::endl;
// }

// /**
//  * @test Test if SetFastMode working properly.
//  *
//  * @warning: this test evolve drone movements!
//  * 1. Takeoff
//  * 2. Move backward 10% stick, for 1 sec.
//  * 3. Land
//  *
//  * Setup:
//  * Create Tello instance.
//  * Attach to FlightData.
//  * Connect to drone.
//  * Run:
//  * * Takeoff()
//  * Test:
//  * * Set backward stick 10%. (The drone will start to move)
//  * * SetFastMode(true)
//  * * Wait for 5 seconds
//  * * SetFastMode(false)
//  * * Set backward stick 0%. (The drone will stop moving)
//  * * Land()
// **/
// TEST(WetTelloSticksCommandTest, ToggleFastMode)
// {
//     // Setup
//     TelloDriver tello(spdlog::level::info);
//     Dummy dummy;
//     tello.Attach(OBSERVERS::FLIGHT_DATA_MSG, &dummy);

//     // Run
//     tello.Connect();
//     if (!tello.WaitForConnection(10))
//     {
//         tello.GetLogger()->error("Couldn't Connect to drone.");
//         ASSERT_TRUE(false);
//     }

//     // Test
//     tello.Takeoff();
//     std::this_thread::sleep_for(std::chrono::seconds(5));

//     tello.Backward(25);
//     std::this_thread::sleep_for(std::chrono::seconds(2));

//     tello.GetLogger()->warn("Setting FastMode true");
//     tello.SetFastMode(true);

//     std::this_thread::sleep_for(std::chrono::seconds(2));

//     tello.GetLogger()->warn("Setting FastMode false");
//     tello.SetFastMode(false);

//     tello.Backward(0);
//     std::this_thread::sleep_for(std::chrono::seconds(5));

//     tello.Land();
//     std::this_thread::sleep_for(std::chrono::seconds(5));

//     std::cout << "Done " << testing::UnitTest::GetInstance()->current_test_info()->name() << std::endl;
// }
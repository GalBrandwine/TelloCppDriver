#include "socket_test.hpp"
using namespace std::chrono_literals;
#include <cstdlib>
#include <iostream>
#include "asio.hpp"
#include <thread>
using asio::ip::udp;

// Lowest level testing: test aisio library async capabilities
TEST(AsioSocketAsyncOperations, TrySendConnReq)
{

    // We run the io_context off in its own thread so that it operates
    // completely asynchronously with respect to the rest of the program.
    asio::io_context io_context;
    asio::executor_work_guard<asio::io_context::executor_type> work_(asio::make_work_guard(io_context));
    std::thread thread(std::thread([&io_context]() { io_context.run(); std::cout << "work_io_context_ finished!\n"; }));

    server s(io_context, std::atoi("9000"));
    char connreq[1024];
    std::string conn = "conn_req:\x96\x17";
    int index = 0;
    for (auto &letter : conn)
        connreq[index++] = letter;
    s.do_send(connreq, conn.length());
    // io_context.run(); // <--- BLOKING

    int timeout_ticks = 500;
    while (!s.is_bytes_recvd)
    {
        std::this_thread::sleep_for(20ms);
        if (timeout_ticks-- < 0)
        {
            break;
        }
    }
    EXPECT_TRUE(s.is_bytes_recvd);
    io_context.stop();
    thread.join();
    std::cout << "Done " << testing::UnitTest::GetInstance()->current_test_info()->name() << std::endl;
}

// Mid-level testing: test if 'connReq' being sent correctly
TEST(TelloSocketAsync, TrySendConnReq)
{
    // Implememted using this example:
    // https://www.boost.org/doc/libs/1_66_0/doc/html/boost_asio/example/cpp03/services/logger_service.hpp
    TelloSocket tello_socket("192.168.10.1", 8889, 9000);

    tello_socket.Send("conn_req:\x96\x17");

    int timeout_ticks = 500;
    while (!tello_socket.IsBytesReceived())
    {
        std::this_thread::sleep_for(20ms);
        if (timeout_ticks-- < 0)
        {
            break;
        }
    }
    EXPECT_TRUE(tello_socket.IsBytesReceived());

    // io_context.stop();
    // thread.join();
    std::cout << "Done " << testing::UnitTest::GetInstance()->current_test_info()->name() << std::endl;
}

/* Mid-level testing:
 * Test if 'connAck' being received correctly.
 * If so. this means a connection has been established,
 *  */
TEST(TelloSocketAsync, TrySendRecieveConnReq)
{
    TelloSocket tello_socket("192.168.10.1", 8889, 9000);

    std::string msg("conn_req:\x96\x17");
    tello_socket.Send(msg);
    int timeout_ticks = 500;

    std::vector<unsigned char> received_data(1024);
    int bytes_recieve = 0;
    // Run
    while (bytes_recieve != 11)
    {
        tello_socket.Send(msg);
        std::this_thread::sleep_for(20ms);
        bytes_recieve = tello_socket.Receive(received_data);
        if (timeout_ticks-- < 0)
        {
            break;
        }
    }

    // Test
    std::vector<unsigned char> rec(received_data.begin(), received_data.begin() + 4);
    std::vector<unsigned char> sended(msg.begin(), msg.begin() + 4);
    EXPECT_EQ(sended, rec);

    // io_context.stop();
    // thread.join();
    std::cout << "Done " << testing::UnitTest::GetInstance()->current_test_info()->name() << std::endl;
}

// TEST(TelloSocketAsync, TryTakeoffThroughSocketDirectly)
// {
//     // This is a 'active' test - the drone will Takeoff!!
//     TelloSocket tello_socket("192.168.10.1", 8889, 9000);

//     std::string msg("conn_req:\x96\x17");
//     tello_socket.Send(msg);
//     int timeout_ticks = 500;

//     std::vector<unsigned char> received_data(1024);
//     int bytes_recieve = 0;
//     // Run
//     while (bytes_recieve != 11)
//     {
//         tello_socket.Send(msg);
//         std::this_thread::sleep_for(20ms);
//         bytes_recieve = tello_socket.Receive(received_data);
//         if (timeout_ticks-- < 0)
//         {
//             break;
//         }
//     }

//     // Test
//     auto pkt = tello_protocol::Packet(tello_protocol::TAKEOFF_CMD);
//     pkt.Fixup();
//     tello_socket.Send(pkt.GetBuffer());
// }
#include "TelloVisSocket.hpp"

void TelloVisSocket::in_data_worker()
{
    while (m_keep_running)
    {
        do_receive();
        m_is_bytes_received = true;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

void TelloVisSocket::do_receive()
{
    try
    {
        m_tello_socket->wait(m_tello_socket->wait_read);
        m_bytes_recvd = m_tello_socket->receive(
            boost::asio::buffer(data_, max_length));
    }
    catch (const std::exception &e)
    {
        std::cerr << __PRETTY_FUNCTION__ << "::" << __LINE__ << " " << e.what() << '\n';
    }
}

int TelloVisSocket::Receive(std::vector<unsigned char> &data)
{
            m_tello_socket->wait(m_tello_socket->wait_read);
        int bytes_recvd = m_tello_socket->receive(
            boost::asio::buffer(data, max_length));
    return bytes_recvd;    

    // if (m_is_bytes_received)
    // {
    //     int bytes_recvd = m_bytes_recvd;
    //     m_bytes_recvd = 0;
    //     std::copy(data_, data_ + bytes_recvd, data.begin());
    //     memset(data_, 0, sizeof(data_));
    //     return bytes_recvd;
    // }
    // return 0;
}

// void TelloVisSocket::Send(const std::string &cmd)
// {
//     try
//     {
//         m_tello_socket->wait(m_tello_socket->wait_write);
//         m_tello_socket->send_to(boost::asio::buffer(cmd, cmd.length()), m_sender_endpoint);
//     }
//     catch (const std::exception &e)
//     {
//         std::cerr << __PRETTY_FUNCTION__ << "::" << __LINE__ << " " << e.what() << '\n';
//     }
// }

TelloVisSocket::~TelloVisSocket()
{
    m_keep_running = false;
    boost::system::error_code ignored;

    if (m_tello_socket != nullptr)
    {
        m_tello_socket->shutdown(m_tello_socket->shutdown_both, ignored);
    }

    if (m_in_socket_thread.joinable())
    {
        m_any_received = true;
        m_in_socket_thread.join();
    }
    std::cout << "TelloVisSocket Destructed.\n";
}

TelloVisSocket::TelloVisSocket(const short droneVideoPort)
    : m_droneVideoPort(droneVideoPort)
{
    m_tello_socket = std::make_shared<boost::asio::ip::udp::socket>(work_io_context_, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), droneVideoPort));
    // m_in_socket_thread = std::thread(std::bind(&TelloVisSocket::in_data_worker, this));
}
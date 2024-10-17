#include "TelloSocket.hpp"

void TelloSocket::in_data_worker()
{
    while (m_keep_running)
    {
        do_receive();
        m_is_bytes_received = true;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

void TelloSocket::do_receive()
{
    try
    {
        m_tello_socket->wait(m_tello_socket->wait_read);
        m_bytes_recvd = m_tello_socket->receive_from(
            asio::buffer(data_, max_length), m_sender_endpoint);
    }
    catch (const std::exception &e)
    {
        std::cerr << __PRETTY_FUNCTION__ << "::" << __LINE__ << " " << e.what() << '\n';
    }
}

int TelloSocket::Receive(std::vector<unsigned char> &data)
{

    if (m_is_bytes_received)
    {
        int bytes_recvd = m_bytes_recvd;
        m_bytes_recvd = 0;
        std::copy(data_, data_ + bytes_recvd, data.begin());
        memset(data_, 0, sizeof(data_));
        // do_receive();
        return bytes_recvd;
    }
    return 0;
}

void TelloSocket::Send(const std::string &cmd)
{
    try
    {
        m_tello_socket->wait(m_tello_socket->wait_write);
        m_tello_socket->send_to(asio::buffer(cmd, cmd.length()), m_sender_endpoint);
    }
    catch (const std::exception &e)
    {
        std::cerr << __PRETTY_FUNCTION__ << "::" << __LINE__ << " " << e.what() << '\n';
    }
}

TelloSocket::~TelloSocket()
{
    m_keep_running = false;
    asio::error_code ignored;

    if (m_tello_socket != nullptr)
    {
        m_tello_socket->shutdown(m_tello_socket->shutdown_both, ignored);
    }

    if (m_in_socket_thread.joinable())
    {
        m_any_received = true;
        m_in_socket_thread.join();
    }
    std::cout << "TelloSocket Destructed.\n";
}

TelloSocket::TelloSocket(const std::string &droneIp, const short droneCommandPort, const short droneDataPort)
{
    m_tello_socket = std::make_shared<asio::ip::udp::socket>(work_io_context_, asio::ip::udp::endpoint(asio::ip::udp::v4(), droneDataPort));
    m_sender_endpoint = asio::ip::udp::endpoint(asio::ip::address_v4::from_string(droneIp), droneCommandPort);

    m_in_socket_thread = std::thread(std::bind(&TelloSocket::in_data_worker, this));
}
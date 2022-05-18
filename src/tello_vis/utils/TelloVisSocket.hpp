#pragma once
#include "IVisReceiver.hpp"
#include <mutex>
#include <atomic>
#include <iostream>
#include <thread>
#include <cstdlib>
#include "boost/asio.hpp"

class TelloVisSocket : public IVisReceiver
{
public:
    // /**
    //  * @brief Receive new bytes from socket (None blocking).
    //  *
    //  * @param[out] data - buffer filled with new data.
    //  * @return int amount of received bytes.
    //  */
    int Receive(std::vector<unsigned char> &data) override;

    /**
     * @brief Send command to socket.
     * 1. Wait for socket to be in *read* mode.
     * 2. Block until operation complete.
     * 
     * @param cmd const std::string & filled with data.
     */
    // void Send(const std::string &cmd) override;

    TelloVisSocket(const short droneVideoPort = 6038);
    ~TelloVisSocket();

private:
    /**
     * @brief Synced socket receive.
     * 1.Wait for socket to be ready to *read*.
     * 2.**Block** until socket return with new data.
     * 
     */
    void do_receive();

    std::mutex m_sendM;

    bool m_is_bytes_received = false;
    // unsigned short m_port = 8889;
    unsigned short m_droneVideoPort = 6038; // Maybe to this port: 11111. Video listening is on lochalhost.
    std::shared_ptr<boost::asio::ip::udp::socket> m_tello_socket;
    // boost::asio::ip::udp::endpoint m_sender_endpoint;

    size_t m_bytes_recvd;
    enum
    {
        max_length = 2000
    };
    std::atomic<char> data_[max_length];

    /// Private io_context used for performing logging operations.
    boost::asio::io_context work_io_context_;

    void in_data_worker();

    std::thread m_in_socket_thread;
    bool m_keep_running = true;
    bool m_any_received = false;
};
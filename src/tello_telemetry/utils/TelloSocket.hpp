#pragma once
#include "IReciever.hpp"
#include "ISender.hpp"
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <iostream>
#include <functional>
#include <thread>
#include <cstdlib>
#include "boost/asio.hpp"

class TelloSocket : public ISender, public IReciever
{
public:
    /**
     * @brief Receive new bytes from socket (None blocking).
     * 
     * @param[out] data - buffer filled with new data.
     * @return int amount of received bytes.
     */
    int Receive(std::vector<unsigned char> &data) override;

    /**
     * @brief Send command to socket.
     * 1. Wait for socket to be in *read* mode.
     * 2. Block until operation complete.
     * 
     * @param cmd const std::string & filled with data.
     */
    void Send(const std::string &cmd) override;

    TelloSocket(const std::string &droneIp, const short droneCommandPort, const short droneDataPort);
    ~TelloSocket();

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
    unsigned short m_port = 8889;
    unsigned short droneDataPort = 9000;
    std::shared_ptr<boost::asio::ip::udp::socket> m_tello_socket;
    boost::asio::ip::udp::endpoint m_sender_endpoint;

    size_t m_bytes_recvd;
    enum
    {
        max_length = 2000
    };
    std::atomic<char> data_[max_length];

    /// Private io_context used for performing logging operations.
    boost::asio::io_context work_io_context_;

    /// Work for the private io_context to perform. If we do not give the
    /// io_context some work to do then the io_context::run() function will exit
    /// immediately.
    // boost::asio::executor_work_guard<
    //     boost::asio::io_context::executor_type>
    //     work_;

    /// Thread used for running the work io_context's run loop.
    // std::thread work_thread_;

    void in_data_worker();

    std::thread m_in_socket_thread;
    bool m_keep_running = true;
    bool m_any_received = false;
};
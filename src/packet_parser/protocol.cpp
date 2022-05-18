#include "protocol.hpp"

namespace tello_protocol
{
    Packet::Packet(const std::string &cmd)
    {
        m_bufSize = cmd.length();
        m_buf_hex = cmd;
    }
    Packet::Packet(const std::string &cmd, const std::string &payload, const unsigned char pkt_type)
    {
        if ((unsigned char)cmd.at(0) != tello_protocol::START_OF_PACKET)
        {
            throw std::invalid_argument{"Not A Tello packet"};
            return;
        }
        m_bufSize = cmd.length();
        m_buf_hex = cmd;
    }
    Packet::Packet(const std::vector<unsigned char> &data)
        : m_bufSize(data.size())
    {
        // Method 1 - Timer took: 0.060789ms
        // for (auto &letter : data)
        //     m_buf_hex += letter;

        // Method 2 - Timer took: 0.057199ms
        m_buf_hex = std::string{data.begin(), data.end()};
    }

    Packet::Packet(const unsigned int cmd, const unsigned char pkt_type)
    {
        m_buf_hex.reserve(2000); // Bytes;
        m_buf_hex.push_back(tello_protocol::START_OF_PACKET);
        m_buf_hex.push_back(0);
        m_buf_hex.push_back(0);
        m_buf_hex.push_back(0);
        m_buf_hex.push_back(pkt_type);
        m_buf_hex.push_back(cmd & 0xff);
        m_buf_hex.push_back((cmd >> 8) & 0xff);
        m_buf_hex.push_back(0);
        m_buf_hex.push_back(0);

        m_bufSize = m_buf_hex.length();
    }
    Packet::Packet(const unsigned int cmd, const std::string &payload, const unsigned char pkt_type)
    {
        m_buf_hex.reserve(2000); // Bytes;
        m_buf_hex.at(0) = tello_protocol::START_OF_PACKET;
        m_buf_hex.at(1) = 0;
        m_buf_hex.at(2) = 0;
        m_buf_hex.at(3) = 0;
        m_buf_hex.at(4) = pkt_type;
        m_buf_hex.at(5) = (cmd & 0xff);
        m_buf_hex.at(6) = ((cmd >> 8) & 0xff);
        m_buf_hex.at(7) = 0;
        m_buf_hex.at(8) = 0;

        if (payload.length() != 0)
            m_buf_hex.append(payload);

        m_bufSize = m_buf_hex.length();
    }
    Packet::~Packet()
    {
        // delete[] m_buf;
    }
    const std::string Packet::GetData() const
    {
        return m_buf_hex.substr(9, m_bufSize);
    }
    const std::string &Packet::GetBuffer() const
    {
        return m_buf_hex;
    }
    const int Packet::GetBufferSize() const
    {
        return m_bufSize;
    }
    void Packet::Fixup(int seq_num)
    {

        if ((unsigned char)m_buf_hex.at(0) == tello_protocol::START_OF_PACKET)
        {
            auto byte = le16(m_bufSize + 2);
            m_buf_hex.at(1) = byte.LeftNibble;
            m_buf_hex.at(2) = byte.RightNibble;
            m_buf_hex.at(1) = (m_buf_hex.at(1) << 3);
            m_buf_hex.at(3) = crc8(m_buf_hex.substr(0, 3)); //From original packet_processor [python]: buf[0:3]
            byte = le16(seq_num);
            m_buf_hex.at(7) = byte.LeftNibble;
            m_buf_hex.at(8) = byte.RightNibble;
            addInt16(crc16(m_buf_hex));
        }
    }
    void Packet::addInt16(const int16_t val)
    {

        AddByte(val);
        AddByte(val >> 8);
    }
    void Packet::AddByte(const unsigned char val)
    {
        /* To data section */
        m_buf_hex += (val & 0xff);
        m_bufSize++;
    }
    void Packet::AddInt(const int input)
    {
        unsigned char ch[4];
        memcpy(ch, &input, sizeof(int));
        //BigEndian {:x}{:x}{:x}{:x} :: , ch[3], ch[2], ch[1], ch[0])
        AddByte(ch[0]);
        AddByte(ch[1]);
    }
    void Packet::AddFloat(const float input)
    {
        unsigned char ch[4];
        memcpy(ch, &input, sizeof(float));
        //{:x}{:x}{:x}{:x}", ch[3], ch[2], ch[1], ch[0])

        AddByte(ch[0]);
        AddByte(ch[1]);
        AddByte(ch[2]);
        AddByte(ch[3]);
    }
    void Packet::AddTime()
    {
        using namespace std::chrono;
        auto now_hr = time_point_cast<hours>(system_clock::now());
        auto now_min = time_point_cast<minutes>(system_clock::now());
        auto now_sec = time_point_cast<seconds>(system_clock::now());
        auto now_ms = time_point_cast<milliseconds>(system_clock::now());

        auto temp_hr = short(now_hr.time_since_epoch().count() % 24);
        auto temp_min = short(now_min.time_since_epoch().count() % 60);
        auto temp_sec = short(now_sec.time_since_epoch().count() % 60);
        auto temp_ms = short(now_ms.time_since_epoch().count() % 1000);

        /**
         * @brief GMP offset
         * @todo remove to define or a parameter.
         * 
         */
        short GMP_offset = 2; 
        addInt16(temp_hr + GMP_offset);
        addInt16(temp_min);
        addInt16(temp_sec);
        addInt16(temp_ms); // (int(time.microsecond/1000) & 0xff)
        addInt16(0);       // ((int(time.microsecond/1000) >> 8) & 0xff)
    }
}; // namespace tello_protocol
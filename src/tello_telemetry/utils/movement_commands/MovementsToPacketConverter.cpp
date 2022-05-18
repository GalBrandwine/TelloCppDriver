#include "MovementsToPacketConverter.hpp"
namespace tello_protocol
{
    /** fill_data
     * There are 4 sticks, and 1 bit for speedMode.
     * Neutral stick value is at 1024.
     * Stick values can vary from -1024 ~ +1023
     * They are stored in signed short int.
     * 
     * Each axe being packed into 11 bits:
     * 11 bits (-1024 ~ +1023) x 4 axis = 44 bits
     * fast_mode takes 1 bit        
     * 44 bits will be packed in to 6 bytes (48 bits)
     *  axis5      axis4      axis3      axis2      axis1
     *      |          |          |          |          |
     *      |   4      |  3       | 2        |1         0
     * 98765432109876543210987654321098765432109876543210
     *  |       |       |       |       |       |       |
     *      byte5   byte4   byte3   byte2   byte1   byte0
     * 
     * **/
    void MovementsToPacketConverter::fill_data(Packet &pktOut)
    {
        auto axe0 = ((unsigned long long int)m_axis[0] << SHIFT_LEFT_AMOUNT * 0);
        auto axe1 = ((unsigned long long int)m_axis[1] << SHIFT_LEFT_AMOUNT * 1);
        auto axe2 = ((unsigned long long int)m_axis[2] << SHIFT_LEFT_AMOUNT * 2);
        auto axe3 = ((unsigned long long int)m_axis[3] << SHIFT_LEFT_AMOUNT * 3);
        auto axe4 = ((unsigned long long int)m_axis[4] << SHIFT_LEFT_AMOUNT * 4);

        auto packed_data = axe0 | axe1 | axe2 | axe3 | axe4;

        std::string s = std::to_string(packed_data);

        char pchar[sizeof(unsigned long long)];
        std::memcpy(pchar, &packed_data, sizeof(packed_data));
        // std::cout << "data[int]=" << packed_data << "\n";
        // std::cout << "data[hex]=" << std::hex << packed_data << "\n";
        // std::cout << "data[char*]=" << pchar << "\n";

        pktOut.AddByte(pchar[0]);
        pktOut.AddByte(pchar[1]);
        pktOut.AddByte(pchar[2]);
        pktOut.AddByte(pchar[3]);
        pktOut.AddByte(pchar[4]);
        pktOut.AddByte(pchar[5]);
        pktOut.AddTime();
        pktOut.Fixup();

        // auto date = pktOut.GetBuffer();
        /* 
        packed = axis1 | (axis2 << 11) | (axis3 << 22) | (axis4 << 33) | (axis5 << 44)
        packed_bytes = struct.pack('<Q', packed)
        pkt.add_byte(byte(packed_bytes[0]))
        pkt.add_byte(byte(packed_bytes[1]))
        pkt.add_byte(byte(packed_bytes[2]))
        pkt.add_byte(byte(packed_bytes[3]))
        pkt.add_byte(byte(packed_bytes[4]))
        pkt.add_byte(byte(packed_bytes[5]))
        pkt.add_time()
        pkt.fixup()

        # Tello: 17:54:20.667:  Info: stick command: fast=0000 yaw=0400 thr=0400 pit=03be rol=0400
        #   b'\x00\xf4\x1d\x00\x01\x08\x00\x00'
        #   0x801001df400
        # packed
        # 8800389952512
        # packed_bytes
        # b'\x00\xf4\x1d\x00\x01\x08\x00\x00'
        # packet.getbuffer = bytearray(b'\xcc\xd8\x00S`P\x00\x00\x00    \x00\xf4\x1d\x00\x01\x08\   x11\x005\x00-\x00\xbd\x00\x03\x00\x8dV')
        */
    }

    /** 
     * 0 axis1 = int(1024 + 660.0 * self.right_x) & 0x7ff <------- Force signed.
     * 1 axis2 = int(1024 + 660.0 * self.right_y) & 0x7ff
     * 2 axis3 = int(1024 + 660.0 * self.left_y) & 0x7ff
     * 3 axis4 = int(1024 + 660.0 * self.left_x) & 0x7ff
     * 4 axis5 = int(self.fast_mode) & 0x01  
    **/
    void MovementsToPacketConverter::fill_axis(const std::unordered_map<Sticks, float> &stick_movements_dict)
    {
        m_axis[0] = STICK_NEUTRAL_CONVERTION_VALUE + 660.0f * stick_movements_dict.find(Sticks::RIGHT_X)->second;
        m_axis[1] = STICK_NEUTRAL_CONVERTION_VALUE + 660.0f * stick_movements_dict.find(Sticks::RIGHT_Y)->second;
        m_axis[2] = STICK_NEUTRAL_CONVERTION_VALUE + 660.0f * stick_movements_dict.find(Sticks::LEFT_Y)->second;
        m_axis[3] = STICK_NEUTRAL_CONVERTION_VALUE + 660.0f * stick_movements_dict.find(Sticks::LEFT_X)->second;
        m_axis[4] = bool(stick_movements_dict.find(Sticks::FAST_MODE)->second);

        // std::cout << "Stick commands [int]:"
        //           << " fast=" << m_axis[4]
        //           << " yaw=" << m_axis[3]
        //           << " thr=" << m_axis[2]
        //           << " pit=" << m_axis[1]
        //           << " rol=" << m_axis[0]
        //           << "\n";

        // std::cout << "Stick commands [hex]:"
        //           << " fast=" << std::hex << m_axis[4]
        //           << " yaw=" << std::hex << m_axis[3]
        //           << " thr=" << std::hex << m_axis[2]
        //           << " pit=" << std::hex << m_axis[1]
        //           << " rol=" << std::hex << m_axis[0]
        //           << "\n";
    }

    /**
     * @brief Convert stick movements into a packet according to tello_protocol. 
     * Assumes pktOut is empty. 
     * 
     * @param[in] stick_movements_dict 
     * @param[out] pktOut 
     * @return true 
     * @return false 
     */
    bool MovementsToPacketConverter::Convert(const std::unordered_map<Sticks, float> &stick_movements_dict, Packet &pktOut)
    {

        if (pktOut.GetData().size() > 0)
            return false;

        fill_axis(stick_movements_dict);
        fill_data(pktOut);

        /*
        log.info("stick command: %s" % byte_to_hexstring(pkt.get_buffer()))
         */
        return true;
    }
    MovementsToPacketConverter::MovementsToPacketConverter(/* args */)
    {
    }

    MovementsToPacketConverter::~MovementsToPacketConverter()
    {
    }
} // namespace tello_protocol

#pragma once
#include <unordered_map>
#include <bitset>

#include<algorithm> //must include
#include "MovementCommandsManager.hpp"
#include "protocol.hpp"

#define SHIFT_LEFT_AMOUNT 11
/** Some initial thoughts:
 * This class could be wraped with IConverter interface . 
 * But since its not likely to have more packages that need to be converted. An interface is overkill.
 */

namespace tello_protocol
{
    class MovementsToPacketConverter
    {
    public:
        bool Convert(const std::unordered_map<Sticks, float> &, Packet &);
        MovementsToPacketConverter(/* args */);
        ~MovementsToPacketConverter();

    private:
        void fill_data(Packet &);
        void fill_axis(const std::unordered_map<Sticks, float> &);
        signed char m_packed_data[6]{0};

        // std::fill_n(axis, 5, 1024); I could fill it like this, But its only 5 elemts. So..
        signed short m_axis[5]{STICK_NEUTRAL_CONVERTION_VALUE,
                               STICK_NEUTRAL_CONVERTION_VALUE,
                               STICK_NEUTRAL_CONVERTION_VALUE,
                               STICK_NEUTRAL_CONVERTION_VALUE,
                               STICK_NEUTRAL_CONVERTION_VALUE};
    };
} // namespace tello_protocol

#pragma once
#include <unordered_map>
namespace tello_protocol
{
    namespace FlyModes
    {
        enum FlyModes
        {
            UN_DOC0 = 0,
            ERROR = 1,
            HOLDING_POSITION = 6,
            MANUAL = 10,
            TAKING_OFF = 11,
            LANDING = 12,
            FAST_MODE = 31,
            ACROBAT_MODE = 34
        };
        /**
         * @brief Fly mode names.
         * Keep track of all documented fly_modes.
         * 
         * @return std::unordered_map<int, std::string> 
         */
        static std::unordered_map<int, const std::string> FlyModeNameMap(
            {
                {1, "ERROR"},
                {6, "HOLDING_POSITION"},
                {10, "MANUAL"},
                {11, "TAKING_OFF"},
                {12, "LANDING"},
                {31, "FAST_MODE"},
                {34, "ACROBAT_MODE"},
            });

        /**
         * @brief Get the name of current fly mode.
         * This function relay on the fly modes that I understood.
         * 
         * @note This function returns NO_SUCH_FLY_MODE if fly_mode not found.
         * 
         * @param fly_mode - mode received from TelloTelemetry.
         * @return const std::string Mode name.

         */
        static const std::string ToName(int fly_mode)
        {
            auto fly_mode_name_it = FlyModeNameMap.find(fly_mode);
            if (fly_mode_name_it == FlyModeNameMap.end())
            {
                return "NO_SUCH_FLY_MODE";
            }
            return fly_mode_name_it->second;
        };
    } // namespace FlyModes

} // namespace tello_protocol
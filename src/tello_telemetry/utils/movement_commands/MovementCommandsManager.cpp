#include "MovementCommandsManager.hpp"

namespace tello_protocol
{
    const std::unordered_map<Sticks, float> &MovementCommandsManager::GetStickMovements() const
    {
        return m_SticksDict;
    }

    bool MovementCommandsManager::SetFastMode(bool mode)
    {
        m_SticksDict[Sticks::FAST_MODE] = mode;
        return true;
    }

    void MovementCommandsManager::SetAttitude(AttitudeMovements movement, float amount)
    {
        auto fixed_amount = tello_protocol::FixRange(amount);
        switch (movement)
        {
        case AttitudeMovements::THROTTLE:
            m_SticksDict[tello_protocol::Sticks::LEFT_Y] = fixed_amount;
            break;
        case AttitudeMovements::YAW:
            m_SticksDict[tello_protocol::Sticks::LEFT_X] = fixed_amount;
            break;
        case AttitudeMovements::PITCH:
            m_SticksDict[tello_protocol::Sticks::RIGHT_Y] = fixed_amount;
            break;
        case AttitudeMovements::ROLL:
            m_SticksDict[tello_protocol::Sticks::RIGHT_X] = fixed_amount;
            break;
        default:
            break;
        }
    }

    bool MovementCommandsManager::SetMovementCommand(Movements movement, float amount)
    {

        if (amount < 0 || amount > 100)
            return false;

        std::cout << __PRETTY_FUNCTION__ << movement_to_string(movement).c_str() << "\n";
        switch (movement)
        {
        case Movements::DOWN:
            set_down(amount);
            return true;
        case Movements::UP:
            set_up(amount);
            return true;
        case Movements::RIGHT:
            set_right(amount);
            return true;
        case Movements::LEFT:
            set_left(amount);
            return true;
        case Movements::BACKWARD:
            set_backward(amount);
            return true;
        case Movements::FORWARD:
            set_forward(amount);
            return true;
        case Movements::CLOCKWISE:
            set_clockwise(amount);
            return true;
        case Movements::COUNTER_CLOCKWISE:
            set_counter_clockwise(amount);
            return true;
        default:
            return false;
        }
    }

    void MovementCommandsManager::set_down(float amount)
    {
        std::cout << __PRETTY_FUNCTION__ << " (" << amount << ")\n";
        m_SticksDict[Sticks::LEFT_Y] = amount / 100 * -1;
    }
    void MovementCommandsManager::set_up(float amount)
    {
        std::cout << __PRETTY_FUNCTION__ << " (" << amount << ")\n";
        m_SticksDict[Sticks::LEFT_Y] = amount / 100;
    }
    void MovementCommandsManager::set_right(float amount)
    {
        std::cout << __PRETTY_FUNCTION__ << " (" << amount << ")\n";
        m_SticksDict[Sticks::RIGHT_X] = amount / 100;
    }
    void MovementCommandsManager::set_left(float amount)
    {
        std::cout << __PRETTY_FUNCTION__ << " (" << amount << ")\n";
        m_SticksDict[Sticks::RIGHT_X] = amount / 100 * -1;
    }
    void MovementCommandsManager::set_counter_clockwise(float amount)
    {
        std::cout << __PRETTY_FUNCTION__ << " (" << amount << ")\n";
        m_SticksDict[Sticks::LEFT_X] = amount / 100 * -1;
    }
    void MovementCommandsManager::set_clockwise(float amount)
    {
        std::cout << __PRETTY_FUNCTION__ << " (" << amount << ")\n";
        m_SticksDict[Sticks::LEFT_X] = amount / 100;
    }
    void MovementCommandsManager::set_forward(float amount)
    {
        std::cout << __PRETTY_FUNCTION__ << " (" << amount << ")\n";
        m_SticksDict[Sticks::RIGHT_Y] = amount / 100;
    }

    void MovementCommandsManager::set_backward(float amount)
    {
        std::cout << __PRETTY_FUNCTION__ << " (" << amount << ")\n";
        m_SticksDict[Sticks::RIGHT_Y] = amount / 100 * -1;
    }

    MovementCommandsManager::MovementCommandsManager()
    {
        std::cout << __PRETTY_FUNCTION__ << "::" << __LINE__ << "::Initiated";
        for (int possible_stick_movement = Sticks::RIGHT_Y; possible_stick_movement != Sticks::FAST_MODE; possible_stick_movement++)
        {
            Sticks movement = static_cast<Sticks>(possible_stick_movement);
            m_SticksDict[movement] = STICK_NEUTRAL_VALUE;
        }

        m_SticksDict[Sticks::FAST_MODE] = false;
    }

    MovementCommandsManager::~MovementCommandsManager()
    {
    }
} // namespace tello_protocol

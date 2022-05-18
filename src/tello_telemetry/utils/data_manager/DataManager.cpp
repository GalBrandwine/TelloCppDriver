#include "DataManager.hpp"
namespace tello_protocol
{
    const FlightDataStruct &DataManager::GetFlightData() const
    {
        return m_flightData;
    }
    ConnectionInformation &DataManager::GetConnectionInformation()
    {
        return m_connection_information;
    }

    void DataManager::SetPowerOnTimer(const tello_protocol::PowerOnTimerInfo &power_on_timer)
    {
        m_flightData.power_on_timer_info = power_on_timer;
        Notify(OBSERVERS::FLIGHT_DATA_MSG);
    }

    void DataManager::SetFlightData(const std::shared_ptr<IFlightDataGetter> flight_data_processor)
    {
        try
        {
            if (flight_data_processor != nullptr)
            {
                flight_data_processor->GetFlightData(m_flightData);
                Notify(OBSERVERS::FLIGHT_DATA_MSG);
            }
        }
        catch (const std::exception &e)
        {
            m_logger->error("Couldn't get flight data: {}", e.what());
        }
    }

    void DataManager::SetAttLimit(float att_limit)
    {
        m_logger->debug("SetAttLimit received: {}", std::to_string(att_limit));
        m_flightData.attitude_limit = att_limit;
        Notify(OBSERVERS::FLIGHT_DATA_MSG);
    }

    void DataManager::SetAltLimit(unsigned char alt_limit)
    {
        m_logger->debug("SetAltLimit received: {}", std::to_string(alt_limit));
        m_flightData.alt_limit = alt_limit;
        Notify(OBSERVERS::FLIGHT_DATA_MSG);
    }

    void DataManager::SetLowBatThreshLimit(unsigned char low_bat_thresh)
    {
        m_logger->debug("SetLowBatThreshLimit received: {}", std::to_string(low_bat_thresh));
        m_flightData.low_battery_threshold = low_bat_thresh;
        Notify(OBSERVERS::FLIGHT_DATA_MSG);
    }

    void DataManager::SetWifiMsg(const unsigned char &wifi_strength)
    {
        m_logger->debug("SetWifiMsg received: {}", wifi_strength);
        m_flightData.wifi_strength = wifi_strength;
        Notify(OBSERVERS::FLIGHT_DATA_MSG);
    }

    void DataManager::SetConnReqAck()
    {
        m_logger->debug("Connection request acknowledged!");
        m_connection_information.IsConnected = true;
    }

    void DataManager::SetLogData(const std::shared_ptr<ILogDataGetter> log_data_processor)
    {
        /**
         * @brief Upon SetLogData, also Notify all attached to CONN_ACK_MSG.
         * Sometimes conenction to the drone might be already established to the drone wont sent CONN_ACK_MSG.
         * We need to trigger it: SetConnAcked();
         * 
         */
        if (!m_connection_information.IsConnected)
        {
            SetConnReqAck();
        }

        if (log_data_processor == nullptr)
        {
            m_logger->warn("Received empty log_data_processor. returning");
            return;
        }

        if (log_data_processor->GetLogMvo().GetPosVelIfUpdated(m_posVel))
        {
            m_logger->debug("pose {} {} {}", m_posVel.pose.x, m_posVel.pose.y, m_posVel.pose.z);
            Notify(OBSERVERS::POSITION_VELOCITY_LOG);
        }
        if (log_data_processor->GetLogImuAtti().GetImuAttiIfUpdated(m_imuAtti))
        {
            m_logger->debug("acc {} {} {}", m_imuAtti.acc.x, m_imuAtti.acc.y, m_imuAtti.acc.z);
            m_logger->debug("gyro {} {} {}", m_imuAtti.gyro.x, m_imuAtti.gyro.y, m_imuAtti.gyro.z);
            m_logger->debug("vg {} {} {}", m_imuAtti.vg.x, m_imuAtti.vg.y, m_imuAtti.vg.z);
            m_logger->debug("quat {} {} {} {}", m_imuAtti.quat.x, m_imuAtti.quat.y, m_imuAtti.quat.z, m_imuAtti.quat.w);
            Notify(OBSERVERS::IMU_ATTITUDE_LOG);
        }
    }

    void DataManager::SetLogID(const unsigned short id)
    {
        std::memcpy(&m_log_header_information.LogId, &id, sizeof(unsigned short));
        m_logger->debug("LogId: {};", id);
        Notify(ACK_LOG_HEADER);
    }

    void DataManager::SetBuildDate(const std::vector<unsigned char> &build_date)
    {
        m_log_header_information.BuildDate = build_date;

        std::stringstream ss;
        for (auto &character : m_log_header_information.BuildDate)
        {
            ss << character;
        }
        m_logger->info("BUILD date: {};", ss.str());
    }

    void DataManager::SetDJILogVersion(const std::vector<unsigned char> &log_version)
    {
        m_log_header_information.DJILogVersion = log_version;

        std::stringstream ss;
        for (auto &character : m_log_header_information.DJILogVersion)
        {
            ss << character;
        }
        m_logger->info("DJI LOG VERSION: {};", ss.str());
        Notify(DJI_LOG_VERSION);
    }

    void DataManager::notify_imu_attitude_received(IObserver *observer)
    {
        try
        {
            auto casted = dynamic_cast<IImuAttitudeObserver *>(observer);
            if (casted == nullptr)
            {
                m_logger->error("Attached Observer does not implement ImuAttitudeObserver. \n \
            Observer Update() declartation should look like: \n \
            void ImuAttitudeObserver::Update(const tello_protocol::ImuAttitudeData &imu_attitude_data)");
                return;
            }

            casted->Update(m_imuAtti);
        }
        catch (const std::exception &e)
        {
            m_logger->error("Caught dynamic_cast exception: {}", e.what());
        }
    }
    void DataManager::notify_flight_data_received(IObserver *observer)
    {
        try
        {
            auto casted = dynamic_cast<IFlightDataObserver *>(observer);
            if (casted == nullptr)
            {
                m_logger->error("Attached Observer does not implement IFlightDataObserver. \n \
            Observer Update() declartation should look like: \n \
            void IFlightDataObserver::Update(const tello_protocol::FlightDataStruct &flight_data)");
                return;
            }

            casted->Update(m_flightData);
        }
        catch (const std::exception &e)
        {
            m_logger->error("Caught dynamic_cast exception: {}", e.what());
        }
    }

    void DataManager::notify_position_velocity(IObserver *observer)
    {
        try
        {
            auto casted = dynamic_cast<IPositionVelocityObserver *>(observer);
            if (casted == nullptr)
            {
                m_logger->error("Attached Observer does not implement IPositionVelocityObserver. \n \
            Observer Update() declartation should look like: \n \
            void IPositionVelocityObserver::Update(const tello_protocol::PoseVelData &pos_vel)");
                return;
            }
            casted->Update(m_posVel);
        }
        catch (const std::exception &e)
        {
            m_logger->error("Caught dynamic_cast exception: {}", e.what());
        }
    }

    void DataManager::notify_log_req_received(IObserver *observer)
    {
        try
        {
            observer->Update(m_log_header_information.LogId);
        }
        catch (const std::exception &e)
        {
            m_logger->error("{}::Could not update observer: {}", __PRETTY_FUNCTION__, e.what());
        }
    }

    void DataManager::notify_dji_log_version(IObserver *observer)
    {

        std::vector<unsigned char> data;
        try
        {
            data = m_log_header_information.DJILogVersion;
            data.insert(data.end(), m_log_header_information.BuildDate.begin(), m_log_header_information.BuildDate.end());
            observer->Update(data);
        }
        catch (const std::exception &e)
        {
            std::string err(__PRETTY_FUNCTION__);
            m_logger->error("Caught dynamic_cast exception: " + err + "::" + e.what());
        }
    }

    void DataManager::Notify(const OBSERVERS observer_type)
    {
        if (m_attached_dict[observer_type].empty())
        {
            m_logger->info("No one is listening. Returning");
            return;
        }

        howManyObservers(observer_type);

        for (auto observer : m_attached_dict[observer_type])
        {

            if (observer == nullptr)
            {
                return;
            }

            switch (observer_type)
            {
            case OBSERVERS::ACK_LOG_HEADER:
                notify_log_req_received(observer);
                break;
            case OBSERVERS::DJI_LOG_VERSION:
                notify_dji_log_version(observer);
                break;
            case OBSERVERS::POSITION_VELOCITY_LOG:
                notify_position_velocity(observer);
                break;
            case OBSERVERS::FLIGHT_DATA_MSG:
                notify_flight_data_received(observer);
                break;
            case OBSERVERS::IMU_ATTITUDE_LOG:
                notify_imu_attitude_received(observer);
                break;
            default:
                break;
            }
        }
    }

    void DataManager::Attach(const OBSERVERS observer_type, IObserver *observer)
    {
        m_attached_dict[observer_type].push_back(observer);
    }
    void DataManager::Attach(const OBSERVERS observer_type, IPositionVelocityObserver *observer)
    {
        m_attached_dict[observer_type].push_back(observer);
    }
    void DataManager::Attach(const OBSERVERS observer_type, IFlightDataObserver *observer)
    {
        m_attached_dict[observer_type].push_back(observer);
    }
    void DataManager::Attach(const OBSERVERS observer_type, IImuAttitudeObserver *observer)
    {
        m_attached_dict[observer_type].push_back(observer);
    }
    // void DataManager::Attach(const OBSERVERS observer_type, IConnectionEstablishedObserver *observer)
    // {
    //     m_attached_dict[observer_type].push_back(observer);
    // }
    void DataManager::howManyObservers(const OBSERVERS observer_type)
    {
        m_logger->debug("There are " + std::to_string(m_attached_dict[observer_type].size()) + " observers attached to: " + observer_name(observer_type) + ".\n");
    }

    DataManager::DataManager(std::shared_ptr<spdlog::logger> logger, spdlog::level::level_enum lvl)
        : m_logger(logger)
    {
        m_log_header_information.LogId.reserve(sizeof(unsigned short));
    }

    DataManager::~DataManager()
    {
        m_logger->debug(m_logger->name() + " Removing observers");

        for (int observer_type_index = OBSERVERS::ACK_LOG_HEADER; observer_type_index != OBSERVERS::FLIGHT_DATA_MSG; observer_type_index++)
        {
            OBSERVERS observer_type = static_cast<OBSERVERS>(observer_type_index);

            while (!m_attached_dict[observer_type].empty())
            {
                m_attached_dict[observer_type].pop_back();
            }
        }

        m_logger->debug(m_logger->name() + " Destructed!");
    }
} // namespace tello_protocol

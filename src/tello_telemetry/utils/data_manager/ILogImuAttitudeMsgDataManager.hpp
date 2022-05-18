#pragma once
#include <vector>
#include "log_data/LogData.hpp"
class ILogImuAttitudeMsgDataManager
{
public:
    virtual void SetLogImuAtitudeData(const std::shared_ptr<ILogDataGetter> log_data_processor) = 0;
    virtual ~ILogImuAttitudeMsgDataManager(){};
};

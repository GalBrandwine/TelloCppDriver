#pragma once
#include <vector>
#include "log_data/LogData.hpp"
class ILogDataMsgDataManager
{
public:
    virtual void SetLogData(const std::shared_ptr<ILogDataGetter> log_data_processor) = 0;
    virtual ~ILogDataMsgDataManager(){};
};

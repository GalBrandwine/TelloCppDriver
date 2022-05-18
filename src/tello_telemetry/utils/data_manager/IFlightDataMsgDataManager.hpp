#pragma once
#include <vector>
#include "flight_data/IFlightDataGetter.hpp"
class IFlightDataMsgDataManager
{
public:
    virtual void SetFlightData(const std::shared_ptr<IFlightDataGetter> flight_data_processor) = 0;
    virtual ~IFlightDataMsgDataManager(){};
};

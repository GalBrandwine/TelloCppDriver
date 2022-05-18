#pragma once

#include "LogNewMvoFeedback.hpp"
#include "LogNewImuAttiFeedback.hpp"
class ILogDataGetter
{
public:
    virtual tello_protocol::LogImuAtti &GetLogImuAtti() = 0;
    virtual tello_protocol::LogNewMvoFeedback &GetLogMvo() = 0;
    virtual ~ILogDataGetter(){};
};

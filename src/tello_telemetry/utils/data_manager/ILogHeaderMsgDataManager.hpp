#pragma once
#include <vector>
#include "utils/tello_observer/IDataMgrSubject.hpp"
class ILogHeaderMsgDataManager
{
public:
    virtual void SetLogID(const unsigned short id) = 0;
    virtual void SetBuildDate(const std::vector<unsigned char> &build_date) = 0;
    virtual void SetDJILogVersion(const std::vector<unsigned char> &log_version) = 0;
    virtual ~ILogHeaderMsgDataManager(){};
};

#pragma once
class ILowBatThreshMsgDataManager
{
public:
    virtual void SetLowBatThreshLimit(unsigned char low_bat_thresh) = 0;
    virtual ~ILowBatThreshMsgDataManager(){};
};

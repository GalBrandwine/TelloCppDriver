#pragma once
class IAttLimitMsgDataManager
{
public:
    virtual void SetAttLimit(float att_limit) = 0;
    virtual ~IAttLimitMsgDataManager(){};
};
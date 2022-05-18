#pragma once
class IAltLimitMsgDataManager
{
public:
    virtual void SetAltLimit(unsigned char alt_limit) = 0;
    virtual ~IAltLimitMsgDataManager(){};
};

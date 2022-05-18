#pragma once
class IConnAckMsgDataManager
{
public:
    virtual void SetConnReqAck() = 0;
    virtual ~IConnAckMsgDataManager(){};
};

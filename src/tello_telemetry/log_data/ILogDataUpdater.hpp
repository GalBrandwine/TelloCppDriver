#pragma once

#include <vector>
class ILogDataUpdater
{
public:
    virtual void Update(const std::vector<unsigned char> &data) = 0;
    virtual ~ILogDataUpdater(){};
};

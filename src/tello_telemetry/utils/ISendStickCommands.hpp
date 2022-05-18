#pragma once
#include <string>
class ISendStickCommands
{
public:
    virtual bool SendStickCommands() = 0;
    virtual ~ISendStickCommands(){};
};

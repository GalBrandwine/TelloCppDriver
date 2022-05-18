#pragma once
#include <chrono>

struct Byte
{
    // Left most
    unsigned char LeftNibble;
    // Right most
    unsigned char RightNibble;

    const Byte &operator=(Byte a)
    {
        LeftNibble = a.LeftNibble;
        RightNibble = a.RightNibble;
        return *this;
    }
};

static const Byte le16(int val)
{
    return Byte{val & 0xff, (val >> 8) & 0xff};
}

static int16_t int16(unsigned char val0, unsigned char val1)
{
    if (val1 & 0xff != 0)
        return ((val0 & 0xff) | ((val1 & 0xff) << 8)) - 0x10000;
    else
    {
        return (val0 & 0xff) | ((val1 & 0xff) << 8);
    }
}

static const uint16_t uint16(unsigned char val0, unsigned char val1)
{
    return (val0 & 0xff) | ((val1 & 0xff) << 8);
}
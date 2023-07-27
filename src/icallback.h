#ifndef ICALLBACK_H
#define ICALLBACK_H

#include <QtCore>

class ICallback
{
public:
    virtual void BinaryPacket(const char* data, std::size_t size) = 0;
    virtual void TextPacket(const char* data, std::size_t size) = 0;
};

#endif // ICALLBACK_H

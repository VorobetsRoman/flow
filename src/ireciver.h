#ifndef IRECIVER_H
#define IRECIVER_H

#include <QtCore>
#include "icallback.h"

struct IReciver
{
    virtual void Recive(const char* data, std::size_t size) = 0;
};

class Reciver : public IReciver
{
public:
    Reciver(ICallback *callback);
    ~Reciver();
        // IReciver interface
    void Recive(const char *data, std::size_t size);


private:
    ICallback *mp_callback {nullptr};

    static const u_int8_t mc_binaryId = 0x24;
    static constexpr std::size_t mc_idSize = sizeof(mc_binaryId);
    static const std::size_t mc_binaryDataSize = 4;
    static constexpr std::size_t mc_binaryHeaderSize = mc_idSize + mc_binaryDataSize;
    const char *m_finish = "\r\n\r\n";

    char *m_cache {nullptr};
    std::size_t m_cacheSize {0};

    void m_processBlock(
        const char* data
        , std::size_t size
    );

    int m_processBinary(
        const char *data
        , std::size_t size
    );
    int m_processText(
        const char *data
        , std::size_t size
    );

    bool m_isBinary(const char *data);

    void m_increaseCache(std::size_t appendSize);
};

#endif // IRECIVER_H

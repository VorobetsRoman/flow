#include "ireciver.h"
#include <cstring>

Reciver::Reciver(ICallback *callback)
    : mp_callback(callback)
{

}

Reciver::~Reciver()
{
    if (m_cache) {
        delete [] m_cache;
    }
}

void Reciver::Recive(
    const char *data
    , std::size_t size
) {
    if (m_cache) {
        m_increaseCache(size);

        memcpy(m_cache + m_cacheSize, data, size);

        m_processBlock(m_cache, m_cacheSize);
    } else {
        m_processBlock(data, size);
    }
}

void Reciver::m_processBlock(
    const char *data
    , std::size_t size
) {
    int readed = 0;
    if (m_isBinary(data)) {
        if (size > mc_binaryHeaderSize) {
            readed = m_processBinary(data, size);
        }
    } else {
        readed = m_processText(data, size);
    }

    if (readed == 0) { //блок в потоке не закончился
        if (!m_cache) { //если контейнер не существует, т.е. читали из потока
            m_cache = new char [size];
            m_cacheSize = size;
            memcpy(m_cache, data, size);
        } // иначе ждём новых данных
    } else {
        size -= readed;
        if (size > 0) { //в потоке остались данные
            if (m_cache) {
                memmove(m_cache, m_cache + readed, size);
                m_cacheSize -= size;
                m_processBlock(m_cache, m_cacheSize);
            } else {
                m_processBlock(data + readed, size);
            }
        } else {//конец блока совпал с концом потока
            if (m_cache) {
                delete [] m_cache;
                m_cache = nullptr;
                m_cacheSize = 0;
            }
        }
    }
}

int Reciver::m_processBinary(
    const char *data
    , std::size_t size
) {
    const u_int32_t *datasize = reinterpret_cast
        <const u_int32_t*>
        (data + mc_idSize);
    const auto binaryPackageSize = mc_binaryHeaderSize + *datasize;
    if (size >= binaryPackageSize) {
        mp_callback->BinaryPacket(data + mc_binaryHeaderSize, *datasize);
        return binaryPackageSize;
    } else {
        return 0;
    }
}

int Reciver::m_processText(
    const char *data
    , std::size_t size
) {
    const char* foundPos = strstr(data, m_finish);

    if (foundPos != nullptr) {
        auto strlen = foundPos - data + mc_binaryDataSize;
        mp_callback->TextPacket(foundPos, strlen);
        return strlen;
    } else {
        return 0;
    }
}

bool Reciver::m_isBinary(const char *data)
{
    const u_int8_t *dataId = reinterpret_cast<const u_int8_t*>(data);
    return *dataId == mc_binaryId;
}

void Reciver::m_increaseCache(std::size_t appendSize)
{
    auto temp = m_cache;
    m_cache = new char [m_cacheSize + appendSize];
    memcpy(m_cache, temp, m_cacheSize);
    m_cacheSize += appendSize;
    delete [] temp;
}

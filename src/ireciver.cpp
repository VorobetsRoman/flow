#include "ireciver.h"
#include <cstring>

Reciver::Reciver(ICallback *callback)
    : mp_callback(callback)
{

}

void Reciver::Recive(
    const char *data
    , std::size_t size
) {
    if (m_cache) {
        memcpy(m_cache + m_cacheSize, data, size);
        m_cacheSize += size;
        m_procesData(m_cache, m_cacheSize);
    } else {
        m_procesData(data, size);
    }
}

void Reciver::m_procesData(
    const char *data
    , std::size_t size
) {
    int readed = 0;
    if (m_isBinary(data)) {
        if(size > 4 + 1) {
            readed = m_processBinary(data, size);
        }
    } else {
        readed = m_processText(data, size);
    }
    if (readed == 0) { //данных не хватило
        if (!m_cache) { //если контейнер не существует
            m_cache = new char [size];
            m_cacheSize = size;
        } // иначе ждём новых данных
    } else {
        size -= readed;
        if (size > 0) { //остались данные
            if (m_cache) {
                memmove(m_cache, m_cache + readed, size);
                m_cacheSize -= size;
            }
            m_procesData(data + readed, size);
        } else {
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
        (data + 1);
    const auto binaryPackageSize = *datasize + 4 + 1;
    if (size >= binaryPackageSize) {
        mp_callback->BinaryPacket(data + 1 + 4, *datasize);
        return binaryPackageSize;
    }
    return 0;
}

int Reciver::m_processText(
    const char *data
    , std::size_t size
) {
    const char* foundPos = strstr(data, m_finish);

    if (foundPos != nullptr) {
        auto strlen = foundPos - data + 4;
        mp_callback->TextPacket(foundPos, strlen);
        return strlen;
    }
    return 0;
}

bool Reciver::m_isBinary(const char *data)
{
    const u_int8_t *dataId = reinterpret_cast<const u_int8_t*>(data);
    return *dataId == mc_binaryId;
}

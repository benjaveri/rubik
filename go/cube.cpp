#include "cube.h"

qword Cube::computeHash() const {
    // from crc64.cpp
    uint64_t crc64(uint64_t crc, const unsigned char *s, uint64_t l);

    // compute hash of this object
    return crc64(0,(const unsigned char*)this,sizeof(*this));
}

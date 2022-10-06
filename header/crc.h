
#ifndef _CRC_H
#define _CRC_H

#include <stdint.h>

class Crc32
{
public:

	static uint32_t GetCrc32(void *pData, const uint32_t length);

};

#endif

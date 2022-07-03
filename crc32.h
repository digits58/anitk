#ifndef CRC__H
#define CRC__H

#include <vector>
#include <string>

uint32_t crc32update(uint8_t byte, uint32_t crc);
uint32_t crc32buf(const std::vector<uint8_t> &buf);
uint32_t crc32file(const std::string &name);

#endif

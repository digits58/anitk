#ifndef CRC__H
#define CRC__H

#include <stdlib.h>
#include <optional>
#include <utility>
#include <vector>
#include <string>

uint32_t crc32update(uint8_t byte, uint32_t crc);
std::optional<std::pair<uint32_t, long>> crc32file(const std::string &name);
uint32_t crc32buf(std::vector<uint8_t> buf);

#endif

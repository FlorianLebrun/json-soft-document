
#include <stdint.h>

namespace SoftDoc_Utils {
  static const void* EndOfPtr = (void*)-1;
  struct base_interface {};

  uint32_t hash_utf8_crc31(uint32_t crc, const void *buf, size_t size);
  uint32_t hash_utf8_icrc31(uint32_t crc, const void *buf, size_t size);
  uint32_t hash_murmur3_31(const uint8_t* key, size_t len, uint32_t seed);
  uint32_t hash_jenkins_31(const uint8_t* key, size_t length);
}
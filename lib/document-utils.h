
uint32_t utf8_crc32(uint32_t crc, const void *buf, size_t size);
uint32_t murmur3_32(const uint8_t* key, size_t len, uint32_t seed);
uint32_t jenkins(const uint8_t* key, size_t length);

inline int align8(int offset) {return ((-offset)&7)+offset;}

inline uint32_t hash_symbol_l(const char* symbol, int len) {
   //return utf8_crc32(0, symbol, len);
   return murmur3_32((uint8_t*)symbol, len, 0);
   //return jenkins((uint8_t*)symbol, len);
}

inline uint32_t hash_symbol(const char* symbol) {
   return hash_symbol_l(symbol, strlen(symbol));
}

int32_t compare_symbol(
   const char* buffer1, int32_t length1,
   const char* buffer2, int32_t length2);


uint32_t softdoc_hash_utf8_crc32(uint32_t crc, const void *buf, size_t size);
uint32_t softdoc_hash_murmur3_32(const uint8_t* key, size_t len, uint32_t seed);
uint32_t softdoc_hash_jenkins(const uint8_t* key, size_t length);


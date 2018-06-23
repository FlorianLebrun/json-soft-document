
template <bool caseInsensitive>
struct ObjectSymbolBase;

template <>
struct ObjectSymbolBase<true> : Object {
  static uint32_t hash_symbol(const char* str, int len) {
    return SoftDoc_Utils::hash_utf8_icrc31(0, str, len);
  }
protected:
  static int compare_bytes(const char* buffer1, const char* buffer2, int length) {
    return _strnicmp(buffer1, buffer2, length);
  }
};

template <>
struct ObjectSymbolBase<false> : Object {
  static uint32_t hash_symbol(const char* str, int len) {
    return SoftDoc_Utils::hash_murmur3_31((uint8_t*)str, len, 0);
  }
protected:
  static int compare_bytes(const char* buffer1, const char* buffer2, int length) {
    return memcmp(buffer1, buffer2, length);
  }
};

struct ObjectSymbol : ObjectSymbolBase<caseInsensitive> {
  uint32_t hash;
  uint32_t length;
  char buffer[1];

  inline bool equals(ObjectSymbol* other) {
    if (this->hash != other->hash) return false;
    if (this->length != other->length) return false;
    return !ObjectSymbolBase::compare_bytes(this->buffer, other->buffer, this->length);
  }
  inline bool equals(const char* buffer) {
    if (this->length != strlen(buffer)) return false;
    return !ObjectSymbolBase::compare_bytes(this->buffer, buffer, this->length);
  }
  inline int compare(uint32_t hash, const char* buffer, int length) {
    int32_t c = int32_t(this->hash - hash);
    if (!c) {
      c = int32_t(this->length - length);
      if (!c) {
        c = ObjectSymbolBase::compare_bytes(this->buffer, buffer, length);
      }
    }
    return c;
  }
  inline int compare(ObjectSymbol* other) {
    return this->compare(other->hash, other->buffer, other->length);
  }
};

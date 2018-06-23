
struct ObjectString : Object {
  uint32_t length;
  char buffer[1];

  bool equals(ObjectString* other) {
    if (this->length != other->length) return false;
    return !memcmp(this->buffer, other->buffer, this->length);
  }
  bool equals(const char* buffer, bool caseSensitive = true) {
    if (this->length != strlen(buffer)) return false;
    return !memcmp(this->buffer, buffer, this->length);
  }
};


struct ObjectString : Object {
   uint32_t length;
   uint8_t charset;
   char buffer[1];

   bool equals(ObjectString* other) {
      if(this->length != other->length) return false;
      return !memcmp(this->buffer, other->buffer, this->length);
   }
   bool equals(const char* buffer, bool caseSensitive = true) {
      if(this->length != strlen(buffer)) return false;
      return !memcmp(this->buffer, buffer, this->length);
   }
};

struct ObjectSymbol : Object {
   uint32_t hash;
   uint32_t length;
   uint8_t charset;
   char buffer[1];

   // Symbol is a string with: mandatory hash + customizable case sensitivity
   inline bool equals(ObjectSymbol* other) {
      if(this->hash != other->hash) return false;
      if(this->length != other->length) return false;
      return !memcmp(this->buffer, other->buffer, this->length);
   }
   inline bool equals(const char* buffer, bool caseSensitive = true) {
      if(this->length != strlen(buffer)) return false;
      return !memcmp(this->buffer, buffer, this->length);
   }
   inline int compare(uint32_t hash, const char* buffer, int length) {
      int32_t c = int32_t(this->hash - hash);
      if(!c) {
         c = int32_t(this->length - length);
         if(!c) c = strncmp(this->buffer, buffer, length);
      }
      return c;
   }
   inline int compare(ObjectSymbol* other) {
      return this->compare(other->hash, other->buffer, other->length);
   }
};


struct Document;
struct Object;
struct ObjectString;
struct ObjectSymbol;
struct ObjectArray;
struct ObjectMap;
struct ObjectExpression;
struct Property;
struct Item;
struct Value;

enum tCharsetType {
   ASCII_charset,
   UTF8_charset,
};

template <int x>
inline static intptr_t alignX(intptr_t offset) {
   return ((-offset)&(x-1))+offset;
}
inline static intptr_t alignPtr(intptr_t offset) {
   return ((-offset)&(sizeof(void*)-1))+offset;
}
inline static uint32_t hash_case_insensitive(const char* symbol, int len) {
   //return utf8_crc32(0, symbol, len);
   return softdoc_hash_murmur3_32((uint8_t*)symbol, len, 0);
   //return jenkins((uint8_t*)symbol, len);
}
inline static uint32_t hash_case_sensitive(const char* symbol, int len) {
   //return utf8_crc32(0, symbol, len);
   return softdoc_hash_murmur3_32((uint8_t*)symbol, len, 0);
   //return jenkins((uint8_t*)symbol, len);
}

struct Object {
};

struct ValueMetric {
   int depth;
   int width;
};

struct Value : IValue {
   Document* document;
   union {
      uint64_t _bits;
      bool _boolean;
      double _number;
      int64_t _integer;
      ObjectMap* _map;
      ObjectArray* _array;
      ObjectExpression* _expression;
      ObjectString* _string;
      ObjectSymbol* _symbol;
      Object* _object;
   };
   TypeID typeID;

   Value(Document* document);

   void undefine();
   void set(TypeID typeID);
   void set(bool x);
   void set(int64_t x);
   void set(double x);
   void set(const char* x, int len = -1);
   void set(Value* x);
   void set_symbol(const char* x, int len = -1);

   ObjectSymbol* className();

   Value& map(ObjectSymbol* key);
   Value& map(const char* key);
   Value* find(const char* key);

   Value& get(intptr_t index);
   Value& push_front();
   Value& push_back();

   bool equals(Value* other);
   void copy(Value* src);
   void copyMinimize(Value* src) ;
   void duplicate(Value* src);
   void subtract(Value* valueA, Value* valueB);

   int operator = (int8_t x) {this->set(int64_t(x));return x;}
   int operator = (int16_t x) {this->set(int64_t(x));return x;}
   int operator = (int32_t x) {this->set(int64_t(x));return x;}
   int operator = (int64_t x) {this->set(int64_t(x));return x;}
   int operator = (uint8_t x) {this->set(int64_t(x));return x;}
   int operator = (uint16_t x) {this->set(int64_t(x));return x;}
   int operator = (uint32_t x) {this->set(int64_t(x));return x;}
   int operator = (uint64_t x) {this->set(int64_t(x));return x;}
   float operator = (float x) {this->set(double(x));return x;}
   float operator = (double x) {this->set(double(x));return x;}
   char* operator = (char* x) {this->set(x);return x;}
   const char* operator = (const char* x) {this->set(x);return x;}
   const std::string& operator = (const std::string& x) {this->set(x.c_str());return x;}
   Value& operator = (Value& x) {this->set(&x);return x;}
   Value& operator [] (const char* x) {return this->map(x);}
   Value& operator [] (const std::string& x) {return this->map(x.c_str());}
   Value& operator [] (intptr_t x) {return this->get(x);}

   ValueMetric getMetric();
};

struct Item {
   Item* next;
   Value value;
};

struct Property {
   Property* next;
   ObjectSymbol* key;
   Value value;

   bool equals(Property* other) {
      if(!this->key->equals(other->key)) return false;
      return this->value.Value::equals(&other->value);
   }
};


template <class ValueImpl = Value>
struct array_iterator {
   Item* item;
   array_iterator(Value* value) {
      this->item = value->_array->firstItem;
   }
   ValueImpl* begin() {
      if(Item* item = this->item) return (ValueImpl*)&item->value;
      else return 0;
   }
   ValueImpl* next() {
      _ASSERT(this->item != 0);
      if(Item* item = this->item = this->item->next) return (ValueImpl*)&item->value;
      else return 0;
   }
};

template <class ValueImpl = Value>
struct map_iterator : ObjectMap::iterator {
   map_iterator(Value* value) : iterator(value->_map) {
   }
   ValueImpl* begin() {
      if(Property* prop = this->iterator::begin()) return (ValueImpl*)&prop->value;
      else return 0;
   }
   ValueImpl* next() {
      if(Property* prop = this->iterator::next()) return (ValueImpl*)&prop->value;
      else return 0;
   }
   ObjectSymbol* key() {
      _ASSERT(this->cproperty != 0);
      return this->cproperty->key;
   }
};

struct Allocator {
public:
   Allocator(int pageSize = DefaultPage_size);
   ~Allocator();
   __forceinline void* alloc(int size);
   __forceinline void free(void* ptr, int size);

private:
   struct tAllocPage {
      tAllocPage* next;
      int used;
      int size;
      uint8_t buffer[0];
   };

   tAllocPage* page;
   int pageSize;

   __declspec(noinline) tAllocPage* alloc_page(int size);
};

struct Document : IDocument, Allocator {

   tCharsetType charset;
   void** hashMapReserve[24];

   Document(tCharsetType charset = ASCII_charset, int pageSize = DefaultPage_size);

   __forceinline void** allocHashMap(int shift);
   __forceinline void freeHashMap(void** hashmap, int shift);

   __forceinline ObjectMap* createObjectMap();
   __forceinline ObjectArray* createObjectArray();
   __forceinline ObjectString* createObjectString(const char* str, int len);
   __forceinline ObjectSymbol* createObjectSymbol(uint32_t hash, const char* str, int len);
   __forceinline ObjectSymbol* createObjectSymbol(const char* str, int len = -1);

   __forceinline Property* createProperty(uint32_t hash, const char* str, int len);
   __forceinline Property* createProperty(ObjectSymbol* symbol);
   __forceinline Value* createValue();
   __forceinline Item* createItem();

#ifdef SoftDoc_TEMPLATE
   virtual void* allocValue(int head);
#else
   void* allocValue(int head);
#endif
};

#ifdef SoftDoc_TEMPLATE
template <class ValueImpl>
struct DocumentEx : Document {
   virtual void* allocValue(int head) {
      char* buffer = (char*)this->alloc(sizeof(ValueImpl)+head);
      new ((ValueImpl*)&buffer[head]) ValueImpl(this);
      return buffer;
   }
};
#endif


namespace SoftDocument {
  template <
    bool caseInsensitive = false,
    bool orderedMap = false,
  class IDocument = SoftDocument::base_interface,
  class IValue = SoftDocument::base_interface
  >
  struct Template {

    static const size_t DefaultPage_size = 4096;

    struct Document;
    struct Value;
    struct Symbol;

    typedef SoftDocument::TypeID TypeID;
    typedef SoftDocument::tCharsetType tCharsetType;

    typedef SoftDocument::Object Object;
    typedef SoftDocument::ObjectString ObjectString;
    typedef SoftDocument::ObjectArray<Document, Value> ObjectArray;
    typedef SoftDocument::Item<Document, Value> Item;
    typedef SoftDocument::ObjectMap<Document, Value, Symbol, orderedMap> ObjectMap;
    typedef SoftDocument::Property<Document, Value, Symbol> Property;
    typedef SoftDocument::ObjectExpression<Document, Value, ObjectString> ObjectExpression;

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
        Object* _object;
      };
      TypeID typeID;

      Value(Document* document);

      bool is_defined() { return this->typeID != TypeID::Undefined; }
      bool is_undefined() { return this->typeID == TypeID::Undefined; }
      bool is_boolean() { return this->typeID == TypeID::Boolean; }
      bool is_integer() { return this->typeID == TypeID::Integer; }
      bool is_number() { return this->typeID == TypeID::Number || this->typeID == TypeID::Integer; }
      bool is_map() { return this->typeID == TypeID::Map; }
      bool is_array() { return this->typeID == TypeID::Array; }
      bool is_string() { return this->typeID == TypeID::String; }
      bool is_symbol() { return this->typeID == TypeID::Symbol; }
      bool is_null() { return this->typeID == TypeID::Null; }

      void set(TypeID typeID);
      void set(bool x) { this->typeID = TypeID::Boolean; this->_boolean = x; }
      void set(uint8_t x) { this->set(int64_t(x)); }
      void set(uint16_t x) { this->set(int64_t(x)); }
      void set(uint32_t x) { this->set(int64_t(x)); }
      void set(uint64_t x) { this->set(int64_t(x)); }
      void set(int8_t x) { this->set(int64_t(x)); }
      void set(int16_t x) { this->set(int64_t(x)); }
      void set(int32_t x) { this->set(int64_t(x)); }
      void set(int64_t x) { this->typeID = TypeID::Integer; this->_integer = x; }
      void set(double x) { this->typeID = TypeID::Number; this->_number = x; }
      void set(std::string x);
      void set(Value* x);
      void set(const char* x, size_t len = 0);
      void set_symbol(const char* x, size_t len = 0);
      void set_undefined() { this->typeID = TypeID::Undefined;this->_bits = 0; }

      Value* className();

      Value& map(Symbol* key);
      Value& map(const char* key, size_t len = 0);
      Value* find(Symbol* key);
      Value* find(const char* key, size_t len = 0);
      Value* findAt(const char* key, size_t len = 0);

      Value& get(intptr_t index);
      Value& push_front();
      Value& push_back();

      bool equals(Value* other);
      void copy(Value* src);
      void copyMinimize(Value* src);
      void duplicate(Value* src);
      void subtract(Value* valueA, Value* valueB);

      int count();

      bool toBoolean(bool defaultValue = false) const;
      int64_t toInteger(int64_t defaultValue = 0) const;
      double toNumber(double defaultValue = 0) const;
      std::string toString(const char* defaultValue = "") const;

      operator bool() const { return this->toBoolean(); }
      operator int8_t() const { return this->toInteger(); }
      operator int16_t() const { return this->toInteger(); }
      operator int32_t() const { return this->toInteger(); }
      operator int64_t() const { return this->toInteger(); }
      operator uint8_t() const { return this->toInteger(); }
      operator uint16_t() const { return this->toInteger(); }
      operator uint32_t() const { return this->toInteger(); }
      operator uint64_t() const { return this->toInteger(); }
      operator float() const { return this->toNumber(); }
      operator double() const { return this->toNumber(); }
      operator const char*() const { return this->toString().c_str(); }
      operator std::string() const { return this->toString().c_str(); }

      bool operator = (bool x) { this->set(bool(x)); return x; }
      int operator = (int8_t x) { this->set(int64_t(x)); return x; }
      int operator = (int16_t x) { this->set(int64_t(x)); return x; }
      int operator = (int32_t x) { this->set(int64_t(x)); return x; }
      int operator = (int64_t x) { this->set(int64_t(x)); return x; }
      int operator = (uint8_t x) { this->set(int64_t(x)); return x; }
      int operator = (uint16_t x) { this->set(int64_t(x)); return x; }
      int operator = (uint32_t x) { this->set(int64_t(x)); return x; }
      int operator = (uint64_t x) { this->set(int64_t(x)); return x; }
      float operator = (float x) { this->set(double(x)); return x; }
      float operator = (double x) { this->set(double(x)); return x; }
      char* operator = (char* x) { this->set(x); return x; }
      const char* operator = (const char* x) { this->set(x); return x; }
      const std::string& operator = (const std::string& x) { this->set(x.c_str()); return x; }
      Value& operator = (Value& x) { this->set(&x); return x; }
      Value& operator [] (const char* x) { return this->map(x); }
      Value& operator [] (const std::string& x) { return this->map(x.c_str()); }
      Value& operator [] (intptr_t x) { return this->get(x); }

      ValueMetric getMetric();
    };

    struct Symbol: SymbolBase<caseInsensitive> {
      uint32_t hash;
      Symbol* ns;
      Value name;

      inline bool equals(Symbol* other) {
        if (this->hash != other->hash) return false;
        if (this->name._string->length != other->name._string->length) return false;
        return !this->compare_bytes(this->name._string->buffer, other->name._string->buffer, this->name._string->length);
      }
      inline bool equals(const char* buffer) {
        if (this->length != strlen(buffer)) return false;
        return !this->compare_bytes(this->buffer, buffer, this->length);
      }
      inline int compare(uint32_t hash, const char* buffer, size_t length) {
        ObjectString* name = this->name._string;
        int32_t c = int32_t(this->hash - hash);
        if (!c) {
          c = int32_t(name->length - length);
          if (!c) {
            c = this->compare_bytes(name->buffer, buffer, length);
          }
        }
        return c;
      }
      inline int compare(Symbol* other) {
        ObjectString* other_name = other->name._string;
        return this->compare(other->hash, other_name->buffer, other_name->length);
      }
    };

    template <class ValueImpl = Value>
    struct array_iterator {
      Item* item;
      array_iterator(Value* value) {
        this->item = (value->typeID == TypeID::Array) ? (value->_array->firstItem) : 0;
      }
      ValueImpl* begin() {
        if (Item* item = this->item) return (ValueImpl*)&item->value;
        else return 0;
      }
      ValueImpl* next() {
        _ASSERT(this->item != 0);
        if (Item* item = this->item = this->item->next) return (ValueImpl*)&item->value;
        else return 0;
      }
    };

    template <class ValueImpl = Value>
    struct map_iterator : ObjectMap::iterator {
      map_iterator(Value* value) : ObjectMap::iterator((value->typeID == TypeID::Map) ? value->_map : 0) {
      }
      ValueImpl* begin() {
        if (Property* prop = this->iterator::begin()) return (ValueImpl*)&prop->value;
        else return 0;
      }
      ValueImpl* next() {
        if (Property* prop = this->iterator::next()) return (ValueImpl*)&prop->value;
        else return 0;
      }
      ObjectString* key() {
        _ASSERT(this->cproperty != 0);
        return this->cproperty->key;
      }
    };

    struct Allocator {
    public:
      Allocator(size_t defaultPageSize = DefaultPage_size);
      ~Allocator();
      __forceinline void* alloc(size_t size);
      __forceinline void free(void* ptr, size_t size);
      void clean();

    private:
#pragma warning( disable : 4200 )
      struct tAllocPage {
        tAllocPage* next;
        size_t used;
        size_t size;
        uint8_t buffer[0];
      };
#pragma warning( default : 4200 )

      tAllocPage* page;
      size_t pageSize;
      size_t defaultPageSize;

      __declspec(noinline) tAllocPage* alloc_page(size_t size);
    };

    struct Document : IDocument, Allocator {

      tCharsetType charset;
      void** hashMapReserve[24];
      std::map<uint32_t, Symbol*> symbols;

      Symbol* className_symbol;

      Document(tCharsetType charset = tCharsetType::ASCII_charset, size_t pageSize = DefaultPage_size);

      __forceinline void** allocHashMap(int shift);
      __forceinline void freeHashMap(void** hashmap, int shift);

      Symbol* _lookup_symbol(uint32_t hash, const char* str, size_t len, Symbol* ns);
      Symbol* createSymbol(const char* str, size_t len, Symbol* ns = 0);
      Symbol* getSymbol(const char* str, size_t len, Symbol* ns = 0);
      Symbol* mapSymbol(const char* str, size_t len, Symbol* ns = 0);

      __forceinline ObjectMap* createObjectMap();
      __forceinline ObjectArray* createObjectArray();
      __forceinline ObjectString* createObjectString(const char* str, size_t len);

      __forceinline Property* createProperty(Symbol* symbol);
      __forceinline Value* createValue();
      __forceinline Item* createItem();

      virtual void* allocValue(size_t head);

      void printDictionary();
    };

#  include "document-json-helper.hpp"

  };
#define SoftDoc_TEMPLATE_DECL template<bool caseInsensitive,bool orderedMap,class IDocument,class IValue>
#define SoftDoc_TEMPLATE_PREFIX Template<caseInsensitive,orderedMap,IDocument,IValue>
#define SoftDoc_CTOR()     SoftDoc_TEMPLATE_DECL inline SoftDoc_TEMPLATE_PREFIX::
#define SoftDoc_IMPLn(T)   SoftDoc_TEMPLATE_DECL inline T SoftDoc_TEMPLATE_PREFIX::
#define SoftDoc_IMPLi(T)   SoftDoc_TEMPLATE_DECL inline typename SoftDoc_TEMPLATE_PREFIX::T SoftDoc_TEMPLATE_PREFIX::

  SoftDoc_CTOR() Value::Value(Document* document) {
    this->document = document;
    this->typeID = TypeID::Undefined;
    this->_bits = 0;
  }
  SoftDoc_IMPLn(void) Value::set(TypeID typeID) {
    switch (typeID) {
    case TypeID::Boolean:
    case TypeID::Integer:
    case TypeID::Number:
    case TypeID::Null:
      this->typeID = typeID;
      this->_bits = 0;
      return;
    case TypeID::Array:
      this->typeID = TypeID::Array;
      this->_array = this->document->createObjectArray();
      return;
    case TypeID::Map:
      this->typeID = TypeID::Map;
      this->_map = this->document->createObjectMap();
      return;
    default:
      this->typeID = TypeID::Undefined;
      this->_bits = 0;
    }
  }
  SoftDoc_IMPLn(void) Value::set(std::string x) {
    this->typeID = TypeID::String;
    this->_object = (Object*)document->createObjectString(x.c_str(), x.size());
  }
  SoftDoc_IMPLn(void) Value::set(const char* x, size_t len) {
    this->typeID = TypeID::String;
    this->_object = (Object*)document->createObjectString(x, len ? len : strlen(x) );
  }
  SoftDoc_IMPLn(void) Value::set_symbol(const char* x, size_t len) {
    this->typeID = TypeID::Symbol;
    this->_object = (Object*)document->createObjectString(x, len ? len : strlen(x) );
  }
  SoftDoc_IMPLn(void) Value::set(Value* x) {
    if (x) {
      this->typeID = x->typeID;
      this->_bits = x->_bits;
    }
    else {
      this->typeID = TypeID::Null;
      this->_bits = 0;
    }
  }
  SoftDoc_IMPLn(int) Value::count() {
    switch (typeID) {
    case TypeID::Array:
      return this->_array ? this->_array->count() : 0;
    case TypeID::Map:
      return this->_map ? this->_map->count() : 0;
    default:
      return 0;
    }
  }
  SoftDoc_IMPLn(bool) Value::toBoolean(bool defaultValue) const {
    switch (this->typeID) {
    case TypeID::Boolean:
      return this->_boolean;
    case TypeID::Integer:
      return this->_integer != 0;
    case TypeID::Number:
      return this->_number != 0;
    case TypeID::String:
      if (this->_string->equals("true")) return true;
      return false;
    default:
      return defaultValue;
    }
  }
  SoftDoc_IMPLn(int64_t) Value::toInteger(int64_t defaultValue) const {
    switch (this->typeID) {
    case TypeID::Boolean:
      return int64_t(this->_boolean);
    case TypeID::Integer:
      return int64_t(this->_integer);
    case TypeID::Number:
      return int64_t(this->_number);
    case TypeID::String:
      return _atoi64(this->_string->buffer);
    default:
      return defaultValue;
    }
  }
  SoftDoc_IMPLn(double) Value::toNumber(double defaultValue) const {
    switch (this->typeID) {
    case TypeID::Boolean:
      return double(this->_boolean);
    case TypeID::Integer:
      return double(this->_integer);
    case TypeID::Number:
      return double(this->_number);
    case TypeID::String:
      return atof(this->_string->buffer);
    default:
      return defaultValue;
    }
  }
  SoftDoc_IMPLn(std::string) Value::toString(const char* defaultValue) const {
    char tmp[64];
    switch (this->typeID) {
    case TypeID::Boolean:
      if (this->_boolean) return "true";
      else return "false";
    case TypeID::Integer:
      _i64toa_s(this->_integer, tmp, 64, 10);
      return tmp;
    case TypeID::Number:
      _gcvt_s(tmp, 64, this->_number, 10);
      return tmp;
    case TypeID::String:
      return this->_string->buffer;
    case TypeID::Symbol:
      return this->_string->buffer;
    default:
      return defaultValue;
    }
  }

  SoftDoc_IMPLi(Value*) Value::className() {
    return this->find(this->document->className_symbol);
  }
  SoftDoc_IMPLi(Value&) Value::map(Symbol* key) {
    if (this->typeID != TypeID::Map) {
      this->typeID = TypeID::Map;
      this->_map = this->document->createObjectMap();
    }
    return this->_map->map(key, this->document)->value;
  }
  SoftDoc_IMPLi(Value&) Value::map(const char* key, size_t len) {
    if (this->typeID != TypeID::Map) {
      this->typeID = TypeID::Map;
      this->_map = this->document->createObjectMap();
    }
    Symbol* symbol = this->document->mapSymbol(key, len ? len : strlen(key) );
    return this->_map->map(symbol, this->document)->value;
  }
  SoftDoc_IMPLi(Value*) Value::find(Symbol* key) {
    if (this->typeID == TypeID::Map) {
      Property* found = this->_map->find(key);
      if (found && found->value.typeID != TypeID::Undefined) {
        return &found->value;
      }
    }
    return 0;
  }
  SoftDoc_IMPLi(Value*) Value::find(const char* key, size_t len) {
    if (this->typeID == TypeID::Map) {
      Symbol* symbol = this->document->getSymbol(key, len ? len : strlen(key) );
      if (symbol) return this->find(symbol);
    }
    return 0;
  }
  SoftDoc_IMPLi(Value*) Value::findAt(const char* key, size_t len) {
    throw "@TODO";
  }
  SoftDoc_IMPLi(Value&) Value::get(intptr_t index) {
    if (this->typeID == TypeID::Array) {
      if (Item* r = this->_array->get(index)) {
        return r->value;
      }
    }
    return *this->document->createValue();
  }
  SoftDoc_IMPLi(Value&) Value::push_front() {
    if (this->typeID != TypeID::Array) {
      this->typeID = TypeID::Array;
      this->_array = this->document->createObjectArray();
    }
    return this->_array->push_front(this->document)->value;
  }
  SoftDoc_IMPLi(Value&) Value::push_back() {
    if (this->typeID != TypeID::Array) {
      this->typeID = TypeID::Array;
      this->_array = this->document->createObjectArray();
    }
    return this->_array->push_back(this->document)->value;
  }
  SoftDoc_IMPLn(bool) Value::equals(Value* other) {
    if (this->typeID == other->typeID) {
      if (this->_bits == other->_bits) {
        return true;
      }
      switch (this->typeID) {
      case TypeID::Array:
        return this->_array->equals(other->_array);
      case TypeID::Map:
        return this->_map->equals(other->_map);
      case TypeID::Symbol:
        return this->_string->equals(other->_string);
      case TypeID::String:
        return this->_string->equals(other->_string);
      case TypeID::Boolean:
        return this->_boolean == other->_boolean;
      case TypeID::Integer:
        return this->_boolean == other->_boolean;
      case TypeID::Number:
        return this->_number == other->_number;
      }
    }
    return false;
  }
  SoftDoc_IMPLn(void) Value::copy(Value* src) {
    this->typeID = src->typeID;
    switch (src->typeID) {
    case TypeID::Array:
      this->_array = src->_array->copy(this->document);
      break;
    case TypeID::Map:
      this->_map = src->_map->copy(this->document);
      break;
    default:
      this->_bits = src->_bits;
      break;
    }
  }
  SoftDoc_IMPLn(void) Value::duplicate(Value* src) {
    this->typeID = src->typeID;
    switch (src->typeID) {
    case TypeID::Array:
      this->_array = src->_array->duplicate(this->document);
      break;
    case TypeID::Map:
      this->_map = src->_map->duplicate(this->document);
      break;
    default:
      this->_bits = src->_bits;
      break;
    }
  }
  SoftDoc_IMPLn(void) Value::copyMinimize(Value* src) {
    switch (this->typeID = src->typeID) {
    case TypeID::Array:
      this->_array = this->document->createObjectArray();
      break;
    case TypeID::Map:
      this->_map = this->document->createObjectMap();
      break;
    default:
      this->_bits = src->_bits;
    }
  }
  SoftDoc_IMPLn(void) Value::subtract(Value* valueA, Value* valueB)
  {
    if (valueA->typeID == valueB->typeID) {
      switch (valueA->typeID) {
      case TypeID::Array:
        if (this->_array = valueA->_array->subtract(valueB->_array, this->document)) {
          this->typeID = TypeID::Array;
          return;
        } break;
      case TypeID::Map:
        if (this->_map = valueA->_map->subtract(valueB->_map, this->document)) {
          this->typeID = TypeID::Map;
          return;
        } break;
      case TypeID::String:
        if (!valueA->_string->equals(valueB->_string)) {
          this->set(valueA);
          return;
        } break;
      case TypeID::Symbol:
        if (!valueA->_string->equals(valueB->_string)) {
          this->set(valueA);
          return;
        } break;
      default:
        if (valueA->_bits != valueB->_bits) {
          this->set(valueA);
          return;
        }
      }
      this->typeID = TypeID::Undefined;
      this->_bits = 0;
    }
    else {
      this->set(valueA);
    }
  }

  SoftDoc_IMPLi(ValueMetric) Value::getMetric() {
    typedef ObjectArray::iterator _array_iterator;
    typedef ObjectMap::iterator _map_iterator;

    ValueMetric metric;
    metric.depth = 0;
    metric.width = 1;
    switch (this->typeID) {
    case TypeID::Array:
      {
        _array_iterator it(this->_array);
        for (Item* item = it.begin(); item; item = it.next()) {
          ValueMetric propMetric = item->value.getMetric();
          if (propMetric.depth > metric.depth) metric.depth = propMetric.depth;
          metric.width += metric.width;
        }
        metric.depth++;
        return metric;
      }
    case TypeID::Map:
      {
        _map_iterator it(this->_map);
        for (Property* prop = it.begin(); prop; prop = it.next()) {
          ValueMetric propMetric = prop->value.getMetric();
          if (propMetric.depth > metric.depth) metric.depth = propMetric.depth;
          metric.width += metric.width;
        }
        metric.depth++;
        return metric;
      }
    }
    return metric;
  }



  SoftDoc_CTOR() Allocator::Allocator(size_t defaultPageSize) {
    this->defaultPageSize = defaultPageSize;
    this->pageSize = defaultPageSize;
    this->page = 0;
    this->alloc_page(0);
  }
  SoftDoc_CTOR() Allocator::~Allocator() {
    while (this->page) {
      tAllocPage* buf = this->page;
      this->page = buf->next;
      ::free(buf);
    }
  }
  SoftDoc_IMPLn(void) Allocator::clean() {
    tAllocPage*& cpage = this->page->next;
    this->page->used = 0;
    while (cpage) {
      tAllocPage* buf = cpage;
      cpage = buf->next;
      ::free(buf);
    }
    this->pageSize = this->defaultPageSize;
  }
  SoftDoc_IMPLi(Allocator::tAllocPage*) Allocator::alloc_page(size_t size) {

    // When the size is too big, a specific page is create for it
    if (this->pageSize < (size << 3)) { // limit internal fragmentation to 12.5%
      tAllocPage* buf = ((tAllocPage*)malloc(sizeof(tAllocPage) + size));
      buf->size = size;
      buf->used = 0;
      buf->next = this->page->next;
      this->page->next = buf;
      return buf;
    }
    // Append a new free page
    else {
      size_t buf_size = this->pageSize;
      tAllocPage* buf = ((tAllocPage*)malloc(sizeof(tAllocPage) + buf_size));
      memset(buf->buffer, 0, buf_size);
      buf->size = buf_size;
      buf->used = 0;
      buf->next = this->page;
      this->page = buf;
      this->pageSize <<= 1; // Grow future page size
      return buf;
    }
  }
  SoftDoc_IMPLn(void*) Allocator::alloc(size_t size) {
    tAllocPage* buf = this->page;
    size = SoftDocument::alignPtr(size);
    if (buf->used + size > buf->size) {
      buf = this->alloc_page(size);
    }
    _ASSERT(buf->used + size <= buf->size);
    void* ptr = &buf->buffer[buf->used];
    buf->used += size;
    return ptr;
  }
  SoftDoc_IMPLn(void) Allocator::free(void* ptr, size_t size) {
    //::free(ptr);
  }


  SoftDoc_CTOR() Document::Document(SoftDocument::tCharsetType charset, size_t pageSize) : Allocator(pageSize) {
    this->charset = charset;
    this->className_symbol = this->createSymbol("classname", 0);
    memset(this->hashMapReserve, 0, sizeof(this->hashMapReserve));
  }
  SoftDoc_IMPLn(void**) Document::allocHashMap(int shift) {
    void** hashmap;
    if (hashmap = this->hashMapReserve[shift]) {
      this->hashMapReserve[shift] = (void**)hashmap[0];
      return hashmap;
    }
    return (void**)this->alloc((sizeof(void*) << shift) + sizeof(void*));
  }
  SoftDoc_IMPLn(void) Document::freeHashMap(void** hashmap, int shift) {
    hashmap[0] = this->hashMapReserve[shift];
    this->hashMapReserve[shift] = hashmap;
  }
  SoftDoc_IMPLi(ObjectMap*) Document::createObjectMap() {
    int _size = sizeof(ObjectMap);
    ObjectMap* obj = (ObjectMap*)this->alloc(_size);
    obj->ObjectMap::ObjectMap();
    return obj;
  }
  SoftDoc_IMPLi(ObjectArray*) Document::createObjectArray() {
    ObjectArray* obj = (ObjectArray*)this->alloc(sizeof(ObjectArray));
    obj->ObjectArray::ObjectArray();
    return obj;
  }
  SoftDoc_IMPLi(ObjectString*) Document::createObjectString(const char* str, size_t len) {
    ObjectString* obj = (ObjectString*)this->alloc(sizeof(ObjectString) + len);
    memcpy(obj->buffer, str, len);
    obj->buffer[len] = 0;
    obj->length = (uint32_t)len;
    return obj;
  }
  SoftDoc_IMPLi(Property*) Document::createProperty(Symbol* key) {
    Property* prop = (Property*)this->allocValue(sizeof(Property) - sizeof(Value));
    prop->key = key;
    return prop;
  }
  SoftDoc_IMPLi(Item*) Document::createItem() {
    return (Item*)this->allocValue(sizeof(Item) - sizeof(Value));
  }
  SoftDoc_IMPLi(Value*) Document::createValue() {
    return (Value*)this->allocValue(0);
  }
  SoftDoc_IMPLn(void*) Document::allocValue(size_t head) {
    char* buffer = (char*)this->alloc(sizeof(Value) + head);
    ((Value*)&buffer[head])->Value::Value(this);
    return buffer;
  }
  SoftDoc_IMPLi(Symbol*) Document::_lookup_symbol(uint32_t hash, const char* str, size_t len, Symbol* ns) {
    for(std::map<uint32_t,Symbol*>::iterator it=this->symbols.find(hash);it!=this->symbols.end();it++) {
      if(!it->second->compare(hash, str, len)) {
        return it->second;
      }
    }
    return 0;
  }
  SoftDoc_IMPLi(Symbol*) Document::getSymbol(const char* str, size_t len, Symbol* ns) {
    uint32_t hash = Symbol::hash_symbol(str, len);
    return this->_lookup_symbol(hash, str, len, ns);
  }
  SoftDoc_IMPLi(Symbol*) Document::mapSymbol(const char* str, size_t len, Symbol* ns) {
    uint32_t hash = Symbol::hash_symbol(str, len);
    Symbol* symbol = this->_lookup_symbol(hash, str, len, ns);
    if(!symbol) {
      symbol = (Symbol*)this->allocValue(sizeof(Symbol) - sizeof(Value));
      symbol->hash = hash;
      symbol->ns = ns;
      symbol->name.typeID = TypeID::Symbol;
      symbol->name._string = this->createObjectString(str, len);
      this->symbols.insert(std::pair<uint32_t,Symbol*>(hash, symbol));
    }
    return symbol;
  }
  SoftDoc_IMPLi(Symbol*) Document::createSymbol(const char* str, size_t len, Symbol* ns) {
    uint32_t hash = Symbol::hash_symbol(str, len);
    Symbol* symbol = (Symbol*)this->allocValue(sizeof(Symbol) - sizeof(Value));
    symbol->hash = hash;
    symbol->ns = ns?ns:symbol;
    symbol->name.typeID = TypeID::Symbol;
    symbol->name._string = this->createObjectString(str, len);
    return symbol;
  }
  SoftDoc_IMPLn(void) Document::printDictionary() {
    for(std::map<uint32_t,Symbol*>::iterator it=this->symbols.begin();it!=this->symbols.end();it++) {
      printf("> symbol '%s'\n", it->second->name._string->buffer);
    }
  }
}


SoftDoc_CTOR() Value::Value(Document* document) {
   this->document = document;
   this->typeID = TypeID::Undefined;
   this->_bits = 0;
}
SoftDoc_IMPLn(void) Value::undefine() {
   this->typeID = TypeID::Undefined;
   this->_bits = 0;
}
SoftDoc_IMPLn(void) Value::set(TypeID typeID) {
   switch(typeID) {
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
   default:
      this->typeID = TypeID::Undefined;
      this->_bits = 0;
   }
}
SoftDoc_IMPLn(void) Value::set(bool x) {
   this->typeID = TypeID::Boolean;
   this->_boolean = x;
}
SoftDoc_IMPLn(void) Value::set(int64_t x) {
   this->typeID = TypeID::Integer;
   this->_integer = x;
}
SoftDoc_IMPLn(void) Value::set(double x) {
   this->typeID = TypeID::Number;
   this->_number = x;
}
SoftDoc_IMPLn(void) Value::set(const char* x, int len) {
   this->typeID = TypeID::String;
   this->_object = (Object*)document->createObjectString(x, len<0?strlen(x):len);
}
SoftDoc_IMPLn(void) Value::set_symbol(const char* x, int len) {
   if(len<0) len = strlen(x);
   this->typeID = TypeID::Symbol;
   this->_object = (Object*)document->createObjectSymbol(hash_case_insensitive(x, len), x, len);
}
SoftDoc_IMPLn(void) Value::set(Value* x) {
   this->typeID = x->typeID;
   this->_bits = x->_bits;
}
SoftDoc_IMPLi(ObjectSymbol*) Value::className() {
   if(this->typeID == TypeID::Map) {
      return this->_map->classname;
   }
   return 0;
}
SoftDoc_IMPLi(Value&) Value::map(const char* key) {
   if(this->typeID != TypeID::Map) {
      this->typeID = TypeID::Map;
      this->_map = this->document->createObjectMap();
   }
   return this->_map->map(key, this->document)->value;
}
SoftDoc_IMPLi(Value&) Value::map(ObjectSymbol* key) {
   if(this->typeID != TypeID::Map) {
      this->typeID = TypeID::Map;
      this->_map = this->document->createObjectMap();
   }
   return this->_map->map(key, this->document)->value;
}
SoftDoc_IMPLi(Value*) Value::find(const char* key) {
   if(this->typeID == TypeID::Map) {
      Value& found = this->map(key);
      if(found.typeID != TypeID::Undefined) {
         return &found;
      }
   }
   return 0;
}
SoftDoc_IMPLi(Value&) Value::get(intptr_t index) {
   if(this->typeID == TypeID::Array) {
      if(Item* r = this->_array->get(index)) {
         return r->value;
      }
   }
   return *this->document->createValue();
}
SoftDoc_IMPLi(Value&) Value::push_front() {
   if(this->typeID != TypeID::Array) {
      this->typeID = TypeID::Array;
      this->_array = this->document->createObjectArray();
   }
   return this->_array->push_front(this->document)->value;
}
SoftDoc_IMPLi(Value&) Value::push_back() {
   if(this->typeID != TypeID::Array) {
      this->typeID = TypeID::Array;
      this->_array = this->document->createObjectArray();
   }
   return this->_array->push_back(this->document)->value;
}
SoftDoc_IMPLn(bool) Value::equals(Value* other) {
   if(this->typeID == other->typeID) {
      if(this->_bits == other->_bits) {
         return true;
      }
      switch(this->typeID) {
      case TypeID::Array:
         return this->_array->equals(other->_array);
      case TypeID::Map:
         return this->_map->equals(other->_map);
      case TypeID::Symbol:
         return this->_symbol->equals(other->_symbol);
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
   switch(src->typeID) {
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
   switch(src->typeID) {
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
   switch(this->typeID = src->typeID) {
   case TypeID::Array:
      this->_array = this->document->createObjectArray();
      break;
   case TypeID::Map:
      this->_map = this->document->createObjectMap();
      this->_map->classname = src->_map->classname;
      break;
   default:
      this->_bits = src->_bits;
   }
}
SoftDoc_IMPLn(void) Value::subtract(Value* valueA, Value* valueB)
{
   if(valueA->typeID == valueB->typeID) {
      switch(valueA->typeID) {
      case TypeID::Array:
         if(this->_array = valueA->_array->subtract(valueB->_array, this->document)) {
            this->typeID = TypeID::Array;
            return;
         } break;
      case TypeID::Map:
         if(this->_map = valueA->_map->subtract(valueB->_map, this->document)) {
            this->typeID = TypeID::Map;
            return;
         } break;
      case TypeID::String:
         if(!valueA->_string->equals(valueB->_string)) {
            this->set(valueA);
            return;
         } break;
      case TypeID::Symbol:
         if(!valueA->_symbol->equals(valueB->_symbol)) {
            this->set(valueA);
            return;
         } break;
      default:
         if(valueA->_bits != valueB->_bits) {
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
   ValueMetric metric;
   metric.depth = 0;
   metric.width = 1;
   switch(this->typeID) {
   case TypeID::Array:
      {
         ObjectArray::iterator it(this->_array);
         for (Item* item=it.begin();item;item=it.next()) {
            ValueMetric propMetric = item->value.getMetric();
            if(propMetric.depth > metric.depth) metric.depth = propMetric.depth;
            metric.width += metric.width;
         }
         metric.depth++;
      }
   case TypeID::Map:
      {
         ObjectMap::iterator it(this->_map);
         for (Property* prop=it.begin();prop;prop=it.next()) {
            ValueMetric propMetric = prop->value.getMetric();
            if(propMetric.depth > metric.depth) metric.depth = propMetric.depth;
            metric.width += metric.width;
         }
         metric.depth++;
      }
   }
   return metric;
}



SoftDoc_CTOR() Allocator::Allocator(int pageSize) {
   this->pageSize = pageSize;
   this->page = 0;
   this->alloc_page(0);
}
SoftDoc_CTOR() Allocator::~Allocator() {
   while(this->page) {
      tAllocPage* buf = this->page;
      this->page = buf->next;
      ::free(buf);
   }
}
SoftDoc_IMPLi(Allocator::tAllocPage*) Allocator::alloc_page(int size) {

   // When the size is too big, a specific page is create for it
   if(this->pageSize < (size<<3)) { // limit internal fragmentation to 12.5%
      tAllocPage* buf = ((tAllocPage*)malloc(sizeof(tAllocPage)+size));
      buf->size = size;
      buf->used = 0;
      buf->next = this->page->next;
      this->page->next = buf;
      return buf;
   }
   // Append a new free page
   else {
      int buf_size = this->pageSize;
      tAllocPage* buf = ((tAllocPage*)malloc(sizeof(tAllocPage)+buf_size));
      memset(buf->buffer, 0, buf_size);
      buf->size = buf_size;
      buf->used = 0;
      buf->next = this->page;
      this->page = buf;
      this->pageSize <<= 1; // Grow future page size
      return buf;
   }
}
SoftDoc_IMPLn(void*) Allocator::alloc(int size) {
   tAllocPage* buf = this->page;
   size = alignPtr(size);
   if(buf->used+size > buf->size) {
      buf = this->alloc_page(size);
   }
   _ASSERT(buf->used+size <= buf->size);
   void* ptr = &buf->buffer[buf->used];
   buf->used += size;
   return ptr;
}
SoftDoc_IMPLn(void) Allocator::free(void* ptr, int size) {
   //::free(ptr);
}



SoftDoc_CTOR() Document::Document(tCharsetType charset, int pageSize) : Allocator(pageSize) {
   this->charset = charset;
   memset(this->hashMapReserve, 0, sizeof(this->hashMapReserve));
}
SoftDoc_IMPLn(void**) Document::allocHashMap(int shift) {
   void** hashmap;
   if(hashmap = this->hashMapReserve[shift]) {
      this->hashMapReserve[shift] = (void**)hashmap[0];
      return hashmap;
   }
   return (void**)this->alloc((sizeof(void*)<<shift)+sizeof(void*));
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
SoftDoc_IMPLi(ObjectString*) Document::createObjectString(const char* str, int len) {
   ObjectString* obj = (ObjectString*)this->alloc(sizeof(ObjectString)+len);
   memcpy(obj->buffer, str, len);
   obj->buffer[len] = 0;
   obj->length = len;
   return obj;
}
SoftDoc_IMPLi(ObjectSymbol*) Document::createObjectSymbol(const char* str, int len) {
   if(len < 0) len = strlen(str);
   return this->createObjectSymbol(hash_case_insensitive(str, len), str, len);
}
SoftDoc_IMPLi(ObjectSymbol*) Document::createObjectSymbol(uint32_t hash, const char* str, int len) {
   ObjectSymbol* obj = (ObjectSymbol*)this->alloc(sizeof(ObjectSymbol)+len);
   memcpy(obj->buffer, str, len);
   obj->buffer[len] = 0;
   obj->hash = hash;
   obj->length = len;
   return obj;
}
SoftDoc_IMPLi(Property*) Document::createProperty(uint32_t hash, const char* str, int len) {
   int symbol_size = alignPtr(sizeof(ObjectSymbol)+len);
   char* buffer = (char*)this->allocValue(symbol_size+sizeof(Property)-sizeof(Value));
   Property* prop = (Property*)&buffer[symbol_size];
   ObjectSymbol* key = prop->key = (ObjectSymbol*)&buffer[0];
   memcpy(key->buffer, str, len);
   key->buffer[len] = 0;
   key->hash = hash;
   key->length = len;
   return prop;
}
SoftDoc_IMPLi(Property*) Document::createProperty(ObjectSymbol* key) {
   Property* prop = (Property*)this->allocValue(sizeof(Property)-sizeof(Value));
   prop->key = key;
   return prop;
}
SoftDoc_IMPLi(Item*) Document::createItem() {
   return (Item*)this->allocValue(sizeof(Item)-sizeof(Value));
}
SoftDoc_IMPLi(Value*) Document::createValue() {
   return (Value*)this->allocValue(0);
}
SoftDoc_IMPLn(void*) Document::allocValue(int head) {
   char* buffer = (char*)this->alloc(sizeof(Value)+head);
   ((Value*)&buffer[head])->Value::Value(this);
   return buffer;
}

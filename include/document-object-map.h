
struct ObjectMap : Object {
  ObjectSymbol* classname;
  Property** hashmap;
  int32_t limit;
  int32_t shift;

  struct iterator {
    Property** chashmap;
    Property* cproperty;
    inline iterator(ObjectMap* obj) {
      this->cproperty = (Property*)EndOfPtr;
      this->chashmap = (obj && obj->hashmap)?obj->hashmap:&this->cproperty;
    }
    inline ObjectSymbol* key() {
      return this->cproperty->key;
    }
    inline Property* begin() {
      while(!(this->cproperty = this->chashmap[0])) this->chashmap++;
      if(this->cproperty == EndOfPtr) return 0;
      return this->cproperty;
    }
    inline Property* next() {
      _ASSERT(this->chashmap[0] != EndOfPtr && this->cproperty != EndOfPtr);
      if(!(this->cproperty = this->cproperty->next)) {
        do{this->chashmap++;}while(!this->chashmap[0]);
        if((this->cproperty = this->chashmap[0]) == EndOfPtr) return 0;
      }
      return this->cproperty;
    }
  };

  ObjectMap() {
    this->classname = 0;
    this->hashmap = 0;
    this->shift = 0;
    this->limit = -1;
  }
  bool isEmpty() {
    if(Property** hashmap=this->hashmap) {
      while(!hashmap[0]) hashmap++;
      return hashmap[0]==EndOfPtr;
    }
    return true;
  }
  ObjectMap* _helper_copy(Document* document, bool deep) {
    Property** hashmap_cpy = (Property**)document->allocHashMap(this->shift);
    Property** src_hashmap = this->hashmap;
    Property** dst_hashmap = hashmap_cpy;
    while(src_hashmap[0] != EndOfPtr) {
      if(Property* src_prop = src_hashmap[0]) {
        Property* dst_prop = dst_hashmap[0] = document->createProperty(src_prop->key);
        if(deep) dst_prop->value.duplicate(&src_prop->value);
        else dst_prop->value.copy(&src_prop->value);
        while(src_prop = src_prop->next) {
          dst_prop = dst_prop->next = document->createProperty(src_prop->key);
          if(deep) dst_prop->value.duplicate(&src_prop->value);
          else dst_prop->value.copy(&src_prop->value);
        }
        dst_prop->next = 0;
      }
      else dst_hashmap[0] = 0;
      src_hashmap++;dst_hashmap++;
    }
    dst_hashmap[0] = (Property*)EndOfPtr;

    ObjectMap* obj = document->createObjectMap();
    obj->classname = this->classname;
    obj->hashmap = hashmap_cpy;
    obj->limit = this->limit;
    obj->shift = this->shift;
    return obj;
  }
  ObjectMap* copy(Document* document) {
    return this->_helper_copy(document, false);
  }
  ObjectMap* duplicate(Document* document) {
    return this->_helper_copy(document, true);
  }
  int count() {
    int c = 0;
    Property* cproperty = (Property*)EndOfPtr;
    Property** chashmap = this->hashmap?this->hashmap:&cproperty;
    while(!(cproperty = chashmap[0])) chashmap++;
    while(cproperty != EndOfPtr) {
      c++;
      if(!(cproperty = cproperty->next)) {
        do{chashmap++;}while(!chashmap[0]);
        cproperty = chashmap[0];
      }
    }
    return c;
  }
  bool equals(ObjectMap* other) {

    // Compare classname
    if(this->classname && other->classname) {
      if(!this->classname->equals(other->classname)) return false;
    }
    else if(this->classname != other->classname) return false;

    // Compare hashmap
    Property *propX, *propY;
    Property **hashmapX = this->hashmap, **hashmapY = other->hashmap;
    if(hashmapX && hashmapY) {
      while((propX = hashmapX[0]) != EndOfPtr && (propY = hashmapY[0]) != EndOfPtr) {
        while(propX && propY) {
          if(!propX->equals(propY)) return false;
          propX=propX->next; propY=propY->next;
        }
        if(propX != propY) return false; // Check the ending (null)
        hashmapX++;hashmapY++;
      }
      if(propX != propY) return false; // Check the ending (null)
    }
    else if(hashmapX != hashmapY) {
      return false;
    }

    return true;
  }
  ObjectMap* subtract(ObjectMap* other, Document* document) {
    iterator itX(this), itY(other);
    Property *prop, *propX = itX.begin(), *propY = itY.begin();
    ObjectMap* obj = 0;
    while(propX && propY) {
      int c = propX->key->compare(propY->key);
      if(!c) {
        Value diff(document);
        diff.subtract(&propX->value, &propY->value);
        if(diff.typeID != TypeID::Undefined) {
          if(!obj) obj = document->createObjectMap();
          prop = obj->map(propX->key->hash, propX->key->buffer, propX->key->length, document);
          prop->value.set(&diff);
        }
        propX = itX.next(); propY = itY.next();
      }
      else if(c<0) {
        if(!obj) obj = document->createObjectMap();
        prop = obj->map(propX->key->hash, propX->key->buffer, propX->key->length, document);
        prop->value.set(&propX->value);
        propX = itX.next();
      }
      else {
        propY = itY.next();
      }
    }
    return obj;
  }
  __forceinline void init_hashmap(Document* document) {
    Property** new_map = (Property**)document->allocHashMap(2);
    new_map[0] = 0;
    new_map[1] = 0;
    new_map[2] = 0;
    new_map[3] = 0;
    new_map[4] = (Property*)EndOfPtr;
    this->hashmap = new_map;
    this->shift = 2;
    this->limit = 4;
  }
  void expand_hashmap(Document* document) {

    int32_t old_shift = this->shift++;
    Property** old_map = this->hashmap;

    int32_t new_shift = this->shift;
    Property** new_map = (Property**)document->allocHashMap(new_shift);
    this->hashmap = new_map;
    this->limit += 1<<new_shift;

    uint32_t mask = 1<<(31-shift);
    for(Property** cur_map=old_map;cur_map[0] != EndOfPtr;cur_map++) {
      if(Property* cur = cur_map[0]) {
        if(cur->key->hash & mask) {
          new_map[0] = 0;
          new_map[1] = cur;
        }
        else {
          Property* next = cur->next;
          new_map[0] = cur;
          while(next) {
            if(next->key->hash & mask) break;
            cur = next;
            next = cur->next;
          }
          new_map[1] = next;
          cur->next = 0;
        }
      }
      else {
        new_map[0] = 0;
        new_map[1] = 0;
      }
      new_map += 2;
    }
    new_map[0] = (Property*)EndOfPtr;

    document->freeHashMap((void**)old_map, old_shift);
  }
  Property* map(const char* symbol, Document* document) {
    int symbolLen = strlen(symbol);
    return this->map(document->hash_symbol(symbol, symbolLen), symbol, symbolLen, document);
  }
  Property* map(const char* symbol, int symbolLen, Document* document) {
    return this->map(document->hash_symbol(symbol, symbolLen), symbol, symbolLen, document);
  }
  Property* map(uint32_t hash, const char* buffer, int length, Document* document) {

    // Resize when hashmap too small
    if(this->limit <= 0) {
      if(!this->hashmap) this->init_hashmap(document);
      else this->expand_hashmap(document);
    }

    // Find the insert slot
    uint32_t index = hash>>(31-this->shift);
    Property** pnext = &this->hashmap[index];
    Property* next = *pnext;
    while(next) {
      int c = next->key->compare(hash, buffer, length);
      if(c >= 0) {
        if(!c) return next;
        else break;
      }
      pnext = &next->next;
      next = *pnext;
    }

    // Insert a new property
    Property* prop = *pnext = document->createProperty(hash, buffer, length);prop->next = next;
    //static Property* prop = 0;if(!prop) prop = document->createProperty(hash, buffer, length);

    this->limit--;
    return prop;
  }
  Property* map(ObjectSymbol* key, Document* document) {

    // Resize when hashmap too small
    if(this->limit <= 0) {
      if(!this->hashmap) this->init_hashmap(document);
      else this->expand_hashmap(document);
    }

    // Find the insert slot
    uint32_t index = key->hash>>(31-this->shift);
    Property** pnext = &this->hashmap[index];
    Property* next = *pnext;
    while(next) {
      int c = next->key->compare(key);
      if(c >= 0) {
        if(!c) return next;
        else break;
      }
      pnext = &next->next;
      next = *pnext;
    }

    // Insert a new property
    Property* prop = *pnext = document->createProperty(key);
    prop->next = next;
    this->limit--;
    return prop;
  }
  void print() {
    uint32_t prevHash = 0;
    for(Property** hashmap=this->hashmap;hashmap[0]!=EndOfPtr;hashmap++) {
      Property* cur = hashmap[0];
      while(cur) {
        printf("%.10u %s (delta %d)\n", cur->key->hash, cur->key->buffer,  int(cur->key->hash)>=int(prevHash)?1:0);
        prevHash = cur->key->hash;
        cur = cur->next;
      }
    }
  }
};



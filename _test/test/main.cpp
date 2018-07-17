
//#define SoftDoc_TEMPLATE
#include <json-soft-document>

#include <stdio.h>
#include <map>
#include <unordered_map>
#include "chrono.h"
#include "stl_alloc.h"

typedef std::pair<uint32_t, const char*> t_pair;

void test_stringify() {
  typedef SoftDoc<> JSONDoc;

  JSONDoc::Document doc;
  JSONDoc::Value x(&doc);
  x["a"] = 5;
  x["b"] = 5.6f;
  x["c"]["héllo"] = 9;
  x["d"].push_back() = "x\nr";
  x["d"].push_back() = "y";
  x["d"].push_back() = "z";
  printf("stringify x: %s\n", JSONDoc::JSON::stringify(x).c_str());

  // Result: stringify x: {"b":5.6,"d":["x\nr","y","z"],"a":5,"c":{"h├®llo":9}}
}

void test_parse() {
  typedef SoftDoc<> JSONDoc;
  JSONDoc::Document doc;

  const char* y_txt = "{\"d\":[\"x\\\"\",\"y\",\"z\",30],\"a\":5,\"b\":5.6,\"c\":{\"__classname\":\"MyClass\",\"hello\":9,}}";
  JSONDoc::Value y(&doc);
  JSONDoc::JSON::parse(y, y_txt, 0, "__classname");
  printf("stringify y: %s\n", JSONDoc::JSON::stringify(y, "className").c_str());

  // Result: stringify y: {"b":5.6,"d":["x\"","y","z",30],"a":5,"c":{"className":"MyClass","hello":9}}
}

void test_substract() {
  typedef SoftDoc<> JSONDoc;
  JSONDoc::Document doc;

  JSONDoc::Value x(&doc);
  x["a"] = 5;
  x["b"] = 5.6f;
  x["c"]["w"] = 9;
  x["c"]["v"] = 9;
  x["d"].push_back() = "x\nr";
  x["d"].push_back() = "y";
  x["d"].push_back() = "z";

  JSONDoc::Value y(&doc);
  y["a"] = 5;
  y["b"] = 5.6f;
  y["c"]["w"] = 9;
  y["c"]["v"] = 10;
  y["d"].push_back() = "x\nr";
  y["d"].push_back() = "y";
  y["d"].push_back() = "z";

  JSONDoc::Value z(&doc);
  z.subtract(&x, &y);
  printf("stringify z: %s\n", JSONDoc::JSON::stringify(z).c_str());

  // Result: stringify z: {"c":{"v":9}}
}

const char* propertyNames[] = {
   "Build","PPC","keyword","lists","using","our","free","SEO",
   "and","SEM","keyword","list","generatory.","This","open",
   "source","tool","can","be","used","to","help","you","build",
   "a","list","of","relevant","keywords"
};

void main() {
  typedef SoftDoc<> JSONDoc;

  JSONDoc::Document doc;
  Chrono c;
  int count = 10000;

  struct tSymbol {
    uint32_t hash;
    const char* symbol;
    int length;
    JSONDoc::ObjectSymbol* key;
  };
  int numProperties = sizeof(propertyNames) / sizeof(char*);
  tSymbol* properties = new tSymbol[numProperties];
  for (int i = 0; i < numProperties; i++) {
    properties[i].hash = JSONDoc::ObjectSymbol::hash_symbol(propertyNames[i], strlen(propertyNames[i]));
    properties[i].symbol = propertyNames[i];
    properties[i].length = strlen(propertyNames[i]);
    properties[i].key = doc.createObjectSymbol(propertyNames[i]);
  }

  if (1) {
    c.Start();
    for (int k = 0; k < count; k++) {
      std::map<uint32_t, const char*, std::less<uint32_t>, StackAllocator<t_pair> > obj;
      for (int i = 0; i < numProperties; i++) {
        tSymbol& s = properties[i];
        obj.insert(t_pair(s.hash, s.symbol));
      }
    }
    printf("std::map               = %5.3g Mops\n", c.GetOpsFloat(count*numProperties, Chrono::Mops));
  }
  if (1) {
    c.Start();
    for (int k = 0; k < count; k++) {
      std::unordered_map<uint32_t, const char*, std::hash<uint32_t>, std::equal_to<uint32_t>, StackAllocator<t_pair> > obj;
      for (int i = 0; i < numProperties; i++) {
        tSymbol& s = properties[i];
        obj.insert(t_pair(s.hash, s.symbol));
      }
    }
    printf("std::unordered_map     = %5.3g Mops\n", c.GetOpsFloat(count*numProperties, Chrono::Mops));
  }
  if (1) {
    c.Start();
    for (int k = 0; k < count; k++) {
      for (int i = 0; i < numProperties; i++) {
        tSymbol& s = properties[i];
        doc.alloc(s.length);
      }
    }
    printf("alloc                  = %5.3g Mops\n", c.GetOpsFloat(count*numProperties, Chrono::Mops));
  }
  if (1) {
    c.Start();
    for (int k = 0; k < count; k++) {
      for (int i = 0; i < numProperties; i++) {
        tSymbol& s = properties[i];
        doc.allocValue(s.length);
      }
    }
    printf("allocValue             = %5.3g Mops\n", c.GetOpsFloat(count*numProperties, Chrono::Mops));
  }
  if (1) {
    c.Start();
    for (int k = 0; k < count; k++) {
      for (int i = 0; i < numProperties; i++) {
        tSymbol& s = properties[i];
        doc.createObjectSymbol(s.hash, s.symbol, s.length);
      }
    }
    printf("createSymbol           = %5.3g Mops\n", c.GetOpsFloat(count*numProperties, Chrono::Mops));
  }
  if (1) {
    c.Start();
    for (int k = 0; k < count; k++) {
      for (int i = 0; i < numProperties; i++) {
        tSymbol& s = properties[i];
        doc.createProperty(s.key);
      }
    }
    printf("createProperty (+key)  = %5.3g Mops\n", c.GetOpsFloat(count*numProperties, Chrono::Mops));
  }
  if (1) {
    c.Start();
    for (int k = 0; k < count; k++) {
      for (int i = 0; i < numProperties; i++) {
        tSymbol& s = properties[i];
        doc.createProperty(s.hash, s.symbol, s.length);
      }
    }
    printf("createProperty         = %5.3g Mops\n", c.GetOpsFloat(count*numProperties, Chrono::Mops));
  }
  if (1) {
    c.Start();
    for (int k = 0; k < count; k++) {
      JSONDoc::ObjectMap obj;
      for (int i = 0; i < numProperties; i++) {
        tSymbol& s = properties[i];
        obj.map(s.hash, s.symbol, s.length, &doc)->value = 0;
      }
    }
    printf("insert (+hashing)      = %5.3g Mops\n", c.GetOpsFloat(count*numProperties, Chrono::Mops));
  }
  if (1) {
    JSONDoc::ObjectMap obj;
    for (int i = 0; i < numProperties; i++) {
      tSymbol& s = properties[i];
      obj.map(s.symbol, &doc)->value = 0;
    }

    c.Start();
    for (int k = 0; k < count; k++) {
      for (int i = 0; i < numProperties; i++) {
        tSymbol& s = properties[i];
        JSONDoc::Value& y = obj.map(s.hash, s.symbol, s.length, &doc)->value;
        //Value& y = obj.map2(s.key, &doc)->value;
        y._integer++;
      }
    }
    printf("get                    = %5.3g Mops\n", c.GetOpsFloat(count*numProperties, Chrono::Mops));
  }
  if (0) {
    const char* json_test = "{\"someArray\":[\"foo\",\"bar\",123],\"someObject\":{\"foo\":\"bar\",\"embedded\":{\"eleet\":1337}},\"someString\":\"Hithere!\",\"someNumber\":123,\"someBoolean\":true}";
    JSONDoc::Value y(&doc);
    c.Start();
    for (int k = 0; k < count; k++) {
      JSONDoc::JSON::parse(y, json_test);
    }
    printf("JSON::parse            = %5.3g Mops\n", c.GetOpsFloat(count, Chrono::Mops));
  }
  if (1) {
    test_stringify();
    test_parse();
    test_substract();
  }
  getchar();
}

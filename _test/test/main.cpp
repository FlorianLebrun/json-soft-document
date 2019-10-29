
//#define SoftDoc_TEMPLATE
#include <json-soft-document>

#include <stdio.h>
#include <map>
#include <unordered_map>
#include "chrono.h"
#include "stl_alloc.h"

typedef std::pair<const char*, int> t_pair;

void test_stringify() {
  typedef SoftDocument::Template<> JSONDoc;

  JSONDoc::Document doc;
  JSONDoc::Value x(&doc);
  x["a"] = 5;
  x["b"] = 5.6f;
  x["c"]["héllo"] = 9;
  x["d"].push_back() = "x\nr";
  x["d"].push_back() = "y";
  x["d"].push_back() = "z";
  printf("stringify x: %s\n", JSONDoc::JSON::stringify(x).c_str());

  doc.printDictionary();
  // Result: stringify x: {"b":5.6,"d":["x\nr","y","z"],"a":5,"c":{"h├®llo":9}}
}

void test_parse() {
  typedef SoftDocument::Template<> JSONDoc;
  JSONDoc::Document doc;

  const char* y_txt = "{\"d\":[\"x\\\"\",\"y\",\"z\",30],\"a\":5,\"b\":5.6,\"c\":{\"__classname\":\"MyClass\",\"hello\":9,}}";
  JSONDoc::Value y(&doc);
  JSONDoc::JSON::parse(y, y_txt, 0, "__classname");
  printf("stringify y: %s\n", JSONDoc::JSON::stringify(y, true, "className").c_str());

  // Result: stringify y: {"b":5.6,"d":["x\"","y","z",30],"a":5,"c":{"className":"MyClass","hello":9}}
}

void test_substract() {
  typedef SoftDocument::Template<> JSONDoc;
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

  x.className();
  x.equals(&y);
  doc.createValue()->duplicate(&y);

  JSONDoc::Value z(&doc);
  z.subtract(&x, &y);
  printf("stringify z: %s\n", JSONDoc::JSON::stringify(z).c_str());

  // Result: stringify z: {"c":{"v":9}}
}

/*
void test_perf() {
  static const char* propertyNames[] = {
     "Build","PPC","keyword","lists","using","our","free","SEO",
     "and","SEM","keyword","list","generatory.","This","open",
     "source","tool","can","be","used","to","help","you","build",
     "a","list","of","relevant","keywords"
  };
  typedef SoftDocument::Template<> JSONDoc;
  JSONDoc::Document doc;
  Chrono c;
  int count = 10000;

  struct tSymbol {
    const char* symbol;
    int length;
  };
  int numProperties = sizeof(propertyNames) / sizeof(char*);
  tSymbol* properties = new tSymbol[numProperties];
  for (int i = 0; i < numProperties; i++) {
    properties[i].symbol = propertyNames[i];
    properties[i].length = (int)strlen(propertyNames[i]);
  }

  // Test std::map performance
  if (0) {
    c.Start();
    for (int k = 0; k < count; k++) {
      std::map<const char*, int, std::less<const char*>, StackAllocator<t_pair> > obj;
      for (int i = 0; i < numProperties; i++) {
        tSymbol& s = properties[i];
        obj.insert(t_pair(s.symbol, i));
      }
    }
    printf("std::map               = %5.3g Mops\n", c.GetOpsFloat(count*numProperties, Chrono::Mops));
  }
  // Test std::unordered_map performance
  if (0) {
    c.Start();
    for (int k = 0; k < count; k++) {
      std::unordered_map<const char*, int, std::equal_to<const char*>, StackAllocator<t_pair> > obj;
      for (int i = 0; i < numProperties; i++) {
        tSymbol& s = properties[i];
        obj.insert(t_pair(s.symbol, i));
      }
    }
    printf("std::unordered_map     = %5.3g Mops\n", c.GetOpsFloat(count*numProperties, Chrono::Mops));
  }
  // Test allocValue performance
  if (0) {
    c.Start();
    for (int k = 0; k < count; k++) {
      for (int i = 0; i < numProperties; i++) {
        tSymbol& s = properties[i];
        doc.allocValue(s.length);
      }
    }
    printf("allocValue             = %5.3g Mops\n", c.GetOpsFloat(count*numProperties, Chrono::Mops));
  }
  // Test createSymbol performance
  if (0) {
    c.Start();
    for (int k = 0; k < count; k++) {
      for (int i = 0; i < numProperties; i++) {
        tSymbol& s = properties[i];
        doc.createSymbol(s.symbol, s.length);
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
      obj.map(s.hash, s.symbol, s.length, &doc)->value = 0;
    }

    c.Start();
    for (int k = 0; k < count; k++) {
      for (int i = 0; i < numProperties; i++) {
        tSymbol& s = properties[i];
        JSONDoc::Value& y = obj.map(s.hash, s.symbol, s.length, &doc)->value;
        y._integer++;
      }
    }
    printf("get                    = %5.3g Mops\n", c.GetOpsFloat(count*numProperties, Chrono::Mops));
  }
  if (1) {
    const char* json_test = "{\"someArray\":[\"foo\",\"bar\",123],\"someObject\":{\"foo\":\"bar\",\"embedded\":{\"eleet\":1337}},\"someString\":\"Hithere!\",\"someNumber\":123,\"someBoolean\":true}";
    JSONDoc::Value y(&doc);
    c.Start();
    for (int k = 0; k < count; k++) {
      JSONDoc::JSON::parse(y, json_test);
    }
    printf("JSON::parse            = %5.3g Mops\n", c.GetOpsFloat(count, Chrono::Mops));
  }
}*/

void main() {
  printf("---- Begin JSON Test----\n");
  test_stringify();
  test_parse();
  test_substract();
  printf("---- End JSON Test----\n");
  getchar();
}

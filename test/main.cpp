
//#define SoftDoc_TEMPLATE
#include "Document.h"

#include <stdio.h>
#include <map>
#include <unordered_map>
#include "chrono.h"

const char* propertyNames[] = {
   "Build","PPC","keyword","lists","using","our","free","SEO",
   "and","SEM","keyword","list","generatory.","This","open",
   "source","tool","can","be","used","to","help","you","build",
   "a","list","of","relevant","keywords"
};

struct IMyValue {
   virtual std::string toString() {
      return "IMyValue";
   }
};
struct IMyDocument {
   virtual std::string toString() {return "IMyDocument";}
};

#ifdef SoftDoc_TEMPLATE
typedef SoftDoc<IMyDocument, IMyValue> SoftDocX;

struct Value : SoftDocX::Value {
   SoftDocX::ObjectString* ll;
   Value(SoftDocX::Document* document) : SoftDocX::Value(document) {
   }
   virtual std::string toString() override {
      this->ll = document->createObjectString("hhelo", 5);
      return this->ll->buffer;
   }
};

typedef SoftDocX::DocumentEx<Value> Document;
#else
typedef SoftDoc SoftDocX;
typedef SoftDocX::Document Document;
typedef SoftDocX::Value Value;
#endif

typedef SoftDocX::JSON JSON;
typedef SoftDocX::ObjectMap ObjectMap;
typedef SoftDocX::ObjectString ObjectString;

typedef std::pair<uint32_t, const char*> t_pair;

#include "stl_alloc.h"

void main() {
   Document doc;
   Chrono c;
   int count=1000;

   struct tSymbol {
      uint32_t hash;
      const char* symbol;
      int length;
   };
   int numProperties = sizeof(propertyNames)/sizeof(char*);
   tSymbol* properties = new tSymbol[numProperties];
   for(int i=0;i<numProperties;i++) {
      properties[i].hash = hash_symbol(propertyNames[i]);
      properties[i].symbol = propertyNames[i];
      properties[i].length = strlen(propertyNames[i]);
   }


   if(1){
      //obj.print();
   }
   if(0){
      c.Start();
      for(int k=0;k<count;k++) {
         std::map<uint32_t, const char*, std::less<uint32_t>, StackAllocator<t_pair> > obj;
         for(int i=0;i<numProperties;i++) {
            tSymbol& s = properties[i];
            obj.insert(t_pair(s.hash, s.symbol));
         }
      }
      printf("std::map               = %5.3g Mops\n", c.GetOpsFloat(count*numProperties, Chrono::Mops));
   }
   if(0){
      c.Start();
      for(int k=0;k<count;k++) {
         std::unordered_map<uint32_t, const char*, std::hash<uint32_t>, std::equal_to<uint32_t>, StackAllocator<t_pair> > obj;
         for(int i=0;i<numProperties;i++) {
            tSymbol& s = properties[i];
            obj.insert(t_pair(s.hash, s.symbol));
         }
      }
      printf("std::unordered_map     = %5.3g Mops\n", c.GetOpsFloat(count*numProperties, Chrono::Mops));
   }
   if(1){
      c.Start();
      for(int k=0;k<count;k++) {
         ObjectMap obj;
         for(int i=0;i<numProperties;i++) {
            tSymbol& s = properties[i];
            obj.map(s.hash, s.symbol, s.length, &doc)->value = 0;
         }
      }
      printf("insert (+hashing)      = %5.3g Mops\n", c.GetOpsFloat(count*numProperties, Chrono::Mops));
   }
   if(1){
      ObjectMap obj;
      for(int i=0;i<numProperties;i++) {
         tSymbol& s = properties[i];
         obj.map(s.symbol, &doc)->value = 0;
      }

      c.Start();
      for(int k=0;k<count;k++) {
         for(int i=0;i<numProperties;i++) {
            tSymbol& s = properties[i];
            Value& y = obj.map(s.hash, s.symbol, s.length, &doc)->value;
            y._integer++;
         }
      }
      printf("get                    = %5.3g Mops\n", c.GetOpsFloat(count*numProperties, Chrono::Mops));
   }
   if(1){
      const char* json_test = "{\"someArray\":[\"foo\",\"bar\",123],\"someObject\":{\"foo\":\"bar\",\"embedded\":{\"eleet\":1337}},\"someString\":\"Hithere!\",\"someNumber\":123,\"someBoolean\":true}";
      Value y(&doc);
      c.Start();
      for(int k=0;k<count;k++) {
         JSON::parse(y,json_test);
      }
      printf("JSON::parse            = %5.3g Mops\n", c.GetOpsFloat(count, Chrono::Mops));
   }
   if(0){
      Value x(&doc);
      x["a"] = 5;
      x["b"] = 5.6f;
      x["c"]["hello"] = 9;
      x["d"].push_back() = "x";
      x["d"].push_back() = "y";
      x["d"].push_back() = "z";
      printf("stringify x: %s\n", JSON::stringify(x).c_str());
      Value y(&doc);
      std::string sx = JSON::stringify(x);
      JSON::parse(y, "{\"d\":[\"x\",\"y\",\"z\",30],\"a\":5,\"b\":5.6,\"c\":{\"__classname\":\"MyClass\",\"hello\":9}}");
      //JSON::parse(y, sx.c_str(), sx.size());
      y["d"].push_back() = 30;
      printf("stringify y: %s\n", JSON::stringify(y).c_str());

      Value z(&doc);
      z.subtract(&x, &y);
      printf("stringify z: %s\n", JSON::stringify(z).c_str());

#ifdef SoftDoc_TEMPLATE
      printf("toString: %s\n", doc.toString().c_str());
      printf("toString: %s\n", z.toString().c_str());
#endif
   }
   getchar();
}

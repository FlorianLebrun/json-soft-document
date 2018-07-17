# json-soft-document

This project shall provide a some methods around json document: parse, stringify, etc.

## How To Use

Include _"json-soft-document\include\document.h"_ in cpp, here some exemple of what you can do.

### Stringify

``` cpp
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
```

### Parse

``` cpp
  typedef SoftDoc<> JSONDoc;
  JSONDoc::Document doc;

  const char* y_txt = "{\"d\":[\"x\\\"\",\"y\",\"z\",30],\"a\":5,\"b\":5.6,\"c\":{\"__classname\":\"MyClass\",\"hello\":9,}}";
  JSONDoc::Value y(&doc);
  JSONDoc::JSON::parse(y, y_txt, 0, "__classname");
  printf("stringify y: %s\n", JSONDoc::JSON::stringify(y, "className").c_str());

  // Result: stringify y: {"b":5.6,"d":["x\"","y","z",30],"a":5,"c":{"className":"MyClass","hello":9}}
```

### Substract

``` cpp
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
```

## Exemple

To see how it work you can run the project at https://github.com/FlorianLebrun/sat-memory-allocator-win32.git



#ifndef _Soft_Document__H_
#define _Soft_Document__H_

#include <stdint.h>
#include <sstream>
#include "document-utils.h"

template <
  bool caseInsensitive = false,
  bool orderedMap = false,
  class IDocument = SoftDoc_Utils::base_interface,
  class IValue = SoftDoc_Utils::base_interface
>
struct SoftDoc {

  static const int DefaultPage_size = 4096;

  enum class TypeID {
    Undefined,
    Integer,
    Number,
    Boolean,
    Map,
    Array,
    String,
    Symbol,
    Expression,
    Null,
  };

  enum tCharsetType {
    ASCII_charset,
    UTF8_charset,
  };

  template <int x>
  inline static intptr_t alignX(intptr_t offset) {
    return ((-offset)&(x - 1)) + offset;
  }

  inline static intptr_t alignPtr(intptr_t offset) {
    return ((-offset)&(sizeof(void*) - 1)) + offset;
  }

#  include "document-encoding.hpp"
#  include "document-base.hpp"
#  include "document-object-string.h"
#  include "document-object-symbol.h"
#  include "document-object-array.h"
#  include "document-object-expression.h"
#  include "document-object-map.h"
#  include "document-json-helper.hpp"
};

#include "document-base-impl.hpp"

#endif

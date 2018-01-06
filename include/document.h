
#ifndef _Soft_Document__H_
#define _Soft_Document__H_

#include <stdint.h>
#include <sstream>
#include "document-utils.h"

static const void* EndOfPtr = (void*)-1;
struct SoftDoc_interface {};

template <
   bool caseInsensitive = false, 
   int DefaultPage_size = 4096,
   class IDocument = SoftDoc_interface, 
   class IValue = SoftDoc_interface
> 
struct SoftDoc {

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
#  include "document-object-strings.h"
#  include "document-object-array.h"
#  include "document-object-expression.h"
#  include "document-object-map.h"
#  include "document-json-helper.hpp"
};

#define SoftDoc_TEMPLATE_DECL template<bool caseInsensitive,int DefaultPage_size,class IDocument,class IValue>
#define SoftDoc_TEMPLATE_PREFIX SoftDoc<caseInsensitive,DefaultPage_size,IDocument,IValue>
#define SoftDoc_CTOR()     SoftDoc_TEMPLATE_DECL inline SoftDoc_TEMPLATE_PREFIX::
#define SoftDoc_IMPLn(T)   SoftDoc_TEMPLATE_DECL inline T SoftDoc_TEMPLATE_PREFIX::
#define SoftDoc_IMPLi(T)   SoftDoc_TEMPLATE_DECL inline typename SoftDoc_TEMPLATE_PREFIX::T SoftDoc_TEMPLATE_PREFIX::

#include "document-base-impl.hpp"

#endif

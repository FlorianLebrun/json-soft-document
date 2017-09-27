
#ifndef _Soft_Document__H_
#define _Soft_Document__H_

#include <stdint.h>
#include <sstream>
#include "document-utils.h"

static const void* EndOfPtr = (void*)-1;

#ifdef SoftDoc_TEMPLATE
template <
   class IDocument = IDocument, 
   class IValue = IValue, 
      int DefaultPage_size = 100<<10<<10 // 4 Mb
> 
struct SoftDoc {
#else
struct SoftDoc {
   struct IValue {};
   struct IDocument {};
   static const int DefaultPage_size = 100<<10<<10; // 4 Mb
#  endif

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

#  include "document-base.hpp"
#  include "document-object-strings.h"
#  include "document-object-array.h"
#  include "document-object-expression.h"
#  include "document-object-map.h"
#  include "document-json-helper.hpp"
};

#ifdef SoftDoc_TEMPLATE
#define SoftDoc_TEMPLATE_DECL template<class IDocument,class IValue,int DefaultPage_size>
#define SoftDoc_TEMPLATE_PREFIX SoftDoc<IDocument,IValue,DefaultPage_size>
#define SoftDoc_CTOR()     SoftDoc_TEMPLATE_DECL inline SoftDoc_TEMPLATE_PREFIX::
#define SoftDoc_IMPLn(T)   SoftDoc_TEMPLATE_DECL inline T SoftDoc_TEMPLATE_PREFIX::
#define SoftDoc_IMPLi(T)   SoftDoc_TEMPLATE_DECL inline typename SoftDoc_TEMPLATE_PREFIX::T SoftDoc_TEMPLATE_PREFIX::
#else
#define SoftDoc_CTOR()     inline SoftDoc::
#define SoftDoc_IMPLn(T)   inline T SoftDoc::
#define SoftDoc_IMPLi(T)   inline SoftDoc::T SoftDoc::
#endif
#include "document-base-impl.hpp"

#endif

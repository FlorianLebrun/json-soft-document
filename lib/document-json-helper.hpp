
class JsonDocumentReader {
public:
   struct tString {
      bool symbolic;
      const char*ptr; 
      int len;
      inline bool equals(const char* str, bool isCaseSensitive = false) {
         return !(isCaseSensitive?strncmp(ptr, str, len):strnicmp(ptr, str, len));
      }
   };
   struct tToken {
      enum tId {
         FirstValue_Token,
         STRING = FirstValue_Token,
         NUMBER,
         INTEGER,
         BOOLEAN,
         NUL,
         BEGIN_ARRAY,
         BEGIN_OBJECT,
         BEGIN_XPR,
         LastValue_Token = BEGIN_XPR,

         // Not ex
         END_ARRAY, 
         END_OBJECT, 
         END_XPR,
         SEPARATOR, 
         ASSOCIATION,
         END,
      };
      tId id;
      union {
         tString _string;
         int64_t _integer;
         double _number;
         bool _boolean;
      };
   };

   enum chrMask{
      chr_NAME = 1,
      chr_NUMBER_BEGIN = 2,
      chr_NUMBER = 4,
      chr_SPECIAL = 8,
      chr_INVISIBLE = 16,
   };

   uint8_t char_def[256];

   tToken token;
   const char* buffer;
   int cursor;
   int bufferSize;
   int line;
   bool lenient;

   Document* document;

   JsonDocumentReader(const char* set_buffer, int set_bufferSize, Document* set_archive) {
      this->InitCharTable();
      this->buffer = set_buffer;
      this->bufferSize = set_bufferSize;
      this->line = 1;
      this->cursor = 0;
      this->document = set_archive;
      this->lenient = 0;
   }
   void InitCharTable() {
      memset(char_def, 0, sizeof(char_def));
      for (int c = 'a'; c <= 'z'; c++)
         char_def[c] |= chr_NAME;
      for (int c = 'A'; c <= 'Z'; c++)
         char_def[c] |= chr_NAME;
      for (int c = '0'; c <= '9'; c++)
         char_def[c] |= chr_NAME | chr_NUMBER_BEGIN | chr_NUMBER;
      char_def['-'] |= chr_NAME | chr_NUMBER_BEGIN;
      char_def['_'] |= chr_NAME;
      char_def['.'] |= chr_NAME;
      char_def['!'] |= chr_NAME;
      char_def['@'] |= chr_NAME;
      char_def['$'] |= chr_NAME;
      char_def['#'] |= chr_NAME;

      char_def['{'] |= chr_SPECIAL;
      char_def['}'] |= chr_SPECIAL;
      char_def['['] |= chr_SPECIAL;
      char_def[']'] |= chr_SPECIAL;
      char_def['('] |= chr_SPECIAL;
      char_def[')'] |= chr_SPECIAL;
      char_def['/'] |= chr_SPECIAL;
      char_def['\''] |= chr_SPECIAL;
      char_def['"'] |= chr_SPECIAL;
      char_def[','] |= chr_SPECIAL;
      char_def[':'] |= chr_SPECIAL;

      char_def['\r'] |= chr_INVISIBLE;
      char_def['\n'] |= chr_INVISIBLE;
      char_def['\t'] |= chr_INVISIBLE;
      char_def[' '] |= chr_INVISIBLE;
   }

   void logError(const char* msg, ...) {
      printf("Error(%d): %s\n", line < 0 ? this->line : line, msg);
      //if(context) context->logError(0, msg);
   }

   inline int decodeHexchar(char c)
   {
      if(c>='0' && c<='9') return c-'0';
      if(c>='a' && c<='z') return c-'a'+10;
      if(c>='A' && c<='Z') return c-'A'+10;
      return -1;
   }

   void refactorJsonString(ObjectString* obj) {
      char* dst = obj->buffer;
      for(char* src = obj->buffer;*src;src++) {
         if(*src == '\\') {
            src++;
            switch(*src) {
            case '"':*dst = '"';break;
            case '\\':*dst = '\\';break;
            case '/':*dst = '/';break;
            case 'b':*dst = '\b';break;
            case 'f':*dst = '\f';break;
            case 'n':*dst = '\n';break;
            case 'r':*dst = '\r';break;
            case 't':*dst = '\t';break;
            case 'u':
               *dst = (decodeHexchar(src[1])<<12) + (decodeHexchar(src[2])<<8) + 
                  (decodeHexchar(src[3])<<4) + (decodeHexchar(src[4])<<4);
               src += 4;
               break;
            default:*dst = *src;
            }
            dst++;
         } else {
            *dst = *src;
            dst++;
         }
      }
      *dst = '\0';
      obj->length = dst-obj->buffer;
   }

   void peekString(char endchar) {
      int start = cursor;
      while (cursor < bufferSize) {
         if (buffer[cursor] == '\\') {
            cursor++;
         }
         else if (buffer[cursor] == endchar) {
            break;
         }
         cursor++;
      }
      token.id = tToken::STRING;
      token._string.symbolic = false;
      token._string.ptr = &buffer[start];
      token._string.len = cursor - start;
      if (cursor < bufferSize) cursor++;
   }

   void peekName() {
      int start = cursor;
      while (cursor < bufferSize) {
         char c = buffer[cursor];
         uint8_t cdef = char_def[c];
         if ((cdef & chr_NAME) == 0) {
            break;
         }
         cursor++;
      }
      token.id = tToken::STRING;
      token._string.symbolic = true;
      token._string.ptr = &buffer[start];
      token._string.len = cursor - start;
      if (token._string.equals("true")) {
         token.id = tToken::BOOLEAN;
         token._boolean = true;
      } 
      else if (token._string.equals("false")) {
         token.id = tToken::BOOLEAN;
         token._boolean = false;
      } 
      else if (token._string.equals("null")) {
         token.id = tToken::NUL;
      }
   }

   int32_t peekIntegerDigits(bool isSigned, int64_t &acc) {
      char c = (cursor < bufferSize)?buffer[cursor]:'\0';

      bool sign = false;
      if(isSigned && c =='-') {
         sign = true;
         cursor++;
      }

      int exp = 0;
      while (cursor < bufferSize) {
         c = buffer[cursor];
         uint8_t cdef = char_def[c];
         if (cdef & chr_NUMBER) {
            acc = (acc*10)+(c-'0');
            exp++;
         }
         else {
            break;
         }
         cursor++;
      }
      if(sign) acc = -acc;
      return exp;
   }

   void peekNumber() {
      char c;
      int64_t intPart = 0;
      int expPart = 0;

      peekIntegerDigits(true, intPart);
      c = (cursor < bufferSize)?buffer[cursor]:'\0';

      if(c =='.') {
         cursor++;
         expPart = -peekIntegerDigits(false, intPart);
         c = (cursor < bufferSize)?buffer[cursor]:'\0';
      }

      if(c =='e') {
         int64_t expAcc = 0;
         cursor++;
         peekIntegerDigits(true, expAcc);
         expPart += expAcc;
      }

      if(expPart) {
         token.id = tToken::NUMBER;
         token._number = intPart*pow(10, expPart);
      }
      else {
         token.id = tToken::INTEGER;
         token._integer = intPart*pow(10, expPart);
      }
   }

   void peekToken() {

      // Parse token type
      while (cursor < bufferSize) {
         char c = buffer[cursor];
         uint8_t cdef = char_def[c];

         // Parse a number
         if ((cdef & chr_NUMBER_BEGIN) != 0) {
            return peekNumber();
         }
         // Parse a name
         else if ((cdef & chr_NAME) != 0) {
            return peekName();
         }
         // Parse a keychar
         else if (cdef == chr_SPECIAL) {
            cursor++;
            switch (c) {
            case '"':
            case '\'':
               return peekString(c);
            case '[':
               token.id = tToken::BEGIN_ARRAY;
               return;
            case ']':
               token.id = tToken::END_ARRAY;
               return;
            case '{':
               token.id = tToken::BEGIN_OBJECT;
               return;
            case '}':
               token.id = tToken::END_OBJECT;
               return;
            case '(':
               token.id = tToken::BEGIN_XPR;
               return;
            case ')':
               token.id = tToken::END_XPR;
               return;
            case ',':
               token.id = tToken::SEPARATOR;
               return;
            case ':':
               token.id = tToken::ASSOCIATION;
               return;
            case '/':
               if (cursor >= bufferSize)
                  break;
               if (buffer[cursor] == '*') {
                  cursor++;
                  while (cursor < bufferSize && (buffer[cursor - 1] != '*' || buffer[cursor] != '/')) {
                     if (buffer[cursor] == '\n')
                        line++;
                     cursor++;
                  }
                  cursor++;
               } else if (buffer[cursor] == '/') {
                  cursor++;
                  while (cursor < bufferSize && buffer[cursor] != '\n') {
                     cursor++;
                  }
               } else {
                  logError("unexpected char '/'");
               }

               break;
            }
         }
         // Parse a ignored char
         else {
            if (c == '\n')
               line++;
            if (cdef != chr_INVISIBLE) {
               logError("unexpected char '%c'", (char) c);
            }
            cursor++;
         }
      }
      token.id = tToken::END;
      return;
   }

   void peekProperty(ObjectMap* obj) {

      // Check if it is really a statement begin
      if (token.id == tToken::STRING) {
         if(token._string.equals("__classname")) {
            peekToken();

            if(token.id == tToken::ASSOCIATION) {
               peekToken();
            } else {
               logError("a expected statement is mis formed, it shall be define by 'name:value'");
            }

            if(token.id == tToken::STRING) {
               if(!obj->classname) {
                  obj->classname = document->createObjectSymbol(token._string.ptr, token._string.len);
               } else {
                  logError("classname cannot be defined twice");
               }
               peekToken();
            } else {
               logError("classname value shall be a string");
               Value className(document);
               peekValue(&className);
            }
         }
         else {
            Property* prop = obj->map(token._string.ptr, token._string.len, document);
            if(prop->value.typeID != TypeID::Undefined) {
               logError("an element is declared more than ones");
            }
            peekToken();

            if(token.id == tToken::ASSOCIATION) {
               peekToken();
            } else {
               logError("a expected statement is mis formed, it shall be define by 'name:value'");
            }

            peekValue(&prop->value);
         }
      }
   }


   void peekObjectMap(Value* value, ObjectSymbol* classname) {
      ObjectMap* obj = document->createObjectMap();
      obj->classname = classname;
      value->typeID = TypeID::Map;
      value->_object = obj;

      // Parse collection statements
      if (token.id == tToken::BEGIN_OBJECT) {
         peekToken();
         while (token.id != tToken::END_OBJECT) {
            if (token.id == tToken::STRING) {
               peekProperty(obj);
            }
            else if(!this->lenient) {
               logError("the name/value list is mis formated");
            }
            if (token.id != tToken::SEPARATOR) break;
            peekToken();
         }
         if (token.id == tToken::END_OBJECT) {
            peekToken();
         } else {
            logError("a collection shall end with ')'");
         }
      } else {
         logError("collection expected");
      }
   }

   void peekObjectArray(Value* value) {
      ObjectArray* obj = document->createObjectArray();
      value->typeID = TypeID::Array;
      value->_object = obj;

      // Parse array values
      if (token.id == tToken::BEGIN_ARRAY) {
         peekToken();
         while (token.id != tToken::END_ARRAY) {
            if(token.id <= tToken::LastValue_Token) {
               Item* item = obj->push_back(document);
               peekValue(&item->value);
            }
            else if(!this->lenient) {
               logError("the value list is mis formated");
            }
            if (token.id != tToken::SEPARATOR) break;
            peekToken();
         }
         if (token.id == tToken::END_ARRAY) {
            peekToken();
         } else {
            logError("an array shall end with ']'");
         }
      } else {
         logError("array expected");
      }
   }

   void peekObjectExpression(Value* value, ObjectSymbol* symbol) {
      ObjectArray* obj = document->createObjectArray();
      value->typeID = TypeID::Array;
      value->_object = obj;

      // Parse array values
      if (token.id == tToken::BEGIN_XPR) {
         peekToken();
         while (token.id != tToken::END_XPR) {
            Item* arg = obj->push_back(document);
            if (token.id != tToken::SEPARATOR) {
               peekValue(&arg->value);
            }
            if (token.id != tToken::SEPARATOR) break;
            peekToken();
         }
         if (token.id == tToken::END_XPR) {
            peekToken();
         } else {
            logError("an expression value shall end with ']'");
         }
      } else {
         logError("expression value expected");
      }
   }

   void peekValue(Value* value) {

      // Parse String
      switch(token.id) {
      case tToken::STRING: 
         {
            tString _string = token._string;
            peekToken();
            if (token.id == tToken::BEGIN_XPR) {
               peekObjectExpression(value, document->createObjectSymbol(_string.ptr, _string.len));
            }
            else if (token.id == tToken::BEGIN_OBJECT) {
               peekObjectMap(value, document->createObjectSymbol(_string.ptr, _string.len));
            }
            else if(_string.symbolic) {
               value->typeID = TypeID::Symbol;
               value->_object = document->createObjectSymbol(_string.ptr, _string.len);
            }
            else {
               value->typeID = TypeID::String;
               value->_object = document->createObjectString(_string.ptr, _string.len);
            }
         }break;
         // Parse Number
      case tToken::NUMBER: 
         value->typeID = TypeID::Number;
         value->_number = token._number;
         peekToken();
         break;
         // Parse Integer
      case tToken::INTEGER: 
         value->typeID = TypeID::Integer;
         value->_integer = token._integer;
         peekToken();
         break;
         // Parse Boolean
      case tToken::BOOLEAN: 
         value->typeID = TypeID::Boolean;
         value->_boolean = token._boolean;
         peekToken();
         break;
         // Parse Null
      case tToken::NUL: 
         value->typeID = TypeID::Null;
         value->_object = 0;
         peekToken();
         break;
         // Parse Collection DataValue
      case tToken::BEGIN_OBJECT: 
         peekObjectMap(value, 0);
         break;
         // Parse Array DataValue
      case tToken::BEGIN_ARRAY: 
         peekObjectArray(value);
         break;
         // When token unexpected
      default: 
         logError("The data is mis formated");
         break;
      }
   }
};

struct JsonDocumentWriter {
   std::stringstream out;
   void stringifyMap(ObjectMap* obj) {
      ObjectMap::iterator it(obj);
      int first = 1;
      if(obj->classname) {
         out<<'"'<<obj->classname->buffer<<'"';
      }
      out<<"{";
      for(Property* prop=it.begin();prop;prop=it.next()) {
         if(!first) out<<",";
         else first = 0;
         out<<'"'<<prop->key->buffer<<'"';
         out<<':';
         stringify(prop->value);
         prop = prop->next;
      }
      out<<"}";
   }
   void stringifyArray(ObjectArray* obj) {
      int first = 1;
      out<<"[";
      for(Item* item = obj->firstItem;item;item = item->next) {
         if(!first) out<<",";
         else first = 0;
         stringify(item->value);
      }
      out<<"]";
   }
   void stringifyString(ObjectString* obj) {
      out<<'"'<<obj->buffer<<'"';
   }
   void stringifySymbol(ObjectSymbol* obj) {
      out<<obj->buffer;
   }
   void stringify(Value& x) {
      switch(x.typeID) {
      case TypeID::Integer:
         out<<x._integer;
         return;
      case TypeID::Number:
         out<<x._number;
         return;
      case TypeID::Boolean:
         if(x._boolean) out<<"true";
         else out<<"false";
         return;
      case TypeID::Null:
         out<<"null";
         return;
      case TypeID::Undefined:
         out<<"undefined";
         return;
      case TypeID::Map:
         return this->stringifyMap(x._map);
      case TypeID::Array:
         return this->stringifyArray(x._array);
      case TypeID::String:
         return this->stringifyString(x._string);
      case TypeID::Symbol:
         return this->stringifySymbol(x._symbol);
      }
   }
};

struct JSON {
   static void parse(Value& value, const char* buffer, int length = 0) {
      JsonDocumentReader reader(buffer, length?length:strlen(buffer), value.document);
      reader.peekToken();
      reader.peekValue(&value);
   }
   static std::string stringify(Value &x) {
      JsonDocumentWriter writer;
      writer.stringify(x);
      return writer.out.str();
   }
};

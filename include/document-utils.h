
#include <stdint.h>
#include <string.h>

namespace SoftDocument {
  static const void* EndOfPtr = (void*)-1;
  struct base_interface {};

	template <intptr_t x>
	inline static intptr_t alignX(intptr_t offset) {
		return ((-offset)&(x - 1)) + offset;
	}

	inline static intptr_t alignPtr(intptr_t offset) {
		return ((-offset)&(sizeof(void*) - 1)) + offset;
	}
  
  uint32_t hash_utf8_crc31(uint32_t crc, const void *buf, size_t size);
  uint32_t hash_utf8_icrc31(uint32_t crc, const void *buf, size_t size);
  uint32_t hash_murmur3_31(const uint8_t* key, size_t len, uint32_t seed);
  uint32_t hash_jenkins_31(const uint8_t* key, size_t length);

	struct EncodingBuffer {
		uint8_t* start;
		uint8_t* end;
		EncodingBuffer(void* buffer) {
			this->start = (uint8_t*)buffer;
			this->end = &((uint8_t*)buffer)[strlen((char*)buffer)];
		}
		EncodingBuffer(void* buffer, size_t size) {
			this->start = (uint8_t*)buffer;
			this->end = &((uint8_t*)buffer)[size];
		}
		EncodingBuffer(void* start, void* end) {
			this->start = (uint8_t*)start;
			this->end = (uint8_t*)end;
		}
	};

	//   Char. number range  |        UTF-8 octet sequence 
	//      (hexadecimal)    |              (binary) 
	//   --------------------+--------------------------------------------- 
	//   0000 0000-0000 007F | 0xxxxxxx 
	//   0000 0080-0000 07FF | 110xxxxx 10xxxxxx 
	//   0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx 
	//   0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
	static size_t Ascii_to_Utf8(EncodingBuffer& src, EncodingBuffer& dst) {
		uint8_t *s = src.start, *s_end = src.end;
		uint8_t *d = dst.start, *d_end = dst.end - 1;
		while (s < s_end && d < d_end) {
			uint8_t c0 = (s++)[0];
			if (!(c0 & 0x80)) { // 1 byte
				(d++)[0] = c0;
			}
			else { // 2 bytes
				(d++)[0] = (c0 & 0x40) ? 0xC3 : 0xC2;
				(d++)[0] = (c0 & 0x3f) | 0x80;
			}
		}
		src.start = s;
		dst.start = d;
		return size_t(s_end - s);
	}

	//   Char. number range  |        UTF-8 octet sequence 
	//      (hexadecimal)    |              (binary) 
	//   --------------------+--------------------------------------------- 
	//   0000 0000-0000 007F | 0xxxxxxx 
	//   0000 0080-0000 07FF | 110xxxxx 10xxxxxx 
	//   0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx 
	//   0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
	static size_t Utf8_to_Ascii(EncodingBuffer& src, EncodingBuffer& dst) {
		uint8_t *s = src.start, *s_end = src.end;
		uint8_t *d = dst.start, *d_end = dst.end;
		while (s < s_end && d < d_end) {
			uint8_t c0 = s[0];
			if (!(c0 & 0x80)) { // 1 byte
				(d++)[0] = c0;
				s++;
			}
			else if (!(c0 & 0x40)) { // error: uncomplete character
				(d++)[0] = '?';
				s += 1;
			}
			else if (!(c0 & 0x20)) { // 2 bytes
				uint8_t c1 = s[1]; // WARNING: read overflow risk
				if (!(c0 & 0x1C)) {
					(d++)[0] = (c0 & 0x3F) | (c1 << 6);
				}
				else {
					(d++)[0] = '?';
				}
				s += 2;
			}
			else if (!(c0 & 0x10)) { // 3 bytes
				(d++)[0] = '?';
				s += 3;
			}
			else if (!(c0 & 0x08)) { // 4 bytes
				(d++)[0] = '?';
				s += 4;
			}
		}
		src.start = s;
		dst.start = d;
		return size_t(s_end - s);
	}
	
	template <bool caseInsensitive>
	struct SymbolBase;

	template <>
	struct SymbolBase<true> {
		static uint32_t hash_symbol(const char* str, size_t len) {
			return SoftDocument::hash_utf8_icrc31(0, str, len);
		}
	protected:
		static int compare_bytes(const char* buffer1, const char* buffer2, size_t length) {
			return _strnicmp(buffer1, buffer2, length);
		}
	};

	template <>
	struct SymbolBase<false> {
		static uint32_t hash_symbol(const char* str, size_t len) {
			return SoftDocument::hash_murmur3_31((uint8_t*)str, len, 0);
		}
	protected:
		static int compare_bytes(const char* buffer1, const char* buffer2, size_t length) {
			return memcmp(buffer1, buffer2, length);
		}
	};

}
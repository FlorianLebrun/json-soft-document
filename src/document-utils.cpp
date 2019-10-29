
#include "./document-utils.h"

static const uint32_t c_hash31Mask = 0x7fffffff;

static const uint32_t crc32Table[256] = {
  0xEDB88320,0xF26B8303,0xE13B70F7,0x1350F3F4,0xC79A971F,0x35F1141C,0x26A1E7E8,0xD4CA64EB,
  0x8AD958CF,0x78B2DBCC,0x6BE22838,0x9989AB3B,0x4D43CFD0,0xBF284CD3,0xAC78BF27,0x5E133C24,
  0x105EC76F,0xE235446C,0xF165B798,0x030E349B,0xD7C45070,0x25AFD373,0x36FF2087,0xC494A384,
  0x9A879FA0,0x68EC1CA3,0x7BBCEF57,0x89D76C54,0x5D1D08BF,0xAF768BBC,0xBC267848,0x4E4DFB4B,
  0x20BD8EDE,0xD2D60DDD,0xC186FE29,0x33ED7D2A,0xE72719C1,0x154C9AC2,0x061C6936,0xF477EA35,
  0xAA64D611,0x580F5512,0x4B5FA6E6,0xB93425E5,0x6DFE410E,0x9F95C20D,0x8CC531F9,0x7EAEB2FA,
  0x30E349B1,0xC288CAB2,0xD1D83946,0x23B3BA45,0xF779DEAE,0x05125DAD,0x1642AE59,0xE4292D5A,
  0xBA3A117E,0x4851927D,0x5B016189,0xA96AE28A,0x7DA08661,0x8FCB0562,0x9C9BF696,0x6EF07595,
  0x417B1DBC,0xB3109EBF,0xA0406D4B,0x522BEE48,0x86E18AA3,0x748A09A0,0x67DAFA54,0x95B17957,
  0xCBA24573,0x39C9C670,0x2A993584,0xD8F2B687,0x0C38D26C,0xFE53516F,0xED03A29B,0x1F682198,
  0x5125DAD3,0xA34E59D0,0xB01EAA24,0x42752927,0x96BF4DCC,0x64D4CECF,0x77843D3B,0x85EFBE38,
  0xDBFC821C,0x2997011F,0x3AC7F2EB,0xC8AC71E8,0x1C661503,0xEE0D9600,0xFD5D65F4,0x0F36E6F7,
  0x61C69362,0x93AD1061,0x80FDE395,0x72966096,0xA65C047D,0x5437877E,0x4767748A,0xB50CF789,
  0xEB1FCBAD,0x197448AE,0x0A24BB5A,0xF84F3859,0x2C855CB2,0xDEEEDFB1,0xCDBE2C45,0x3FD5AF46,
  0x7198540D,0x83F3D70E,0x90A324FA,0x62C8A7F9,0xB602C312,0x44694011,0x5739B3E5,0xA55230E6,
  0xFB410CC2,0x092A8FC1,0x1A7A7C35,0xE811FF36,0x3CDB9BDD,0xCEB018DE,0xDDE0EB2A,0x2F8B6829,
  0x82F63B78,0x709DB87B,0x63CD4B8F,0x91A6C88C,0x456CAC67,0xB7072F64,0xA457DC90,0x563C5F93,
  0x082F63B7,0xFA44E0B4,0xE9141340,0x1B7F9043,0xCFB5F4A8,0x3DDE77AB,0x2E8E845F,0xDCE5075C,
  0x92A8FC17,0x60C37F14,0x73938CE0,0x81F80FE3,0x55326B08,0xA759E80B,0xB4091BFF,0x466298FC,
  0x1871A4D8,0xEA1A27DB,0xF94AD42F,0x0B21572C,0xDFEB33C7,0x2D80B0C4,0x3ED04330,0xCCBBC033,
  0xA24BB5A6,0x502036A5,0x4370C551,0xB11B4652,0x65D122B9,0x97BAA1BA,0x84EA524E,0x7681D14D,
  0x2892ED69,0xDAF96E6A,0xC9A99D9E,0x3BC21E9D,0xEF087A76,0x1D63F975,0x0E330A81,0xFC588982,
  0xB21572C9,0x407EF1CA,0x532E023E,0xA145813D,0x758FE5D6,0x87E466D5,0x94B49521,0x66DF1622,
  0x38CC2A06,0xCAA7A905,0xD9F75AF1,0x2B9CD9F2,0xFF56BD19,0x0D3D3E1A,0x1E6DCDEE,0xEC064EED,
  0xC38D26C4,0x31E6A5C7,0x22B65633,0xD0DDD530,0x0417B1DB,0xF67C32D8,0xE52CC12C,0x1747422F,
  0x49547E0B,0xBB3FFD08,0xA86F0EFC,0x5A048DFF,0x8ECEE914,0x7CA56A17,0x6FF599E3,0x9D9E1AE0,
  0xD3D3E1AB,0x21B862A8,0x32E8915C,0xC083125F,0x144976B4,0xE622F5B7,0xF5720643,0x07198540,
  0x590AB964,0xAB613A67,0xB831C993,0x4A5A4A90,0x9E902E7B,0x6CFBAD78,0x7FAB5E8C,0x8DC0DD8F,
  0xE330A81A,0x115B2B19,0x020BD8ED,0xF0605BEE,0x24AA3F05,0xD6C1BC06,0xC5914FF2,0x37FACCF1,
  0x69E9F0D5,0x9B8273D6,0x88D28022,0x7AB90321,0xAE7367CA,0x5C18E4C9,0x4F48173D,0xBD23943E,
  0xF36E6F75,0x0105EC76,0x12551F82,0xE03E9C81,0x34F4F86A,0xC69F7B69,0xD5CF889D,0x27A40B9E,
  0x79B737BA,0x8BDCB4B9,0x988C474D,0x6AE7C44E,0xBE2DA0A5,0x4C4623A6,0x5F16D052,0xAD7D5351,
};

uint32_t SoftDocument::hash_utf8_crc31(uint32_t crc, const void *buf, size_t size)
{
  const uint8_t *p = (const uint8_t*)buf;
  while (size--) {
    const uint8_t x = crc ^ (*p++);
    crc = crc32Table[x] ^ (crc >> 8);
  }
  return crc & c_hash31Mask;
}


static const uint32_t icrc32Table[256] = {
  0xEDB88320,0xF26B8303,0xE13B70F7,0x1350F3F4,0xC79A971F,0x35F1141C,0x26A1E7E8,0xD4CA64EB,
  0x8AD958CF,0x78B2DBCC,0x6BE22838,0x9989AB3B,0x4D43CFD0,0xBF284CD3,0xAC78BF27,0x5E133C24,
  0x105EC76F,0xE235446C,0xF165B798,0x030E349B,0xD7C45070,0x25AFD373,0x36FF2087,0xC494A384,
  0x9A879FA0,0x68EC1CA3,0x7BBCEF57,0x89D76C54,0x5D1D08BF,0xAF768BBC,0xBC267848,0x4E4DFB4B,
  0x20BD8EDE,0xD2D60DDD,0xC186FE29,0x33ED7D2A,0xE72719C1,0x154C9AC2,0x061C6936,0xF477EA35,
  0xAA64D611,0x580F5512,0x4B5FA6E6,0xB93425E5,0x6DFE410E,0x9F95C20D,0x8CC531F9,0x7EAEB2FA,
  0x30E349B1,0xC288CAB2,0xD1D83946,0x23B3BA45,0xF779DEAE,0x05125DAD,0x1642AE59,0xE4292D5A,
  0xBA3A117E,0x4851927D,0x5B016189,0xA96AE28A,0x7DA08661,0x8FCB0562,0x9C9BF696,0x6EF07595,
  0x417B1DBC,0xB3109EBF,0xA0406D4B,0x522BEE48,0x86E18AA3,0x748A09A0,0x67DAFA54,0x95B17957,
  0xCBA24573,0x39C9C670,0x2A993584,0xD8F2B687,0x0C38D26C,0xFE53516F,0xED03A29B,0x1F682198,
  0x5125DAD3,0xA34E59D0,0xB01EAA24,0x42752927,0x96BF4DCC,0x64D4CECF,0x77843D3B,0x85EFBE38,
  0xDBFC821C,0x2997011F,0x3AC7F2EB,0xC8AC71E8,0x1C661503,0xEE0D9600,0xFD5D65F4,0x0F36E6F7,
  0x61C69362,0xB3109EBF,0xA0406D4B,0x522BEE48,0x86E18AA3,0x748A09A0,0x67DAFA54,0x95B17957,
  0xCBA24573,0x39C9C670,0x2A993584,0xD8F2B687,0x0C38D26C,0xFE53516F,0xED03A29B,0x1F682198,
  0x5125DAD3,0xA34E59D0,0xB01EAA24,0x42752927,0x96BF4DCC,0x64D4CECF,0x77843D3B,0x85EFBE38,
  0xDBFC821C,0x2997011F,0x3AC7F2EB,0xE811FF36,0x3CDB9BDD,0xCEB018DE,0xDDE0EB2A,0x2F8B6829,
  0x82F63B78,0x709DB87B,0x63CD4B8F,0x91A6C88C,0x456CAC67,0xB7072F64,0xA457DC90,0x563C5F93,
  0x082F63B7,0xFA44E0B4,0xE9141340,0x1B7F9043,0xCFB5F4A8,0x3DDE77AB,0x2E8E845F,0xDCE5075C,
  0x92A8FC17,0x60C37F14,0x73938CE0,0x81F80FE3,0x55326B08,0xA759E80B,0xB4091BFF,0x466298FC,
  0x1871A4D8,0xEA1A27DB,0xF94AD42F,0x0B21572C,0xDFEB33C7,0x2D80B0C4,0x3ED04330,0xCCBBC033,
  0xA24BB5A6,0x502036A5,0x4370C551,0xB11B4652,0x65D122B9,0x97BAA1BA,0x84EA524E,0x7681D14D,
  0x2892ED69,0xDAF96E6A,0xC9A99D9E,0x3BC21E9D,0xEF087A76,0x1D63F975,0x0E330A81,0xFC588982,
  0xB21572C9,0x407EF1CA,0x532E023E,0xA145813D,0x758FE5D6,0x87E466D5,0x94B49521,0x66DF1622,
  0x38CC2A06,0xCAA7A905,0xD9F75AF1,0x2B9CD9F2,0xFF56BD19,0x0D3D3E1A,0x1E6DCDEE,0xEC064EED,
  0xC38D26C4,0x31E6A5C7,0x22B65633,0xD0DDD530,0x0417B1DB,0xF67C32D8,0xE52CC12C,0x1747422F,
  0x49547E0B,0xBB3FFD08,0xA86F0EFC,0x5A048DFF,0x8ECEE914,0x7CA56A17,0x6FF599E3,0x9D9E1AE0,
  0xD3D3E1AB,0x21B862A8,0x32E8915C,0xC083125F,0x144976B4,0xE622F5B7,0xF5720643,0x07198540,
  0x590AB964,0xAB613A67,0xB831C993,0x4A5A4A90,0x9E902E7B,0x6CFBAD78,0x7FAB5E8C,0x8DC0DD8F,
  0xE330A81A,0x115B2B19,0x020BD8ED,0xF0605BEE,0x24AA3F05,0xD6C1BC06,0xC5914FF2,0x37FACCF1,
  0x69E9F0D5,0x9B8273D6,0x88D28022,0x7AB90321,0xAE7367CA,0x5C18E4C9,0x4F48173D,0xBD23943E,
  0xF36E6F75,0x0105EC76,0x12551F82,0xE03E9C81,0x34F4F86A,0xC69F7B69,0xD5CF889D,0x27A40B9E,
  0x79B737BA,0x8BDCB4B9,0x988C474D,0x6AE7C44E,0xBE2DA0A5,0x4C4623A6,0x5F16D052,0xAD7D5351,
};

static const uint8_t ichrTable[256] = {
  0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
  16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
  32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,
  48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,
  64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,
  80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,
  96,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,
  80,81,82,83,84,85,86,87,88,89,90,123,124,125,126,127,
  128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
  144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,
  160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,
  176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
  192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,
  208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,
  224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,
  240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255,
};

uint32_t SoftDocument::hash_utf8_icrc31(uint32_t crc, const void *buf, size_t size)
{
  const uint8_t *p = (const uint8_t*)buf;
  while (size--) {
    const uint8_t x = crc ^ ichrTable[*p++];
    crc = icrc32Table[x] ^ (crc >> 8);
  }
  return crc & c_hash31Mask;
}

uint32_t SoftDocument::hash_murmur3_31(const uint8_t* key, size_t len, uint32_t seed) {
  uint32_t h = seed;
  if (len > 3) {
    const uint32_t* key_x4 = (const uint32_t*)key;
    size_t i = len >> 2;
    do {
      uint32_t k = *key_x4++;
      k *= 0xcc9e2d51;
      k = (k << 15) | (k >> 17);
      k *= 0x1b873593;
      h ^= k;
      h = (h << 13) | (h >> 19);
      h = (h * 5) + 0xe6546b64;
    } while (--i);
    key = (const uint8_t*)key_x4;
  }
  if (len & 3) {
    size_t i = len & 3;
    uint32_t k = 0;
    key = &key[i - 1];
    do {
      k <<= 8;
      k |= *key--;
    } while (--i);
    k *= 0xcc9e2d51;
    k = (k << 15) | (k >> 17);
    k *= 0x1b873593;
    h ^= k;
  }
  h ^= len;
  h ^= h >> 16;
  h *= 0x85ebca6b;
  h ^= h >> 13;
  h *= 0xc2b2ae35;
  h ^= h >> 16;
  return h & c_hash31Mask;
}

uint32_t SoftDocument::hash_jenkins_31(const uint8_t* key, size_t length) {
  size_t i = 0;
  uint32_t hash = 0;
  while (i != length) {
    hash += key[i++];
    hash += hash << 10;
    hash ^= hash >> 6;
  }
  hash += hash << 3;
  hash ^= hash >> 11;
  hash += hash << 15;
  return hash & c_hash31Mask;
}

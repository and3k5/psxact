#ifndef __psxact_memory__
#define __psxact_memory__


#include <cstdint>
#include <cstring>
#include <cstdio>
#include "addressable.hpp"


template<int bits>
struct memory_t : public addressable_t {
  
  static const int mask = (1 << bits) - 1;
  static const int size = (1 << bits);

  union {
    uint8_t  *buffer;
    uint16_t *buffer16;
    uint32_t *buffer32;
  };

  memory_t()
    : addressable_t("memory") {

    buffer = new uint8_t[size]();
  }

  ~memory_t() {
    delete [] buffer;
  }

  uint32_t io_read_byte(uint32_t address) {
    return buffer[(address & mask) / 1];
  }

  uint32_t io_read_half(uint32_t address) {
    return buffer16[(address & mask) / 2];
  }

  uint32_t io_read_word(uint32_t address) {
    return buffer32[(address & mask) / 4];
  }

  void io_write_byte(uint32_t address, uint32_t data) {
    buffer[(address & mask) / 1] = uint8_t(data);
  }

  void io_write_half(uint32_t address, uint32_t data) {
    buffer16[(address & mask) / 2] = uint16_t(data);
  }

  void io_write_word(uint32_t address, uint32_t data) {
    buffer32[(address & mask) / 4] = data;
  }

  bool load_blob(const char *filename) {
    if (FILE* file = fopen(filename, "rb+")) {
      fread(buffer, sizeof(uint8_t), size, file);
      fclose(file);
      return true;
    }
    else {
      printf("unable to load '%s'\n", filename);

      return false;
    }
  }
};


#endif // __psxact_memory__

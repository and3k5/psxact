#include <cstdio>
#include "expansion/exp3.hpp"


exp3_t::exp3_t()
  : addressable_t("exp3") {
}


uint32_t exp3_t::io_read_byte(uint32_t address) {
  printf("[exp3] io_read_byte(0x%08x)\n", address);
  return 0;
}


uint32_t exp3_t::io_read_half(uint32_t address) {
  printf("[exp3] io_read_half(0x%08x)\n", address);
  return 0;
}


uint32_t exp3_t::io_read_word(uint32_t address) {
  printf("[exp3] io_read_word(0x%08x)\n", address);
  return 0;
}


void exp3_t::io_write_byte(uint32_t address, uint32_t data) {
  printf("[exp3] io_write_byte(0x%08x, 0x%08x)\n", address, data);
}


void exp3_t::io_write_half(uint32_t address, uint32_t data) {
  printf("[exp3] io_write_half(0x%08x, 0x%08x)\n", address, data);
}


void exp3_t::io_write_word(uint32_t address, uint32_t data) {
  printf("[exp3] io_write_word(0x%08x, 0x%08x)\n", address, data);
}

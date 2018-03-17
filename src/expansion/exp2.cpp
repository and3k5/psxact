#include <cstdio>
#include "expansion/exp2.hpp"


exp2_t::exp2_t()
  : addressable_t("exp2") {
}


uint32_t exp2_t::io_read_byte(uint32_t address) {
  printf("[exp2] io_read_byte(0x%08x)\n", address);
  return 0;
}


uint32_t exp2_t::io_read_half(uint32_t address) {
  printf("[exp2] io_read_half(0x%08x)\n", address);
  return 0;
}


uint32_t exp2_t::io_read_word(uint32_t address) {
  printf("[exp2] io_read_word(0x%08x)\n", address);
  return 0;
}


void exp2_t::io_write_byte(uint32_t address, uint32_t data) {
  printf("[exp2] io_write_byte(0x%08x, 0x%08x)\n", address, data);
}


void exp2_t::io_write_half(uint32_t address, uint32_t data) {
  printf("[exp2] io_write_half(0x%08x, 0x%08x)\n", address, data);
}


void exp2_t::io_write_word(uint32_t address, uint32_t data) {
  printf("[exp2] io_write_word(0x%08x, 0x%08x)\n", address, data);
}

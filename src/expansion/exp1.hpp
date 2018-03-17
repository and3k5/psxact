#ifndef __psxact_expansion_exp1__
#define __psxact_expansion_exp1__


#include "addressable.hpp"


struct exp1_t : public addressable_t {

  exp1_t();

  uint32_t io_read_byte(uint32_t address);

  uint32_t io_read_half(uint32_t address);

  uint32_t io_read_word(uint32_t address);

  void io_write_byte(uint32_t address, uint32_t data);

  void io_write_half(uint32_t address, uint32_t data);

  void io_write_word(uint32_t address, uint32_t data);

};


#endif // __psxact_expansion_exp1__

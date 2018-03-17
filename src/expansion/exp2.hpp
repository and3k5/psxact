#ifndef __psxact_expansion_exp2__
#define __psxact_expansion_exp2__


#include "addressable.hpp"


struct exp2_t : public addressable_t {

  exp2_t();

  uint32_t io_read_byte(uint32_t address);

  uint32_t io_read_half(uint32_t address);

  uint32_t io_read_word(uint32_t address);

  void io_write_byte(uint32_t address, uint32_t data);

  void io_write_half(uint32_t address, uint32_t data);

  void io_write_word(uint32_t address, uint32_t data);

};


#endif // __psxact_expansion_exp2__

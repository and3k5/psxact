#ifndef __psxact_addressable__
#define __psxact_addressable__


#include <cstdint>
#include <cstdio>


class addressable_t {

  const char *name;

public:

  addressable_t(const char *name);

  virtual uint32_t io_read_byte(uint32_t address);

  virtual uint32_t io_read_half(uint32_t address);

  virtual uint32_t io_read_word(uint32_t address);

  virtual void io_write_byte(uint32_t address, uint32_t data);

  virtual void io_write_half(uint32_t address, uint32_t data);

  virtual void io_write_word(uint32_t address, uint32_t data);

};


#endif // __psxact_addressable__

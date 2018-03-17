#ifndef __psxact_vram__
#define __psxact_vram__


#include <cstdint>
#include "memory.hpp"


class vram_t {

  memory_t<20> buffer;

public:

  uint16_t *get_pointer(int32_t x, int32_t y);

  uint16_t read(int32_t x, int32_t y);

  void write(int32_t x, int32_t y, uint16_t data);

};


#endif // __psxact_vram__

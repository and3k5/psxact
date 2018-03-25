#include <cassert>
#include <cstring>
#include <exception>
#include "addressable.hpp"
#include "console.hpp"
#include "cdrom/cdrom.hpp"
#include "counter/counter.hpp"
#include "cpu/cpu.hpp"
#include "dma/dma.hpp"
#include "expansion/exp1.hpp"
#include "expansion/exp2.hpp"
#include "expansion/exp3.hpp"
#include "gpu/gpu.hpp"
#include "input/input.hpp"
#include "mdec/mdec.hpp"
#include "spu/spu.hpp"
#include "limits.hpp"
#include "utility.hpp"


console_t::console_t(const char *bios_file_name, const char *game_file_name) {
  cdrom = new cdrom_t(this, game_file_name);
  counter = new counter_t(this);
  cpu = new cpu_t(this);
  dma = new dma_t(this, this);
  gpu = new gpu_t();
  input = new input_t(this);
  mdec = new mdec_t();
  spu = new spu_t();

  exp1 = new exp1_t();
  exp2 = new exp2_t();
  exp3 = new exp3_t();

  bios.load_blob(bios_file_name);
}


void console_t::send(interrupt_type_t flag) {
  int istat = cpu->get_istat() | int(flag);
  cpu->set_istat(istat);
}


addressable_t *console_t::decode(uint32_t address) {
  if (limits::between<0x00000000, 0x007fffff>(address)) { return &wram; }
  if (limits::between<0x1fc00000, 0x1fc7ffff>(address)) { return &bios; }
  if (limits::between<0x1f800000, 0x1f8003ff>(address)) { return &dmem; }

  if (limits::between<0x1f801040, 0x1f80104f>(address)) { return input; }
  if (limits::between<0x1f801070, 0x1f801077>(address)) { return cpu; }
  if (limits::between<0x1f801080, 0x1f8010ff>(address)) { return dma; }
  if (limits::between<0x1f801100, 0x1f80113f>(address)) { return counter; }
  if (limits::between<0x1f801800, 0x1f801803>(address)) { return cdrom; }
  if (limits::between<0x1f801810, 0x1f801817>(address)) { return gpu; }
  if (limits::between<0x1f801820, 0x1f801827>(address)) { return mdec; }
  if (limits::between<0x1f801c00, 0x1f801fff>(address)) { return spu; }

  if (limits::between<0x1f000000, 0x1f7fffff>(address)) { return exp1; } // expansion region 1
  if (limits::between<0x1f802000, 0x1f802fff>(address)) { return exp2; } // expansion region 2
  if (limits::between<0x1fa00000, 0x1fbfffff>(address)) { return exp3; } // expansion region 3

  return nullptr;
}


static uint32_t read_common(uint32_t address) {
  switch (address) {
//  case 0x1f801000: return 0x1f000000;
//  case 0x1f801004: return 0x1f802000;
//  case 0x1f801008: return 0x0013243f;
//  case 0x1f80100c: return 0x00003022;
//  case 0x1f801010: return 0x0013243f;
    case 0x1f801014: return 0x200931e1;
//  case 0x1f801018: return;
//  case 0x1f80101c: return 0x00070777;
//  case 0x1f801020: return;

//  case 0x1f801060: return 0x00000b88;
  }

  if (address == 0xfffe0130) {
    return 0;
  }

  printf("[console] read(0x%08x)\n", address);
  throw std::exception();
}


uint32_t console_t::read_byte(uint32_t address) {
  auto component = decode(address);

  return component
    ? component->io_read_byte(address)
    : read_common(address)
    ;
}


uint32_t console_t::read_half(uint32_t address) {
  auto component = decode(address);

  return component
    ? component->io_read_half(address)
    : read_common(address)
    ;
}


uint32_t console_t::read_word(uint32_t address) {
  auto component = decode(address);

  return component
    ? component->io_read_word(address)
    : read_common(address)
    ;
}


static void write_common(uint32_t address, uint32_t data) {
  switch (address) {
    case 0x1f801000:
    case 0x1f801004:
    case 0x1f801008:
    case 0x1f80100c:
    case 0x1f801010:
    case 0x1f801014:
    case 0x1f801018:
    case 0x1f80101c:
    case 0x1f801020:
    case 0x1f801060:
      printf("[console] write_common(0x%08x, 0x%08x)\n", address, data);
      return;
  }

  if (address == 0x1f802041) {
    printf("[console] post(0x%08x)\n", data);
    return;
  }

  if (address == 0xfffe0130) {
    // system.write(2, 0xfffe0130, 0x00000804)
    // system.write(2, 0xfffe0130, 0x00000800)
    // system.write(2, 0xfffe0130, 0x0001e988)

    //     17 :: nostr  - No Streaming
    //     16 :: ldsch  - Enable Load Scheduling
    //     15 :: bgnt   - Enable Bus Grant
    //     14 :: nopad  - No Wait State
    //     13 :: rdpri  - Enable Read Priority
    //     12 :: intp   - Interrupt Polarity
    //     11 :: is1    - Enable I-Cache Set 1
    //     10 :: is0    - Enable I-Cache Set 0
    //  9,  8 :: iblksz - I-Cache Refill Size
    //      7 :: ds     - Enable D-Cache
    //  5,  4 :: dblksz - D-Cache Refill Size
    //      3 :: ram    - Scratchpad RAM
    //      2 :: tag    - Tag Test Mode
    //      1 :: inv    - Invalidate Mode
    //      0 :: lock   - Lock Mode

    return;
  }

  printf("[console] write(0x%08x, 0x%08x)\n", address, data);
  throw std::exception();
}


void console_t::write_byte(uint32_t address, uint32_t data) {
  auto component = decode(address);

  return component
    ? component->io_write_byte(address, data)
    : write_common(address, data)
    ;
}


void console_t::write_half(uint32_t address, uint32_t data) {
  auto component = decode(address);

  return component
    ? component->io_write_half(address, data)
    : write_common(address, data)
    ;
}


void console_t::write_word(uint32_t address, uint32_t data) {
  auto component = decode(address);

  return component
    ? component->io_write_word(address, data)
    : write_common(address, data)
    ;
}


void console_t::run_for_one_frame(uint16_t **vram, int *w, int *h) {
  const int ITERATIONS = 2;

  const int CPU_FREQ = 33868800;
  const int CYCLES_PER_FRAME = CPU_FREQ / 60 / ITERATIONS;

  for (int i = 0; i < CYCLES_PER_FRAME; i++) {
    cpu->tick();

    for (int j = 0; j < ITERATIONS; j++) {
      counter->tick();
      cdrom->tick();
      input->tick();
    }
  }

  send(interrupt_type_t::VBLANK);

  switch ((gpu->status >> 16) & 7) {
    case 0: *w = 256; break;
    case 1: *w = 368; break;
    case 2: *w = 320; break;
    case 3: *w = 368; break;
    case 4: *w = 512; break;
    case 5: *w = 368; break;
    case 6: *w = 640; break;
    case 7: *w = 368; break;
  }

  switch ((gpu->status >> 19) & 1) {
    case 0: *h = 240; break;
    case 1: *h = 480; break;
  }

  int x = (gpu->display_area_x);
  int y = (gpu->display_area_y);

  *vram = gpu->vram.get_pointer(x, y);
}

#ifndef __psxact_console__
#define __psxact_console__


#include <cstdint>
#include "addressable.hpp"
#include "interrupt_access.hpp"
#include "memory.hpp"
#include "memory_access.hpp"


enum class bus_width_t {
  byte,
  half,
  word
};

struct cdrom_t;

struct counter_t;

struct cpu_t;

struct dma_t;

struct exp1_t;

struct exp2_t;

struct exp3_t;

struct gpu_t;

struct input_t;

struct mdec_t;

struct spu_t;

struct console_t
  : public memory_access_t
  , public interrupt_access_t {

  memory_t<19> bios;
  memory_t<21> wram;
  memory_t<10> dmem;

  cdrom_t *cdrom;
  counter_t *counter;
  cpu_t *cpu;
  dma_t *dma;
  exp1_t *exp1;
  exp2_t *exp2;
  exp3_t *exp3;
  gpu_t *gpu;
  input_t *input;
  mdec_t *mdec;
  spu_t *spu;

  console_t(const char *bios_file_name, const char *game_file_name);

  void send(interrupt_type_t flag);

  uint32_t read_byte(uint32_t address);

  uint32_t read_half(uint32_t address);

  uint32_t read_word(uint32_t address);

  void write_byte(uint32_t address, uint32_t data);

  void write_half(uint32_t address, uint32_t data);

  void write_word(uint32_t address, uint32_t data);

  void run_for_one_frame(int *x, int *y, int *w, int *h);

private:

  addressable_t *decode(uint32_t address);

};


#endif // __psxact_console__

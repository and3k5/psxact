#include <cassert>
#include "gpu/gpu.hpp"
#include "console.hpp"
#include "utility.hpp"


gpu_t::gpu_t()
  : addressable_t("gpu") {
}


uint32_t gpu_t::data() {
  if (gpu_to_cpu_transfer.run.active) {
    uint16_t lower = vram_transfer();
    uint16_t upper = vram_transfer();

    return (upper << 16) | lower;
  }

  return data_latch;
}


uint32_t gpu_t::stat() {
  //  19    Vertical Resolution         (0=240, 1=480, when Bit22=1)  ;GP1(08h).2
  //  26    Ready to receive Cmd Word   (0=No, 1=Ready)  ;GP0(...) ;via GP0
  //  27    Ready to send VRAM to CPU   (0=No, 1=Ready)  ;GP0(C0h) ;via GPUREAD
  //  28    Ready to receive DMA Block  (0=No, 1=Ready)  ;GP0(...) ;via GP0

  return (status & ~0x00080000) | 0x1c002000;
}


uint32_t gpu_t::io_read_word(uint32_t address) {
  printf("[gpu] io_read_word(0x%08x)\n", address);

  switch (address) {
    case 0x1f801810: return data();
    case 0x1f801814: return stat();
  }

  return 0;
}


void gpu_t::io_write_word(uint32_t address, uint32_t data) {
  printf("[gpu] io_write_word(0x%08x, 0x%08x)\n", address, data);

  switch (address) {
    case 0x1f801810: return gp0(data);
    case 0x1f801814: return gp1(data);
  }
}


uint16_t *gpu_t::vram_data(int x, int y) {
  return (uint16_t *)vram.get_pointer(vram_address(x, y));
}


uint32_t gpu_t::vram_address(int x, int y) {
  return ((y * 1024) + x) * sizeof(uint16_t);
}


uint16_t gpu_t::vram_read(int x, int y) {
  return vram.io_read_half(vram_address(x, y));
}


void gpu_t::vram_write(int x, int y, uint16_t data) {
  vram.io_write_half(vram_address(x, y), data);
}


uint16_t gpu_t::vram_transfer() {
  auto &transfer = gpu_to_cpu_transfer;
  if (!transfer.run.active) {
    return 0;
  }

  uint16_t data = vram_read(
      transfer.reg.x + transfer.run.x,
      transfer.reg.y + transfer.run.y);

  transfer.run.x++;

  if (transfer.run.x == transfer.reg.w) {
    transfer.run.x = 0;
    transfer.run.y++;

    if (transfer.run.y == transfer.reg.h) {
      transfer.run.y = 0;
      transfer.run.active = false;
    }
  }

  return data;
}


void gpu_t::vram_transfer(uint16_t data) {
  auto &transfer = cpu_to_gpu_transfer;
  if (!transfer.run.active) {
    return;
  }

  vram_write(
      transfer.reg.x + transfer.run.x,
      transfer.reg.y + transfer.run.y, uint16_t(data));

  transfer.run.x++;

  if (transfer.run.x == transfer.reg.w) {
    transfer.run.x = 0;
    transfer.run.y++;

    if (transfer.run.y == transfer.reg.h) {
      transfer.run.y = 0;
      transfer.run.active = false;
    }
  }
}


// common functionality


gpu_t::color_t gpu_t::uint16_to_color(uint16_t value) {
  color_t color;
  color.r = (value << 3) & 0xf8;
  color.g = (value >> 2) & 0xf8;
  color.b = (value >> 7) & 0xf8;

  return color;
}


uint16_t gpu_t::color_to_uint16(gpu_t::color_t color) {
  return
    ((color.r >> 3) & 0x001f) |
    ((color.g << 2) & 0x03e0) |
    ((color.b << 7) & 0x7c00);
}


gpu_t::color_t gpu_t::get_texture_color__4bpp(gpu_t::tev_t &tev, gpu_t::point_t &coord) {
  uint16_t texel = vram_read(
    tev.texture_page_x + coord.x / 4,
    tev.texture_page_y + coord.y);

  texel = (texel >> ((coord.x & 3) * 4)) & 15;

  uint16_t pixel = vram_read(
    tev.palette_page_x + texel,
    tev.palette_page_y);

  return uint16_to_color(pixel);
}


gpu_t::color_t gpu_t::get_texture_color__8bpp(gpu_t::tev_t &tev, gpu_t::point_t &coord) {
  uint16_t texel = vram_read(
    tev.texture_page_x + coord.x / 2,
    tev.texture_page_y + coord.y);

  texel = (texel >> ((coord.x & 1) * 8)) & 255;

  uint16_t pixel = vram_read(
    tev.palette_page_x + texel,
    tev.palette_page_y);

  return uint16_to_color(pixel);
}


gpu_t::color_t gpu_t::get_texture_color_15bpp(gpu_t::tev_t &tev, gpu_t::point_t &coord) {
  uint16_t pixel = vram_read(
    tev.texture_page_x + coord.x,
    tev.texture_page_y + coord.y);

  return uint16_to_color(pixel);
}


gpu_t::color_t gpu_t::get_texture_color(gpu_t::tev_t &tev, gpu_t::point_t &coord) {
  switch (tev.texture_colors) {
    case 0: return get_texture_color__4bpp(tev, coord);
    case 1: return get_texture_color__8bpp(tev, coord);
    case 2: return get_texture_color_15bpp(tev, coord);
    case 3: return get_texture_color_15bpp(tev, coord);
  }

  return {0};
}

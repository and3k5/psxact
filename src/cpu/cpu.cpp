#include "cpu/cpu.hpp"
#include "cpu/cpu_cop0.hpp"
#include "cpu/cpu_cop2.hpp"
#include "utility.hpp"


cpu_t::opcode_t cpu_t::op_table[64] = {
  nullptr,         &cpu_t::op_bxx,   &cpu_t::op_j,    &cpu_t::op_jal,
  &cpu_t::op_beq,  &cpu_t::op_bne,   &cpu_t::op_blez, &cpu_t::op_bgtz,
  &cpu_t::op_addi, &cpu_t::op_addiu, &cpu_t::op_slti, &cpu_t::op_sltiu,
  &cpu_t::op_andi, &cpu_t::op_ori,   &cpu_t::op_xori, &cpu_t::op_lui,
  &cpu_t::op_cop0, &cpu_t::op_cop1,  &cpu_t::op_cop2, &cpu_t::op_cop3,
  &cpu_t::op_und,  &cpu_t::op_und,   &cpu_t::op_und,  &cpu_t::op_und,
  &cpu_t::op_und,  &cpu_t::op_und,   &cpu_t::op_und,  &cpu_t::op_und,
  &cpu_t::op_und,  &cpu_t::op_und,   &cpu_t::op_und,  &cpu_t::op_und,
  &cpu_t::op_lb,   &cpu_t::op_lh,    &cpu_t::op_lwl,  &cpu_t::op_lw,
  &cpu_t::op_lbu,  &cpu_t::op_lhu,   &cpu_t::op_lwr,  &cpu_t::op_und,
  &cpu_t::op_sb,   &cpu_t::op_sh,    &cpu_t::op_swl,  &cpu_t::op_sw,
  &cpu_t::op_und,  &cpu_t::op_und,   &cpu_t::op_swr,  &cpu_t::op_und,
  &cpu_t::op_lwc0, &cpu_t::op_lwc1,  &cpu_t::op_lwc2, &cpu_t::op_lwc3,
  &cpu_t::op_und,  &cpu_t::op_und,   &cpu_t::op_und,  &cpu_t::op_und,
  &cpu_t::op_swc0, &cpu_t::op_swc1,  &cpu_t::op_swc2, &cpu_t::op_swc3,
  &cpu_t::op_und,  &cpu_t::op_und,   &cpu_t::op_und,  &cpu_t::op_und
};


cpu_t::opcode_t cpu_t::op_table_special[64] = {
  &cpu_t::op_sll,     &cpu_t::op_und,   &cpu_t::op_srl,  &cpu_t::op_sra,
  &cpu_t::op_sllv,    &cpu_t::op_und,   &cpu_t::op_srlv, &cpu_t::op_srav,
  &cpu_t::op_jr,      &cpu_t::op_jalr,  &cpu_t::op_und,  &cpu_t::op_und,
  &cpu_t::op_syscall, &cpu_t::op_break, &cpu_t::op_und,  &cpu_t::op_und,
  &cpu_t::op_mfhi,    &cpu_t::op_mthi,  &cpu_t::op_mflo, &cpu_t::op_mtlo,
  &cpu_t::op_und,     &cpu_t::op_und,   &cpu_t::op_und,  &cpu_t::op_und,
  &cpu_t::op_mult,    &cpu_t::op_multu, &cpu_t::op_div,  &cpu_t::op_divu,
  &cpu_t::op_und,     &cpu_t::op_und,   &cpu_t::op_und,  &cpu_t::op_und,
  &cpu_t::op_add,     &cpu_t::op_addu,  &cpu_t::op_sub,  &cpu_t::op_subu,
  &cpu_t::op_and,     &cpu_t::op_or,    &cpu_t::op_xor,  &cpu_t::op_nor,
  &cpu_t::op_und,     &cpu_t::op_und,   &cpu_t::op_slt,  &cpu_t::op_sltu,
  &cpu_t::op_und,     &cpu_t::op_und,   &cpu_t::op_und,  &cpu_t::op_und,
  &cpu_t::op_und,     &cpu_t::op_und,   &cpu_t::op_und,  &cpu_t::op_und,
  &cpu_t::op_und,     &cpu_t::op_und,   &cpu_t::op_und,  &cpu_t::op_und,
  &cpu_t::op_und,     &cpu_t::op_und,   &cpu_t::op_und,  &cpu_t::op_und,
  &cpu_t::op_und,     &cpu_t::op_und,   &cpu_t::op_und,  &cpu_t::op_und
};


cpu_t::cpu_t(memory_access_t *memory)
  : addressable_t("cpu")
  , memory(memory) {

  regs.gp[0] = 0;
  regs.pc = 0xbfc00000;
  regs.next_pc = regs.pc + 4;

  cop0 = new cpu_cop0_t();
  cop2 = new cpu_cop2_t();
}


void cpu_t::tick() {
  read_code();

  is_branch_delay_slot = is_branch;
  is_branch = false;

  is_load_delay_slot = is_load;
  is_load = false;

  bool iec = (cop0->read_gpr(12) & 1) != 0;
  bool irq = (cop0->read_gpr(12) & cop0->read_gpr(13) & 0xff00) != 0;

  if (iec && irq) {
    enter_exception(cop0_exception_code_t::interrupt);
  }
  else {
    uint32_t code = (this->code >> 26) & 63;
    if (code)
      (*this.*op_table[code])();
    else
      (*this.*op_table_special[this->code & 63])();
  }
}


static uint32_t segments[8] = {

  0x7fffffff, // kuseg ($0000_0000 - $7fff_ffff)
  0x7fffffff, //
  0x7fffffff, //
  0x7fffffff, //
  0x1fffffff, // kseg0 ($8000_0000 - $9fff_ffff)
  0x1fffffff, // kseg1 ($a000_0000 - $bfff_ffff)
  0xffffffff, // kseg2 ($c000_0000 - $ffff_ffff)
  0xffffffff  //

};


static inline uint32_t map_address(uint32_t address) {
  return address & segments[address >> 29];
}


void cpu_t::log_bios_calls() {
  switch (regs.this_pc) {
    case 0x00a0:
      printf("bios::a(0x%02x)\n", regs.gp[9]);
      break;

    case 0x00b0:
      printf("bios::b(0x%02x)\n", regs.gp[9]);
      break;

    case 0x00c0:
      printf("bios::c(0x%02x)\n", regs.gp[9]);
      break;
  }
}


void cpu_t::enter_exception(cop0_exception_code_t code) {
  uint32_t status = cop0->read_gpr(12);
  status = (status & ~0x3f) | ((status << 2) & 0x3f);

  uint32_t cause = cop0->read_gpr(13);
  cause = (cause & ~0x7f) | ((int(code) << 2) & 0x7f);

  uint32_t epc;

  if (is_branch_delay_slot) {
    epc = regs.this_pc - 4;
    cause |= 0x80000000;
  }
  else {
    epc = regs.this_pc;
    cause &= ~0x80000000;
  }

  cop0->write_gpr(12, status);
  cop0->write_gpr(13, cause);
  cop0->write_gpr(14, epc);

  regs.pc = (status & (1 << 22))
    ? 0xbfc00180
    : 0x80000080
    ;

  regs.next_pc = regs.pc + 4;
}


void cpu_t::read_code() {
  if (regs.pc & 3) {
    enter_exception(cop0_exception_code_t::address_error_load);
  }

  regs.this_pc = regs.pc;
  regs.pc = regs.next_pc;
  regs.next_pc += 4;

  // todo: read i-cache

  code = memory->read_word(map_address(regs.this_pc));
}


uint32_t cpu_t::read_data_byte(uint32_t address) {
  if (cop0->read_gpr(12) & (1 << 16)) {
    return 0; // isc=1
  }

  // todo: read d-cache?

  return memory->read_byte(map_address(address));
}


uint32_t cpu_t::read_data_half(uint32_t address) {
  if (cop0->read_gpr(12) & (1 << 16)) {
    return 0; // isc=1
  }

  // todo: read d-cache?

  return memory->read_half(map_address(address));
}


uint32_t cpu_t::read_data_word(uint32_t address) {
  if (cop0->read_gpr(12) & (1 << 16)) {
    return 0; // isc=1
  }

  // todo: read d-cache?

  return memory->read_word(map_address(address));
}


void cpu_t::write_data_byte(uint32_t address, uint32_t data) {
  if (cop0->read_gpr(12) & (1 << 16)) {
    return; // isc=1
  }

  // todo: write d-cache?

  return memory->write_byte(map_address(address), data);
}


void cpu_t::write_data_half(uint32_t address, uint32_t data) {
  if (cop0->read_gpr(12) & (1 << 16)) {
    return; // isc=1
  }

  // todo: write d-cache?

  return memory->write_half(map_address(address), data);
}


void cpu_t::write_data_word(uint32_t address, uint32_t data) {
  if (cop0->read_gpr(12) & (1 << 16)) {
    return; // isc=1
  }

  // todo: write d-cache?

  return memory->write_word(map_address(address), data);
}


void cpu_t::update_irq(uint32_t stat, uint32_t mask) {
  istat = stat;
  imask = mask;

  int flag = (istat & imask)
    ? cop0->read_gpr(13) |  (1 << 10)
    : cop0->read_gpr(13) & ~(1 << 10)
    ;

  cop0->write_gpr(13, flag);
}


uint32_t cpu_t::get_imask() {
  return imask;
}


void cpu_t::set_imask(uint32_t value) {
  update_irq(get_istat(), value);
}


uint32_t cpu_t::get_istat() {
  return istat;
}


void cpu_t::set_istat(uint32_t value) {
  update_irq(value, get_imask());
}


uint32_t cpu_t::io_read_word(uint32_t address) {
  printf("[cpu] io_read_word(0x%08x)\n", address);

  switch (address) {
    case 0x1f801070: return get_istat();
    case 0x1f801074: return get_imask();
  }

  return 0;
}


void cpu_t::io_write_word(uint32_t address, uint32_t data) {
  printf("[cpu] io_write_word(0x%08x, 0x%08x)\n", address, data);

  switch (address) {
    case 0x1f801070: return set_istat(data & get_istat());
    case 0x1f801074: return set_imask(data & 0x7ff);
  }
}

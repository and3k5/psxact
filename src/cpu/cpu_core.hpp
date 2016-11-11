#ifndef PSXACT_CPU_CORE_HPP
#define PSXACT_CPU_CORE_HPP

#include <cstdint>

namespace cpu {
  struct state_t {
    struct {
      uint32_t regs[16];
    } cop0;

    struct {
      union {
        uint32_t gp[32];
      };

      uint32_t lo;
      uint32_t hi;
      uint32_t pc;
      uint32_t this_pc;
      uint32_t next_pc;
    } regs;

    uint32_t code;

    bool is_branch;
    bool is_branch_delay_slot;

    bool is_load;
    bool is_load_delay_slot;
    uint32_t load_index;
    uint32_t load_value;

    uint32_t i_stat;
    uint32_t i_mask;
  };

  extern state_t state;

  void initialize();

  void disassemble();

  void run(int count);

  void enter_exception(uint32_t code);

  void leave_exception();

  void read_code();

  uint32_t read_data(int size, uint32_t address);

  void write_data(int size, uint32_t address, uint32_t data);

  uint32_t mmio_read(int size, uint32_t address);

  void mmio_write(int size, uint32_t address, uint32_t data);

  // --============--
  //   Instructions
  // --============--

  void op_add();
  void op_addi();
  void op_addiu();
  void op_addu();
  void op_and();
  void op_andi();
  void op_beq();
  void op_reg_imm();
  void op_bgtz();
  void op_blez();
  void op_bne();
  void op_break();
  void op_cop0();
  void op_cop1();
  void op_cop2();
  void op_cop3();
  void op_div();
  void op_divu();
  void op_j();
  void op_jal();
  void op_jalr();
  void op_jr();
  void op_lb();
  void op_lbu();
  void op_lh();
  void op_lhu();
  void op_lui();
  void op_lw();
  void op_lwc0();
  void op_lwc1();
  void op_lwc2();
  void op_lwc3();
  void op_lwl();
  void op_lwr();
  void op_mfhi();
  void op_mflo();
  void op_mthi();
  void op_mtlo();
  void op_mult();
  void op_multu();
  void op_nor();
  void op_or();
  void op_ori();
  void op_sb();
  void op_sh();
  void op_sll();
  void op_sllv();
  void op_slt();
  void op_slti();
  void op_sltiu();
  void op_sltu();
  void op_special();
  void op_sra();
  void op_srav();
  void op_srl();
  void op_srlv();
  void op_sub();
  void op_subu();
  void op_sw();
  void op_swc0();
  void op_swc1();
  void op_swc2();
  void op_swc3();
  void op_swl();
  void op_swr();
  void op_syscall();
  void op_xor();
  void op_xori();

  // undefined instruction
  void op_und();
}

#endif //PSXACT_CPU_CORE_HPP

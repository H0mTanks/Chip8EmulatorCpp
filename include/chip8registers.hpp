#ifndef CHIP_REGISTERS
#define CHIP_REGISTERS

#include "config.hpp"

struct chip8_registers {
  unsigned char V[CHIP8_REGISTER_AMOUNT]; //array of 8 bit registers
  unsigned short I; //16 bit register used to store memory addresses
  unsigned char delay_timer; //8 bit delay timer register
  unsigned char sound_timer; //8 bit sound timer register
  unsigned short program_counter; //16 bit program counter used to store address of next instruction
  unsigned char stack_pointer; //8 bit stack pointer to one above top of the stack
};


#endif

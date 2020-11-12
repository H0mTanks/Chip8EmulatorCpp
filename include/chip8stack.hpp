#ifndef CHIP8_STACK
#define CHIP8_STACK

#include "config.hpp"

// struct chip8;
struct chip8_stack {
  unsigned short _stack[CHIP8_STACK_SIZE]; // STACK OF 16, 16 BIT VALUES
  void push(struct chip8& chip8, unsigned short value); //we pass in ref to chip8 object because we need the register.stack_pointer
  unsigned short pop(struct chip8& chip8);
};

#endif

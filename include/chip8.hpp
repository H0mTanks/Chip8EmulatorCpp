#ifndef CHIP8
#define CHIP8

#include "config.hpp"
#include "chip8memory.hpp"
#include "chip8registers.hpp"
#include "chip8stack.hpp"
#include "chip8keyboard.hpp"
#include "chip8screen.hpp"
#include <memory.h>

struct chip8 {
  struct chip8_memory memory;
  struct chip8_stack stack;
  struct chip8_registers registers;
  struct chip8_keyboard keyboard;
  struct chip8_screen screen;
  chip8();
  void load_program(const char* buffer, size_t size);
  void execute(unsigned short opcode);
};


#endif

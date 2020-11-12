#ifndef CHIP8_MEMORY
#define CHIP8_MEMORY

#include "config.hpp"
struct chip8_memory {
  unsigned char _memory[CHIP8_MEMORY_SIZE]; //memory buffer
  void set(int index, unsigned char val);
  unsigned char get(int index);
  unsigned short get_short(int index);
};

#endif

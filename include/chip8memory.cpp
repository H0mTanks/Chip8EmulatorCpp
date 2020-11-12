#include "chip8memory.hpp"
#include <cassert>

static void check_memory_bounds(int index) {
  assert(index >= 0 && index < CHIP8_MEMORY_SIZE);
}

void chip8_memory::set(int index, unsigned char val) {
  check_memory_bounds(index);
  _memory[index] = val;
}

unsigned char chip8_memory::get(int index) {
  check_memory_bounds(index);
  return _memory[index];
}

unsigned short chip8_memory::get_short(int index) {
  unsigned char byte1 = get(index);
  unsigned char byte2 = get(index + 1);
  unsigned short word = (byte1 << 8) | byte2; //returns a 2 byte word
  return word;
}

#include <cassert>
#include "chip8stack.hpp"
#include "chip8.hpp"

static void check_stack_bounds(struct chip8& chip8) {
  assert(chip8.registers.stack_pointer < CHIP8_STACK_SIZE);
}

void chip8_stack::push(struct chip8& chip8, unsigned short value) {
  chip8.registers.stack_pointer++;
  check_stack_bounds(chip8);
  _stack[chip8.registers.stack_pointer] = value; //points one above the top element
}

unsigned short chip8_stack::pop(struct chip8& chip8) {
  check_stack_bounds(chip8);
  unsigned short value =  _stack[chip8.registers.stack_pointer];
  chip8.registers.stack_pointer--;
  return value;
}

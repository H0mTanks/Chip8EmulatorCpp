#include <cassert>
#include "chip8keyboard.hpp"

static void check_keyboard_bounds(int key) {
    assert(key >= 0 && key < CHIP8_TOTAL_KEYS);
}

void chip8_keyboard::keyboard_set_map(const unsigned char map[]) {
  key_map = map;
  return;
}


int chip8_keyboard::keyboard_map(unsigned char key) {
  for (int i = 0; i < CHIP8_TOTAL_KEYS; i++) {
    if (key_map[i] == key) {
      return i;
    }
  }

  return -1;
}

void chip8_keyboard::pressed(int key) {
  keyboard[key] = true;
}

void chip8_keyboard::released(int key) {
  keyboard[key] = false;
}

bool chip8_keyboard::is_pressed(int key) {
  return keyboard[key];
}

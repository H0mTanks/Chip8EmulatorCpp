#ifndef CHIP8_KEYBOARD
#define CHIP8_KEYBOARD
#include <cstdbool>
#include "config.hpp"

struct chip8_keyboard {
  bool keyboard[CHIP8_TOTAL_KEYS];
  const unsigned char* key_map;
  void keyboard_set_map(const unsigned char map[]);
  int keyboard_map(unsigned char key);
  void pressed(int key);
  void released(int key);
  bool is_pressed(int key);
};

#endif

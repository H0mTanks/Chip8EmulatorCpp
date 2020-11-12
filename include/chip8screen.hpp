#ifndef SCREEN
#define SCREEN

#include "config.hpp"

struct chip8_screen {
  bool pixels[CHIP8_HEIGHT][CHIP8_WIDTH];
  void set_pixel(int x, int y);
  bool is_pixel_set(int x, int y);
  bool draw_sprite(int x, int y, const unsigned char* sprite, int num);
  void clear();
};





#endif

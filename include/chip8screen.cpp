#include "chip8screen.hpp"
#include <cassert>
#include <memory.h>

static void check_screen_bounds(int x, int y) {
  assert(x >= 0 && x < CHIP8_WIDTH && y >= 0 && y < CHIP8_HEIGHT);
}

void chip8_screen::set_pixel(int x, int y) {
  check_screen_bounds(x, y);
  pixels[y][x] = true;
}


bool chip8_screen::is_pixel_set(int x, int y) {
  check_screen_bounds(x, y);
  return pixels[y][x];
}

bool chip8_screen::draw_sprite(int x, int y, const unsigned char* sprite, int rows) {
  bool pixel_modified = false;

  for (int iy = 0; iy < rows; iy++) {
    char c = sprite[iy];

    for (int ix = 0; ix < 8; ix++) {
      if ((c & (0b10000000 >> ix)) == 0) {
        continue;
      }

      int dispx = (ix + x) % CHIP8_WIDTH;
      int dispy = (iy + y) % CHIP8_HEIGHT;

      if (pixels[dispy][dispx]) {
        pixel_modified = true;
      }

      pixels[dispy][dispx] ^= true;
    }
  }


  return pixel_modified;
}


void chip8_screen::clear() {
  memset(pixels, 0, sizeof(pixels));
  return;
}

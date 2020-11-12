#include <iostream>
#include <fstream>
#include <Windows.h>
#include "./include/SDL2/SDL.h"
#include "./include/chip8.hpp"
#include "./include/config.hpp"
#include "./include/chip8keyboard.hpp"

const unsigned char keyboard_map[CHIP8_TOTAL_KEYS] = {
  SDLK_0, SDLK_1, SDLK_2, SDLK_3, SDLK_4, SDLK_5,
  SDLK_6, SDLK_7, SDLK_8, SDLK_9, SDLK_a, SDLK_b,
  SDLK_c, SDLK_d, SDLK_e, SDLK_f
};

void close(SDL_Window* window) {
  SDL_DestroyWindow(window);
  SDL_Quit();
}


int main(int argc, char *argv[]) {

  if (argc < 2) {
    std::cout << "Provide a file to load!" << '\n';
    return -1;
  }

  const char* filename = argv[1];
  std::cout << "Loading: " << filename << '\n';

  FILE* file = fopen(filename, "rb");
  if (!file) {
    std::cout << "File failed to open :(" << '\n';
    return -1;
  }

  fseek(file, 0, SEEK_END);
  long filesize = ftell(file);
  fseek(file, 0, SEEK_SET);

  char buffer[filesize];
  int read = fread(buffer, filesize, 1, file);
  if (read != 1) {
    std::cout << "Failed to read from file" << '\n';
    return -1;
  }


  struct chip8 chip8;
  chip8.load_program(buffer, filesize);
  chip8.keyboard.keyboard_set_map(keyboard_map);

//SDL INITIALIZATION
  SDL_Init(SDL_INIT_EVERYTHING);
  SDL_Window* window = SDL_CreateWindow(
    CHIP8_WINDOW_TITLE,
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    CHIP8_WIDTH * CHIP8_WINDOW_SCALER, CHIP8_HEIGHT * CHIP8_WINDOW_SCALER, SDL_WINDOW_SHOWN
  );

  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_TEXTUREACCESS_TARGET);

//GAME LOOP
  while(true) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      switch(event.type) {
        case SDL_QUIT: {
          close(window);
          return EXIT_SUCCESS;
          break;
        }
        case SDL_KEYDOWN: {
          char key = event.key.keysym.sym;
          int vkey = chip8.keyboard.keyboard_map(key);
          if (vkey != -1) {
            chip8.keyboard.pressed(vkey);
          }
          break;
        }
        case SDL_KEYUP: {
          char key = event.key.keysym.sym;
          int vkey = chip8.keyboard.keyboard_map(key);
          if (vkey != -1) {
            chip8.keyboard.released(vkey);
          }
          break;
        }
      }
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);

    for(int x = 0; x < CHIP8_WIDTH; x++) {
      for (int y = 0; y < CHIP8_HEIGHT; y++) {
        if (chip8.screen.is_pixel_set(x, y)) {
          SDL_Rect r;
          r.x = x * CHIP8_WINDOW_SCALER;
          r.y = y * CHIP8_WINDOW_SCALER;
          r.w = CHIP8_WINDOW_SCALER;
          r.h = CHIP8_WINDOW_SCALER;
          SDL_RenderFillRect(renderer, &r);
        }
      }
    }

    SDL_RenderPresent(renderer);

    if (chip8.registers.delay_timer > 0) {
      Sleep(1);
      chip8.registers.delay_timer--;
    }

    if (chip8.registers.sound_timer > 0) {
      Beep(8000, 10 * chip8.registers.sound_timer);
      chip8.registers.sound_timer = 0;
    }

    unsigned short opcode = chip8.memory.get_short(chip8.registers.program_counter);
    chip8.registers.program_counter += 2;
    chip8.execute(opcode);
  }

  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}

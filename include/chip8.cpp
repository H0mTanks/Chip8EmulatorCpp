#include <cassert>
#include <memory.h>
#include <cstdlib>
#include <ctime>
#include "SDL2/SDL.h"
#include "chip8.hpp"

const unsigned char default_character_set[] = {
  0xf0, 0x90, 0x90, 0x90, 0xf0,
  0x20, 0x60, 0x20, 0x20, 0x70,
  0xf0, 0x10, 0xf0, 0x80, 0xf0,
  0xf0, 0x10, 0xf0, 0x10, 0xf0,
  0x90, 0x90, 0xf0, 0x10, 0x10,
  0xf0, 0x80, 0xf0, 0x10, 0xf0,
  0xf0, 0x80, 0xf0, 0x90, 0xf0,
  0xf0, 0x10, 0x20, 0x40, 0x40,
  0xf0, 0x90, 0xf0, 0x90, 0xf0,
  0xf0, 0x90, 0xf0, 0x10, 0xf0,
  0xf0, 0x90, 0xf0, 0x90, 0x90,
  0xe0, 0x90, 0xe0, 0x90, 0xe0,
  0xf0, 0x80, 0x80, 0x80, 0xf0,
  0xe0, 0x90, 0x90, 0x90, 0xe0,
  0xf0, 0x80, 0xf0, 0x80, 0xf0,
  0xf0, 0x80, 0xf0, 0x80, 0x80
};

chip8::chip8() {
  memset(this, 0, sizeof(struct chip8));
  memcpy(this->memory._memory, default_character_set, sizeof(default_character_set));
}

void chip8::load_program(const char* buffer, size_t size) {
  assert(size + CHIP8_LOAD_ADDRESS < CHIP8_MEMORY_SIZE);
  memcpy(&this->memory._memory[CHIP8_LOAD_ADDRESS], buffer, size);
  this->registers.program_counter = CHIP8_LOAD_ADDRESS;
}

static unsigned char wait_for_keypress(chip8& chip8) {
  SDL_Event event;
  while(SDL_WaitEvent(&event)) {
    if (event.type != SDL_KEYDOWN) {
      continue;
    }
    unsigned char c = event.key.keysym.sym;
    unsigned char chip8_key = chip8.keyboard.keyboard_map(c);
    if (chip8_key != -1) {
      return chip8_key;
    }
  }
  return -1;
}


static void execute_special_f(chip8& chip8, unsigned short opcode) {
  unsigned char x = (opcode >> 8) & 0x000f;
  switch(opcode & 0x00ff) {
    case 0x07: {
      chip8.registers.V[x] = chip8.registers.delay_timer;
      break;
    }
    case 0x0A: {
      unsigned char pressed_key = wait_for_keypress(chip8);
      chip8.registers.V[x] = pressed_key;
      break;
    }
    case 0x15: {
      chip8.registers.delay_timer = chip8.registers.V[x];
      break;
    }
    case 0x18: {
      chip8.registers.sound_timer = chip8.registers.V[x];
      break;
    }
    case 0x1E: {
      chip8.registers.I += chip8.registers.V[x];
      break;
    }
    case 0x29: {
      chip8.registers.I = chip8.registers.V[x] * CHIP8_DEFAULT_SPRITE_HEIGHT;
      break;
    }
    case 0x33: {
      unsigned char hundreds = chip8.registers.V[x] / 100;
      unsigned char tens = (chip8.registers.V[x] / 10) % 10;
      unsigned char ones = chip8.registers.V[x] % 10;
      chip8.memory.set(chip8.registers.I, hundreds);
      chip8.memory.set(chip8.registers.I + 1, tens);
      chip8.memory.set(chip8.registers.I + 2, ones);
      break;
    }
    case 0x55: {
      for (int i = 0; i <= x; i++) {
        chip8.memory.set(chip8.registers.I + i, chip8.registers.V[i]);
      }
      break;
    }
    case 0x65: {
      for (int i = 0; i <= x; i++) {
        chip8.registers.V[i] = chip8.memory.get(chip8.registers.I + i);
      }
      break;
    }
  }
}


static void execute_special_eight(chip8& chip8, unsigned short opcode) {
  unsigned char x = (opcode >> 8) & 0x000f;
  unsigned char y = (opcode >> 4) & 0x000f;
  unsigned char last_fourbits = opcode & 0x000f;

  switch(last_fourbits) {
    //LD Vx, Vy 8xy0
    case 0x00: {
      chip8.registers.V[x] = chip8.registers.V[y];
      break;
    }
    //OR Vx, Vy 8xy1
    case 0x01: {
      chip8.registers.V[x] = chip8.registers.V[x] | chip8.registers.V[y];
      break;
    }
    //AND Vx, Vy 8xy2
    case 0x02: {
      chip8.registers.V[x] = chip8.registers.V[x] & chip8.registers.V[y];
      break;
    }
    //XOR Vx, Vy 8xy3
    case 0x03: {
      chip8.registers.V[x] = chip8.registers.V[x] ^ chip8.registers.V[y];
      break;
    }
    //ADD Vx, Vy - 8xy4
    case 0x04: {
      chip8.registers.V[0x0f] = (chip8.registers.V[x] + chip8.registers.V[y]) > 0xff;
      chip8.registers.V[x] += chip8.registers.V[y];
      break;
    }
    //SUB Vx, Vy 8xy5
    case 0x05: {
      chip8.registers.V[0x0f] = chip8.registers.V[x] > chip8.registers.V[y];
      chip8.registers.V[x] -= chip8.registers.V[y];
      break;
    }
    //SHR Vx {, Vy} 8xy6
    case 0x06: {
      chip8.registers.V[0x0f] = chip8.registers.V[x] & 0b00000001;
      chip8.registers.V[x] /= 2;
      break;
    }
    //SUBN Vx {, Vy} 8xy7
    case 0x07: {
      chip8.registers.V[0x0f] = chip8.registers.V[y] > chip8.registers.V[x];
      chip8.registers.V[x] = chip8.registers.V[y] - chip8.registers.V[x];
      break;
    }
    // SHL Vx, {, Vy}
    case 0x0e: {
      chip8.registers.V[0x0f] = chip8.registers.V[x] & 0b10000000;
      chip8.registers.V[x] *= 2;
      break;
    }
  }
}

static void execute_special(chip8& chip8, unsigned short opcode) {
  unsigned short nnn = opcode & 0x0fff;
  unsigned char x = (opcode >> 8) & 0x000f;
  unsigned char y = (opcode >> 4) & 0x000f;
  unsigned short kk = opcode & 0x00ff;
  unsigned char n = opcode & 0x000f;

  switch(opcode & 0xf000) {
    //JP address
    case 0x1000: {
        chip8.registers.program_counter = nnn;
      break;
    }
    //CALL addr
    case 0x2000: {
      chip8.stack.push(chip8, chip8.registers.program_counter);
      chip8.registers.program_counter = nnn;
      break;
    }
    //SE Vx, byte 3xkk
    case 0x3000: {
      if (chip8.registers.V[x] == kk) {
        chip8.registers.program_counter += 2;
      }
      break;
    }
    //SNE Vx, byte 4xkk
    case 0x4000: {
      if (chip8.registers.V[x] != kk) {
        chip8.registers.program_counter += 2;
      }
      break;
    }
    //SE Vx, Vy 5xy0
    case 0x5000: {
      if (chip8.registers.V[x] == chip8.registers.V[y]) {
        chip8.registers.program_counter += 2;
      }
      break;
    }
    //LD Vx, byte 6xkk
    case 0x6000: {
      chip8.registers.V[x] = kk;
      break;
    }
    //ADD Vx, byte 7xkk
    case 0x7000: {
      chip8.registers.V[x] += kk;
      break;
    }
    case 0x8000: {
      execute_special_eight(chip8, opcode);
      break;
    }
    //SNE Vx, Vy
    case 9000: {
      if (chip8.registers.V[x] != chip8.registers.V[y]) {
        chip8.registers.program_counter += 2;
        break;
      }
    }
    //LD I, addr Annn
    case 0xA000: {
      chip8.registers.I = nnn;
      break;
    }
    //JP V0, addr Bnnn
    case 0xB000: {
      chip8.registers.program_counter = chip8.registers.V[0x00] + nnn;
      break;
    }
    //RND Vx, byte Cxkk
    case 0xC000: {
      srand(clock());
      chip8.registers.V[x] = (rand() % 255) & kk;
      break;
    }
    //DRW Vx, Vy, n Dxyn
    case 0xD000: {
      const unsigned char* sprite = (const unsigned char*) &chip8.memory._memory[chip8.registers.I];
      chip8.registers.V[0x0f] = chip8.screen.draw_sprite(chip8.registers.V[x], chip8.registers.V[y], sprite, n);
      break;
    }
    //Ex9E SKP Vx
    case 0xE000: {
      switch (opcode & 0x00ff) {
        case 0x9e: {
            if (chip8.keyboard.is_pressed(chip8.registers.V[x])) {
              chip8.registers.program_counter += 2;
            }
          break;
        }
        case 0xA1: {
          if (!chip8.keyboard.is_pressed(chip8.registers.V[x])) {
            chip8.registers.program_counter += 2;
          }
          break;
        }
      }
      break;
    }
    case 0xF000: {
      execute_special_f(chip8, opcode);
      break;
    }
  }
}

void chip8::execute(unsigned short opcode) {
  chip8& chip8 = *this;
  switch (opcode) {
    //CLS: clears the display
    case 0x00e0: {
      chip8.screen.clear();
      break;
    }
    //RET: return from subroutine
    case 0x00ee: {
      chip8.registers.program_counter = chip8.stack.pop(chip8);
      break;
    }
    //JP: jump to location nnn;
    default:
      execute_special(chip8, opcode);
      break;
  }
}

#ifndef CHIP8_H
#define CHIP8_H

#include <string>
#include <iostream>

class chip8 {
public:
    chip8(); // Constructor
    void Initialize();
    void LoadGame(char const* filename);
    void EmulateCycle();
    unsigned char* GetGFX();
    int GetGFX(int num);
    void UpdateTimers();
    unsigned char key[16]; // Hexadecimal keypad.
    unsigned char GetMemory(int address) { return memory[address]; }
    ~chip8(); // Destructor

private:
    unsigned short opcode; // 35 opcodes.
    unsigned char memory[4096]; // 4K memory in total.
    unsigned char V[16]; // 15 8-bit registers, V0, V1, all the way to VF. The 16th register is used for the 'carry flag'.
    unsigned short I; // Index register I.
    unsigned short pc; // Program counter (pc).
    unsigned char gfx[64 * 32]; // Graphics, 2048 pixels in total.
    unsigned char delay_timer; // Used for timing the events of the game, it's value can be set and read.
    unsigned char sound_timer; // Used for sound effects, it's value can only be set.
    unsigned short stack[16]; // Used to store return addresses when subroutines are called.
    unsigned short sp; // Stack pointer.
};

#endif
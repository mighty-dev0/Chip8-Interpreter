#include "chip8.h"
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <cstring>

chip8::chip8() {
}

void chip8::Initialize() {
    // Initializes registers and memory one time.
    pc = 0x200; // Program counter starts at 0x200.
    opcode = 0;
    I = 0;
    sp = 0;
    delay_timer = 0;
    sound_timer = 0;
    memset(memory, 0, sizeof(memory));
    memset(V, 0, sizeof(V));
    memset(gfx, 0, sizeof(gfx));
    memset(stack, 0, sizeof(stack));
    memset(key, 0, sizeof(key));
    draw_flag = true;

    // Load fontset from 0-80.
    unsigned char chip8_fontset[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
    for (int i = 0; i < 80; ++i)
        memory[i] = chip8_fontset[i];

    // Set random seed.
    srand(time(0));
}

void chip8::LoadGame(char const* filename) {
    // Open the file as a stream of binary.
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        std::cerr << "Failed to open ROM file.\n";
        exit(1);
    }

    // Get size of file
    std::streampos size = file.tellg();

    // Exit if rom is empty or invalid.
    if (size <= 0) {
        std::cerr << "ROM is empty or invalid.\n";
        file.close();
        exit(1);
    }

    // Check if rom is too large for memory.
    if (size > (4096 - 0x200)) {
        std::cerr << "ROM is too large to fit in memory.\n";
        file.close();
        exit(1);
    }

    // Allocate buffer to hold the contents.
    char* buffer = new char[static_cast<size_t>(size)];

    // Fill buffer with file.
    file.seekg(0, std::ios::beg);
    file.read(buffer, size);
    file.close();

    // Load Game into memory.
    for (long i = 0; i < size; ++i) {
        memory[0x200 + i] = buffer[i];
    }

    // Free the buffer.
    delete[] buffer;

    // Debug: Print first 10 bytes of loaded ROM
    // std::cout << "ROM loaded successfully. First 10 bytes:\n";
    // for (int i = 0x200; i < 0x20A; i++)
    //    printf("0x%04X: 0x%02X\n", i, memory[i]);
}


// One emulation cycle.
void chip8::EmulateCycle() {
    // Fetch opcode.
    opcode = memory[pc] << 8 | memory[pc + 1];

    // So we don't have to keep repeating the same code.
    pc += 2;

    // Decode opcode.
    switch (opcode & 0xF000) {
        case 0x0000:
            switch (opcode & 0x000F) {
                case 0x0000: // 00E0
                    memset(gfx, 0, sizeof(gfx));
                    draw_flag = true;
                break;
                
                case 0x000E: // 00EE
                    --sp;
                    pc = stack[sp];
                break;
                
                default:
                    printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
            }
        break;

        case 0x1000: // 1NNN
            pc = opcode & 0x0FFF;
        break;

        case 0x2000: // 2NNN
            stack[sp] = pc;
            ++sp;
            pc = opcode & 0x0FFF;
        break;

        case 0x3000: //3XNN
            if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
                pc += 2;
        break;

        case 0x4000: //4XNN
            if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
                pc += 2;
        break;

        case 0x5000: // 5XY0
            if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 8])
                pc += 2;
        break;

        case 0x6000: // 6XNN
            V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
        break;

        case 0x7000: // 7XNN
            V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
        break;

        case 0x8000: 
            switch (opcode & 0x000F) {
                case 0x0000: // 8XY0
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
                break;

                case 0x0001: // 8XY1
                    V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
                    V[0xF] = 0; // Reset VF
                break;

                case 0x0002: // 8XY2
                    V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
                    V[0xF] = 0; // Reset VF
                break;

                case 0x0003: //8XY3
                    V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
                    V[0xF] = 0; // Reset VF
                break;

                case 0x0004: // 8XY4
                    if (V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8]))
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;
                    V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
                break;

                case 0x0005: // 8XY5
                    if (V[(opcode & 0x0F00) >> 8] >= V[(opcode & 0x00F0) >> 4])
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;
                    V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
                break;

                case 0x0006: // 8XY6
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] >> 1;
                    V[0xF] = (V[((opcode & 0x0F00) >> 8)] & 1);
                break;

                case 0x0007: // 8XY7
                    if (V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8])
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
                break;

                case 0x000E: // 8XYE
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] << 1;
                    V[0xF] = (V[(opcode & 0x0F00) >> 8] >> 7);
                break;

                default:
                    printf("Unknown opcode [0x8000]: 0x%X\n", opcode);
            }
        break;

        case 0x9000: //9XY0
            if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
                pc += 2;
        break;

        case 0xA000: // ANNN
            I = opcode & 0x0FFF;
        break;

        case 0xB000: // BNNN
            pc = V[0] + (opcode & 0x0FFF);
        break;

        case 0xC000: // CXNN
            V[(opcode & 0x0F00) >> 8] = (rand() % (255 + 0)) & (opcode & 0x00FF);
        break;

        case 0xD000: {// DXYN
            uint8_t x = V[(opcode & 0x0F00) >> 8];
            uint8_t y = V[(opcode & 0x00F0) >> 4];
            uint8_t height = opcode & 0x000F;
            uint8_t pixel;
            V[0xF] = 0;

            for (int yline = 0; yline < height; yline++) {
                pixel = memory[I + yline];
                for (int xline = 0; xline < 8; xline++) {
                    if ((pixel & (0x80 >> xline))) {
                        // Wrap coordinates using modulo.
                        uint8_t x_pixel = (x + xline) % 64;
                        uint8_t y_pixel = (y + yline) % 32;

                        // Calculate position with row offset.
                        uint16_t pos = x_pixel + (y_pixel * 64);

                        // Check collision before modifying.
                        if (gfx[pos] == 1)
                            V[0xF] = 1;

                        // XOR the pixel.
                        gfx[pos] ^= 1;
                    }
                }
            }
        }
        draw_flag = true;
        break;

        case 0xE000:
            switch (opcode & 0x00FF) {
                case 0x009E: // EX9E
                    if (key[V[(opcode & 0x0F00) >> 8]] != 0)
                        pc += 2;
                break;

                case 0x00A1: // EXA1
                    if (key[V[(opcode & 0x0F00) >> 8]] == 0)
                        pc += 2;
                break;

                default:
                    printf("Unknown opcode [0xE000]: 0x%X\n", opcode);
            }
        break;

        case 0xF000:
            switch (opcode & 0x00FF) {
                case 0x0007: // FX07
                    V[(opcode & 0x0F00) >> 8] = delay_timer;
                break;

                case 0x000A: {// FX0A
                    bool key_pressed = false;
                    for (int i = 0; i < 16; ++i) {
                        if (key[i]) {
                            V[(opcode & 0x0F00) >> 8] = i;
                            key_pressed = true;
                            break;
                        }
                    }
                    if (!key_pressed)
                        pc -= 2; // If no key is pressed, do this instruction
                }
                break;

                case 0x0015: // FX15
                    delay_timer = V[(opcode & 0x0F00) >> 8];
                break;

                case 0x0018: // FX18
                    sound_timer = V[(opcode & 0x0F00) >> 8];
                break;

                case 0x001E: // FX1E
                    I += V[(opcode & 0x0F00) >> 8];
                break;

                case 0x0029: // FX29
                    I = 0x50 + (5 * V[(opcode & 0x0F00) >> 8]);
                break;

                case 0x0033: // FX33
                    memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
                    memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
                    memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
                break;

                case 0x0055: // FX55
                    for (unsigned char i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
                        memory[I + i] = V[i];
                    I += ((opcode & 0x0F00) >> 8) + 1;
                break;

                case 0x0065: // FX65
                    for (unsigned char i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
                        V[i] = memory[I + i];
                    I += ((opcode & 0x0F00) >> 8) + 1;
                break;

                default:
                    printf("Unknown opcode [0xF000]: 0x%X\n", opcode);
            }
        break;

        default:
            printf("Unknown opcode: 0x%X\n", opcode);
    }

    // For debugging.
    // printf("Executing opcode: 0x%04X at PC: 0x%04X\n", opcode, pc-2);
}

unsigned char* chip8::GetGFX() {
    return gfx;
}

int chip8::GetGFX(int num) {
    return gfx[num];
}

void chip8::UpdateTimers() {
    if (delay_timer > 0) 
        --delay_timer;
    if (sound_timer > 0) {
        if (sound_timer == 1) 
            printf("Beep!");  //TODO Implement proper sound.
        --sound_timer;
    }
}

bool chip8::GetDrawFlag() {
    return draw_flag;
}

// Destructor
chip8::~chip8() {}
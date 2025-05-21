#include <iostream>
#include <chrono>
#include <stdio.h>
#include "platform.h" // SDL for graphics and input.
#include "chip8.h" // My cpu core implementation.

chip8 my_chip8;

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <Scale> <ROM>\n";
        std::exit(EXIT_FAILURE);
    }

    // Set up render system and register input callbacks.
    int video_scale = std::stoi(argv[1]);
    char const* game_file_name = argv[2];
    Platform my_platform("CHIP-8 Interpreter", video_scale, video_scale, 64, 32);

    // Initialize Chip 8 system and load game into memory.
    my_chip8.Initialize();
    my_chip8.LoadGame(game_file_name);

    // Debug, print first 10 bytes of game
    // for (int i = 512; i < 522; ++i)
    //     printf("%X\n", my_chip8.GetMemory(i));

    // Emulation loop.
    const int CYCLES_PER_SECOND = 500; // Target 500Hz for CHIP-8
    const int TIMER_HZ = 60; // 60Hz for timers
    const float CYCLE_DELAY = 1000.0f / CYCLES_PER_SECOND;
    const float TIMER_DELAY = 1000.0f / TIMER_HZ;

    auto last_cycle_time = std::chrono::high_resolution_clock::now();
    auto last_timer_time = last_cycle_time;
    bool quit = false;

    while (!quit) {
        // Store key press state (Press and Release) and exit.
        quit = my_platform.ProcessInput(my_chip8.key);

        auto current_time = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(current_time - last_cycle_time).count();
        float timer_dt = std::chrono::duration<float, std::chrono::milliseconds::period>(current_time - last_timer_time).count();

        // Executes one cycle.
        if (dt > CYCLE_DELAY) {
            last_cycle_time = current_time;

            my_chip8.EmulateCycle();

            // Update the screen.
            if (my_chip8.GetDrawFlag()) {
                my_platform.Update(my_chip8.GetGFX());
                my_chip8.SetDrawFlag(false);
            }
        }

        // Update timers at 60Hz.
        if (timer_dt > TIMER_DELAY) {
            last_timer_time = current_time;
            my_chip8.UpdateTimers();
        }
    }

    return 0;
}
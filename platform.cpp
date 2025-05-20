#include "platform.h"

Platform::Platform(char const* title, int window_width, int window_height, int texture_width, int texture_height) {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow(title, 64, 32, 0);
    renderer = SDL_CreateRenderer(window, NULL);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, texture_width, texture_height);
    SDL_SetWindowSize(window, 64 * window_width, 32 * window_height);
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
}

void Platform::Update(void const* buffer) {
    // Convert pixels to 32-bit color.
    uint32_t pixels[64 * 32];
    const unsigned char* gfx = static_cast<const unsigned char*>(buffer);
    for (int i = 0; i < 64 * 32; ++i)
        pixels[i] = gfx[i] ? 0xFFFFFFFF : 0xFF000000; // 2 colors, white or black.

    SDL_UpdateTexture(texture, nullptr, pixels, 64 * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

bool Platform::ProcessInput(unsigned char* keys) {
    bool quit = false;
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                quit = true;
            break;

            case SDL_EVENT_KEY_DOWN:
                switch (event.key.key) {
                    case SDLK_ESCAPE:
                        quit = true;
                    break;

                    case SDLK_X:
                        keys[0] = 1;
                    break;

                    case SDLK_1:
                        keys[1] = 1;
                    break;

                    case SDLK_2:
                        keys[2] = 1;
                    break;

                    case SDLK_3:
                        keys[3] = 1;
                    break;

                    case SDLK_Q:
                        keys[4] = 1;
                    break;

                    case SDLK_W:
                        keys[5] = 1;
                    break;

                    case SDLK_E:
                        keys[6] = 1;
                    break;

                    case SDLK_A:
                        keys[7] = 1;
                    break;

                    case SDLK_S:
                        keys[8] = 1;
                    break;

                    case SDLK_D:
                        keys[9] = 1;
                    break;

                    case SDLK_Z:
                        keys[0xA] = 1;
                    break;

                    case SDLK_C:
                        keys[0xB] = 1;
                    break;

                    case SDLK_4:
                        keys[0xC] = 1;
                    break;

                    case SDLK_R:
                        keys[0xD] = 1;
                    break;

                    case SDLK_F:
                        keys[0xE] = 1;
                    break;

                    case SDLK_V:
                        keys[0xF] = 1;
                    break;

                    default:
                    break;
                }
            break;

            case SDL_EVENT_KEY_UP:
                switch (event.key.key) {
                    case SDLK_X:
                        keys[0] = 0;
                    break;

                    case SDLK_1:
                        keys[1] = 0;
                    break;

                    case SDLK_2:
                        keys[2] = 0;
                    break;

                    case SDLK_3:
                        keys[3] = 0;
                    break;

                    case SDLK_Q:
                        keys[4] = 0;
                    break;

                    case SDLK_W:
                        keys[5] = 0;
                    break;

                    case SDLK_E:
                        keys[6] = 0;
                    break;

                    case SDLK_A:
                        keys[7] = 0;
                    break;

                    case SDLK_S:
                        keys[8] = 0;
                    break;

                    case SDLK_D:
                        keys[9] = 0;
                    break;

                    case SDLK_Z:
                        keys[0xA] = 0;
                    break;

                    case SDLK_C:
                        keys[0xB] = 0;
                    break;

                    case SDLK_4:
                        keys[0xC] = 0;
                    break;

                    case SDLK_R:
                        keys[0xD] = 0;
                    break;

                    case SDLK_F:
                        keys[0xE] = 0;
                    break;

                    case SDLK_V:
                        keys[0xF] = 0;
                    break;

                    default:
                    break;
                }
            break;

            default:
            break;
        }
    }

    return quit;
}

Platform::~Platform() {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
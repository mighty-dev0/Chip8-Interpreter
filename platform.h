#ifndef platform
#define platform

#include <SDL3/SDL.h>

class Platform {
public:
    Platform(char const* title, int windo_width, int window_height, int texture_width, int texture_height);
    void Update(void const* buffer);
    bool ProcessInput(unsigned char* keys);
    ~Platform();

private:
    SDL_Window* window{};
    SDL_Renderer* renderer{};
    SDL_Texture* texture{};
};

#endif
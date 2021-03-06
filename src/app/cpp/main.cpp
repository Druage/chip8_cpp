#include <SDL2/SDL.h>

#include <chip8_emu.h>
#include <thread>
#include <iostream>

static SDL_Window *window{nullptr};
static SDL_Renderer *renderer{nullptr};
static SDL_Texture *sdlTexture{nullptr};

static std::array<uint32_t, 2048> pixels{0};

int WINDOW_WIDTH = 1024;                   // Window width
int WINDOW_HEIGHT = 512;                    // Window height

uint8_t keymap[16] = {
        SDLK_x,
        SDLK_1,
        SDLK_2,
        SDLK_3,
        SDLK_q,
        SDLK_w,
        SDLK_e,
        SDLK_a,
        SDLK_s,
        SDLK_d,
        SDLK_z,
        SDLK_c,
        SDLK_4,
        SDLK_r,
        SDLK_f,
        SDLK_v,
};

bool drawVideoFrameCb(const uint8_t *vfx, size_t size) {

    for (int i = 0; i < size; ++i) {
        auto pixel = vfx[i];
        if (pixel) {
            pixels[i] = (0x00FFFFFF * pixel) | 0xFF000000;
        } else {
            pixels[i] = 0x000000FF;
        }
    }

    SDL_UpdateTexture(sdlTexture, nullptr, pixels.data(), 64 * sizeof(Uint32));

    // Clear screen and render
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, sdlTexture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

void initSDL(const std::string &gameName) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "Couldn't initialize SDL: " << SDL_GetError() << std::endl;
        exit(3);
    }

    std::string windowTitle = "CHIP-8 | " + gameName;
    window = SDL_CreateWindow(windowTitle.data(),
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              WINDOW_WIDTH,
                              WINDOW_HEIGHT,
                              SDL_WINDOW_SHOWN);

    if (window == nullptr) {
        std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        exit(2);
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_RenderSetLogicalSize(renderer, WINDOW_WIDTH, WINDOW_HEIGHT);

    sdlTexture = SDL_CreateTexture(renderer,
                                   SDL_PIXELFORMAT_RGBA8888,
                                   SDL_TEXTUREACCESS_TARGET,
                                   64,
                                   32);

}

int main(int argc, char *argv[]) {

    std::string gameName = "PONG";
    initSDL(gameName);

    Chip8Emu chip8Emu;
    chip8Emu.load("roms/" + gameName);

    chip8Emu.render_video_frame_cb = drawVideoFrameCb;

    Uint64 NOW = SDL_GetPerformanceCounter();
    Uint64 LAST = 0;
    double deltaTimeMiliSeconds = 0;
    double msSum = 0;
    bool cycle = true;
    double framesPerMiliSecond = 1.2;

    SDL_Event event;
    while (((SDL_PollEvent(&event) || true) && event.type != SDL_QUIT)) {
        LAST = NOW;

        if (event.type == SDL_KEYDOWN) {
            for (int i = 0; i < 16; ++i) {
                if (event.key.keysym.sym == keymap[i]) {
                    chip8Emu.input_keys[i] = 1;
                }
            }
        } else if (event.type == SDL_KEYUP) {
            for (int i = 0; i < 16; ++i) {
                if (event.key.keysym.sym == keymap[i]) {
                    chip8Emu.input_keys[i] = 0;
                }
            }
        }

        if ( cycle ) {
            chip8Emu.run();
        }

        NOW = SDL_GetPerformanceCounter();
        deltaTimeMiliSeconds = (double) ((NOW - LAST) * 1000.0 / (double) SDL_GetPerformanceFrequency());

        if (msSum + deltaTimeMiliSeconds >= framesPerMiliSecond) {
            msSum = (0 + (msSum + deltaTimeMiliSeconds) - framesPerMiliSecond);
            cycle = true;
        } else {
            msSum += deltaTimeMiliSeconds;
            cycle = false;
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    return 0;
}

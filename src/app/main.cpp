#include <chip8_emu.h>
#include <iostream>


bool draw_video_to_console(const uint8_t* vfx, size_t size) {

    const char draw_char = '|';

    for(int y = 0; y < 32; ++y)
    {
        for(int x = 0; x < 64; ++x)
        {
            if( vfx[(y*64) + x] == 0)
                std::cout << ' ';
            else
                std::cout << draw_char;
        }
        std::cout << '\n';
    }

    return true;
}


int main() {
    Chip8Emu emu;

    emu.render_video_frame_cb = ::draw_video_to_console;
    emu.load("/home/lee/Downloads/INVADERS");

    while (true) {
        emu.run();
    }

    return 0;
}

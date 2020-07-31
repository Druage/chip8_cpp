#include <chip8_emu.h>

int main() {
    Chip8Emu emu;
    emu.load("/home/lee/Downloads/INVADERS");

    while (true) {
        emu.run();
    }

    return 0;
}

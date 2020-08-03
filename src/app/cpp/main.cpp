#include <chip8_emu.h>
#include <iostream>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "RenderSurface.h"


bool draw_video_to_console(const uint8_t *vfx, size_t size) {

    const char draw_char = '|';

    for (int y = 0; y < 32; ++y) {
        for (int x = 0; x < 64; ++x) {
            if (vfx[(y * 64) + x] == 0)
                std::cout << ' ';
            else
                std::cout << draw_char;
        }
        std::cout << '\n';
    }

    return true;
}


int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

    qmlRegisterType<RenderSurface>("Chip8", 1, 0, "RenderSurface");

    QQmlApplicationEngine engine;
    engine.load(QUrl("qrc:/main.qml"));

    return app.exec();


//    Chip8Emu emu;
//    emu.render_video_frame_cb = draw_video_to_console;
//    emu.loadGame("roms/INVADERS");
//
//    while (true) {
//        emu.run();
//    }

    return 0;
}

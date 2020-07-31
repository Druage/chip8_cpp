//
// Created by lee on 2020-07-31.
//

#include <iostream>
#include <QtGui/QRgb>
#include "chip8_manager.h"

Chip8Manager::Chip8Manager(QObject *parent)
        : QObject(parent),
          vfx_image(Chip8Emu::VFX_HEIGHT, Chip8Emu::VFX_WIDTH, QImage::Format_ARGB32) {

    emu.render_video_frame_cb = [this](const uint8_t *vfx, size_t size) {
        Q_CHECK_PTR(vfx);
        Q_ASSERT(size == Chip8Emu::VFX_HEIGHT * Chip8Emu::VFX_WIDTH);
        return draw_video_frame_cb(vfx, size);
    };

}

void Chip8Manager::play() {
    emu.run();
}

void Chip8Manager::load(const QString &game_path) {
    const auto std_game_path = game_path.toStdString();
    emu.load(std_game_path);
}

bool Chip8Manager::draw_video_frame_cb(const uint8_t *vfx, size_t size) {
    Q_UNUSED(size)

    for (size_t h = 0; h < Chip8Emu::VFX_HEIGHT; ++h) {
        for (size_t w = 0; w < Chip8Emu::VFX_WIDTH; ++w) {
            quint8 state = vfx_copy[(w + (h * Chip8Emu::VFX_WIDTH))];
            if (state == 0) {
                vfx_image.setPixel(w, h, qRgba(0, 0, 0, 0));
            } else {
                vfx_image.setPixel(w, h, qRgba(255, 255, 255, 255));
            }
        }
    }
    std::cout << "RUN DRAW FRAME" << std::endl;
    return true;
}

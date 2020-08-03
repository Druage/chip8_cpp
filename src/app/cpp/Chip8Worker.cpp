//
// Created by lee on 2020-08-03.
//

#include "Chip8Worker.h"
#include <QDebug>

Chip8Worker::Chip8Worker(QObject *parent) : QObject(parent),
                                            vfxVideoFrame(Chip8Emu::VFX_WIDTH,
                                                          Chip8Emu::VFX_HEIGHT,
                                                          QImage::Format_ARGB32) {

    vfxVideoFrame.fill(Qt::black);

    emu.render_video_frame_cb = [this](const uint8_t *vfx, size_t size) {
        return drawVideoFrameCb(vfx, size);
    };

    emu.update_input_key_state_cb = [this](size_t key) -> uint8_t {
        return inputKeyBuffer[key];
    };
}

bool Chip8Worker::drawVideoFrameCb(const uint8_t *vfx, size_t size) {
    Q_CHECK_PTR(vfx);
    Q_ASSERT(size == Chip8Emu::VFX_WIDTH * Chip8Emu::VFX_HEIGHT);
    QMutexLocker locker(&emuMutex);

    for (size_t h = 0; h < Chip8Emu::VFX_HEIGHT; ++h) {
        for (size_t w = 0; w < Chip8Emu::VFX_WIDTH; ++w) {
            quint8 state = vfx[(w + (h * Chip8Emu::VFX_WIDTH))];
            if (state == 0) {
                vfxVideoFrame.setPixel(w, h, qRgba(0, 0, 0, 0));
            } else {
                vfxVideoFrame.setPixel(w, h, qRgba(255, 255, 255, 255));
            }
        }
    }

    emit draw();
    return true;
}

void Chip8Worker::load(const QString &gamePath) {
    const auto std_game_path = gamePath.toStdString();
    emu.load(std_game_path);
}

void Chip8Worker::run() {
    emu.run();
}

void Chip8Worker::updateInput(int inputIndex, uint8_t state) {
    Q_ASSERT(inputIndex < inputKeyBuffer.size());
    Q_ASSERT(state == 0 || state == 1);

    QMutexLocker locker(&emuMutex);

    inputKeyBuffer[inputIndex] = state;
}

QSGNode *Chip8Worker::createTexture(const QQuickItem *renderSurface, QSGSimpleTextureNode *textureNode) {
    QMutexLocker locker(&emuMutex);

    QSGTexture *texture = renderSurface->window()->createTextureFromImage(vfxVideoFrame,
                                                                          QQuickWindow::TextureOwnsGLTexture);

    textureNode->setTexture(texture);
    textureNode->setRect(renderSurface->boundingRect());
    textureNode->setFiltering(QSGTexture::Nearest);
    textureNode->setOwnsTexture(true);

    return textureNode;
}

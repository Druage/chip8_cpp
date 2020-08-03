//
// Created by lee on 2020-08-03.
//

#include "Chip8Worker.h"

Chip8Worker::Chip8Worker(QObject *parent) : QObject(parent),
                                            vfxVideoFrame(Chip8Emu::VFX_WIDTH,
                                                          Chip8Emu::VFX_HEIGHT,
                                                          QImage::Format_ARGB32) {

    vfxVideoFrame.fill(Qt::black);

    emu.render_video_frame_cb = [this](const uint8_t *vfx, size_t size) {
        return drawVideoFrameCb(vfx, size);
    };

    emu.update_input_key_state_cb = [this](size_t index) -> uint8_t {
        return inputKeyBuffer[index];
    };
}

bool Chip8Worker::drawVideoFrameCb(const uint8_t *vfx, size_t size) {
    Q_CHECK_PTR(vfx);
    Q_ASSERT(size == Chip8Emu::VFX_WIDTH * Chip8Emu::VFX_HEIGHT);
    Q_ASSERT(!vfxVideoFrame.isNull());
    Q_ASSERT(vfxVideoFrame.width() == Chip8Emu::VFX_WIDTH && vfxVideoFrame.height() == Chip8Emu::VFX_HEIGHT);

    emuMutex.lock();

    for (size_t y = 0; y < vfxVideoFrame.height(); ++y) {
        for (size_t x = 0; x < vfxVideoFrame.width(); ++x) {
            auto state = vfx[(x + (y * vfxVideoFrame.width()))];

            Q_ASSERT(state == 0 || state == 1);
            if (!vfxVideoFrame.valid(x, y)) {
                Q_ASSERT(vfxVideoFrame.valid(x, y));
            }
            vfxVideoFrame.setPixel(x, y, state == 1 ? qRgba(0, 255, 255, 255) : qRgba(0, 0, 0, 0));
        }
    }

    emuMutex.unlock();

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
    Q_ASSERT(inputIndex < inputKeyBuffer.size() && inputIndex >= 0);
    Q_ASSERT(state == 0 || state == 1);

    QMutexLocker locker(&emuMutex);

    inputKeyBuffer[inputIndex] = state;
}

QSGNode *Chip8Worker::createTexture(const QQuickItem *renderSurface, QSGSimpleTextureNode *textureNode) {
    QMutexLocker locker(&emuMutex);

    QSGTexture *texture = renderSurface->window()->createTextureFromImage(vfxVideoFrame,
                                                                          QQuickWindow::TextureOwnsGLTexture);

    textureNode->setTexture(texture);
    textureNode->setFiltering(QSGTexture::Nearest);
    textureNode->setRect(renderSurface->boundingRect());
    textureNode->setOwnsTexture(true);

    return textureNode;
}

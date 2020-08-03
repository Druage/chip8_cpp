//
// Created by lee on 2020-07-31.
//

#include <QtGui/QRgb>
#include <QtQuick/QSGSimpleTextureNode>
#include <QtQuick/QQuickWindow>
#include <QtCore/QDebug>
#include <QtQuick/QSGSimpleRectNode>

#include "chip8_manager.h"

Chip8Manager::Chip8Manager(QQuickItem *parent)
        : QQuickItem(parent),
          vfx_image(Chip8Emu::VFX_WIDTH, Chip8Emu::VFX_HEIGHT, QImage::Format_ARGB32) {

    setFlag(QQuickItem::ItemHasContents, true);

    vfx_image.fill(Qt::black);

    emu_timer.setTimerType(Qt::PreciseTimer);
    emu_timer.setInterval(16 / 4);

    connect(&emu_timer, &QTimer::timeout, this, &Chip8Manager::play);
    connect(&emu_thread, &QThread::started, &emu_timer, static_cast<void (QTimer::*)(void)>(&QTimer::start));
    connect(&emu_thread, &QThread::finished, &emu_timer, &QTimer::stop);

    emu.render_video_frame_cb = [this](const uint8_t *vfx, size_t size) {
        Q_CHECK_PTR(vfx);
        Q_ASSERT(size == Chip8Emu::VFX_WIDTH * Chip8Emu::VFX_HEIGHT);
        return draw_video_frame_cb(vfx, size);
    };

    emu.update_input_key_state_cb = [this](size_t key) -> uint8_t {
        return input_key_buffer[key];
    };

    load("roms/PONG");

    emu_thread.start(QThread::HighestPriority);
}

Chip8Manager::~Chip8Manager() {
    emu_thread.quit();
    emu_thread.wait();
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

    render_mutex.lock();

    for (size_t h = 0; h < Chip8Emu::VFX_HEIGHT; ++h) {
        for (size_t w = 0; w < Chip8Emu::VFX_WIDTH; ++w) {
            quint8 state = vfx[(w + (h * Chip8Emu::VFX_WIDTH))];
            if (state == 0) {
                vfx_image.setPixel(w, h, qRgba(0, 0, 0, 0));
            } else {
                vfx_image.setPixel(w, h, qRgba(255, 255, 255, 255));
            }
        }
    }

    render_mutex.unlock();

    // Queue up next frame draw.
    update();
    return true;
}

QSGNode *Chip8Manager::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *) {

    QMutexLocker locker(&render_mutex);

    if (!window() || vfx_image.isNull()) {
        return node;
    }

    auto *texture_node = dynamic_cast<QSGSimpleTextureNode *>( node );
    if (!texture_node) {
        texture_node = new QSGSimpleTextureNode();
    }


    QSGTexture *texture = window()->createTextureFromImage(vfx_image, QQuickWindow::TextureOwnsGLTexture);

    texture_node->setTexture(texture);
    texture_node->setRect(boundingRect());
    texture_node->setFiltering(QSGTexture::Nearest);
    texture_node->setOwnsTexture(true);

    return texture_node;
}

void Chip8Manager::keyPressEvent(QKeyEvent *event) {
    keyEvent(event, 1);
}

void Chip8Manager::keyReleaseEvent(QKeyEvent *event) {
    keyEvent(event, 0);
}

void Chip8Manager::keyEvent(QKeyEvent *event, uint8_t state) {
    switch (event->key()) {
        case Qt::Key_0: {
            input_key_buffer[0] = state;
            break;
        }
        case Qt::Key_1: {
            input_key_buffer[1] = state;
            break;
        }
        case Qt::Key_2: {
            input_key_buffer[2] = state;
            break;
        }
        case Qt::Key_3: {
            input_key_buffer[3] = state;
            break;
        }
        case Qt::Key_4: {
            input_key_buffer[4] = state;
            break;
        }
        case Qt::Key_5: {
            input_key_buffer[5] = state;
            break;
        }
        case Qt::Key_6: {
            input_key_buffer[6] = state;
            break;
        }
        case Qt::Key_7: {
            input_key_buffer[7] = state;
            break;
        }
        case Qt::Key_8: {
            input_key_buffer[8] = state;
            break;
        }
        case Qt::Key_9: {
            input_key_buffer[9] = state;
            break;
        }
        case Qt::Key_A: {
            input_key_buffer[0xA] = state;
            break;
        }
        case Qt::Key_B: {
            input_key_buffer[0xB] = state;
            break;
        }
        case Qt::Key_C: {
            input_key_buffer[0xC] = state;
            break;
        }
        case Qt::Key_D: {
            input_key_buffer[0xD] = state;
            break;
        }
        case Qt::Key_E: {
            input_key_buffer[0xE] = state;
            break;
        }
        case Qt::Key_F: {
            input_key_buffer[0xF] = state;
            break;
        }
        default:
            event->ignore();
            break;
    }

    if (state) {
        QQuickItem::keyPressEvent(event);
    } else {
        QQuickItem::keyReleaseEvent(event);
    }

}

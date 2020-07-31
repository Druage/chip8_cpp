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

    // Set up timers and threads and all the gibberish.
    emu_timer.setTimerType(Qt::PreciseTimer);

    // Chip8 is really slow, so we shall speed up the gameplay.
    emu_timer.setInterval(16 / 2);

    connect(&emu_timer, &QTimer::timeout, this, &Chip8Manager::play);


    emu.render_video_frame_cb = [this](const uint8_t *vfx, size_t size) {
        Q_CHECK_PTR(vfx);
        Q_ASSERT(size == Chip8Emu::VFX_WIDTH * Chip8Emu::VFX_HEIGHT);
        return draw_video_frame_cb(vfx, size);
    };

    load("roms/INVADERS");

    emu_timer.start();

}

Chip8Manager::~Chip8Manager() {
    emu_timer.stop();
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
            quint8 state = vfx[(w + (h * Chip8Emu::VFX_WIDTH))];
            if (state == 0) {
                vfx_image.setPixel(w, h, qRgba(0, 0, 0, 0));
            } else {
                vfx_image.setPixel(w, h, qRgba(255, 255, 255, 255));
            }
        }
    }
    qDebug() << "RUN DRAW FRAME";

    // Queue up next frame draw.
    update();
    return true;
}

QSGNode *Chip8Manager::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *) {

//
//    auto *n = dynamic_cast<QSGSimpleRectNode *>(node);
//    if (!n) {
//        n = new QSGSimpleRectNode();
//        n->setColor(Qt::darkRed);
//    }
//    n->setRect(boundingRect());
//
//    return n;


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

    qDebug() << "UPDATE PAINT" << boundingRect();

    return texture_node;
}

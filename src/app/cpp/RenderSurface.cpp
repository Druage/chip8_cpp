//
// Created by lee on 2020-07-31.
//

#include <QtGui/QRgb>
#include <QtQuick/QSGSimpleTextureNode>
#include <QtQuick/QQuickWindow>
#include <QtCore/QDebug>
#include <QtQuick/QSGSimpleRectNode>

#include "RenderSurface.h"

RenderSurface::RenderSurface(QQuickItem *parent)
        : QQuickItem(parent) {

    setFlag(QQuickItem::ItemHasContents, true);

    chip8Timer.setTimerType(Qt::PreciseTimer);
    chip8Timer.setInterval(2);

    connect(&chip8Timer, &QTimer::timeout, this, &RenderSurface::play);
    connect(&chip8WorkerThread, &QThread::started, &chip8Timer, static_cast<void (QTimer::*)(void)>(&QTimer::start));
    connect(&chip8WorkerThread, &QThread::finished, &chip8Timer, &QTimer::stop);

    chip8Worker.moveToThread(&chip8WorkerThread);
    chip8Timer.moveToThread(&chip8WorkerThread);

    connect(this, &RenderSurface::run, &chip8Worker, &Chip8Worker::run);
    connect(this, &RenderSurface::load, &chip8Worker, &Chip8Worker::load);
    connect(this, &RenderSurface::updateInput, &chip8Worker, &Chip8Worker::updateInput);
    connect(&chip8Worker, &Chip8Worker::draw, this, &RenderSurface::update);

    loadGame("roms/PONG");

    chip8WorkerThread.start(QThread::HighestPriority);
}

RenderSurface::~RenderSurface() {
    chip8WorkerThread.quit();
    chip8WorkerThread.wait();
}

void RenderSurface::play() {
    emit run();
}

void RenderSurface::loadGame(const QString &gamePath) {
    emit load(gamePath);
}

QSGNode *RenderSurface::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *) {

    if (!window()) {
        return node;
    }

    auto *textureNode = dynamic_cast<QSGSimpleTextureNode *>( node );
    if (!textureNode) {
        textureNode = new QSGSimpleTextureNode();
    }

    return chip8Worker.createTexture(this, textureNode);
}

void RenderSurface::keyPressEvent(QKeyEvent *event) {
    keyEvent(event, true);
}

void RenderSurface::keyReleaseEvent(QKeyEvent *event) {
    keyEvent(event, false);
}

void RenderSurface::keyEvent(QKeyEvent *event, bool state) {

    int inputIndex = -1;

    switch (event->key()) {
        case Qt::Key_0: {
            inputIndex = 0;
            break;
        }
        case Qt::Key_1: {
            inputIndex = 1;
            break;
        }
        case Qt::Key_2: {
            inputIndex = 2;
            break;
        }
        case Qt::Key_3: {
            inputIndex = 3;
            break;
        }
        case Qt::Key_4: {
            inputIndex = 4;
            break;
        }
        case Qt::Key_5: {
            inputIndex = 5;
            break;
        }
        case Qt::Key_6: {
            inputIndex = 6;
            break;
        }
        case Qt::Key_7: {
            inputIndex = 7;
            break;
        }
        case Qt::Key_8: {
            inputIndex = 8;
            break;
        }
        case Qt::Key_9: {
            inputIndex = 9;
            break;
        }
        case Qt::Key_A: {
            inputIndex = 0xA;
            break;
        }
        case Qt::Key_B: {
            inputIndex = 0xB;
            break;
        }
        case Qt::Key_C: {
            inputIndex = 0xC;
            break;
        }
        case Qt::Key_D: {
            inputIndex = 0xD;
            break;
        }
        case Qt::Key_E: {
            inputIndex = 0xE;
            break;
        }
        case Qt::Key_F: {
            inputIndex = 0xF;
            break;
        }
        default:
            event->ignore();
            break;
    }

    if (inputIndex > -1) {
        emit updateInput(inputIndex, static_cast<uint8_t>(state));
    }

    if (state) {
        QQuickItem::keyPressEvent(event);
    } else {
        QQuickItem::keyReleaseEvent(event);
    }

}

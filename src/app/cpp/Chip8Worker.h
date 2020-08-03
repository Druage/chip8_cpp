//
// Created by lee on 2020-08-03.
//

#pragma once

#include <QtCore/QObject>
#include <chip8_emu.h>
#include <QtCore/QTimer>
#include <QtCore/QMutex>
#include <QImage>
#include <QtGui/QKeyEvent>
#include <QtQuick/QSGTexture>
#include <QtQuick/QQuickWindow>
#include <QtQuick/QQuickItem>

#include <QtQuick/QSGSimpleTextureNode>

class Chip8Worker : public QObject {
Q_OBJECT
public:
    explicit Chip8Worker(QObject *parent = nullptr);

    QSGNode *createTexture(const QQuickItem *renderSurface, QSGSimpleTextureNode *textureNode);

public slots:

    void load(const QString &gamePath);;

    void run();

    bool drawVideoFrameCb(const uint8_t *vfx, size_t size);

    // Called from render thread.
    void updateInput(int inputIndex, uint8_t state);

signals:

    void draw();

private:

    Chip8Emu emu;

    QMutex emuMutex;

    QImage vfxVideoFrame;
    Chip8Emu::InputKeyBuffer inputKeyBuffer{0};
};
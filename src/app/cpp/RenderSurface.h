//
// Created by lee on 2020-07-31.
//

#pragma once

#include <QtCore/QObject>
#include <chip8_emu.h>
#include <QtCore/QMutex>
#include <QtGui/QImage>
#include <QtQuick/QQuickItem>
#include <QtCore/QTimer>
#include <QtCore/QThread>
#include "Chip8Worker.h"

class RenderSurface : public QQuickItem {
Q_OBJECT
public:
    explicit RenderSurface(QQuickItem *parent = nullptr);

    ~RenderSurface() override;

    void loadGame(const QString &gamePath);

public:

    QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *paint_data) override;

    void keyPressEvent(QKeyEvent *event) override;

    void keyReleaseEvent(QKeyEvent *event) override;

    void keyEvent(QKeyEvent *event, bool state);

signals:

    void load(const QString &gamePath);

    void run();

    void updateInput(int inputIndex, bool state);

public slots:

    void play();

private:
    Chip8Worker chip8Worker;

    QTimer chip8Timer;
    QThread chip8WorkerThread;
};

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

class Chip8Manager : public QQuickItem {
    Q_OBJECT
public:
    explicit Chip8Manager(QQuickItem *parent = nullptr);
    ~Chip8Manager() override;

    void load(const QString &game_path);

    bool draw_video_frame_cb(const uint8_t *vfx, size_t size);

public:

    QSGNode *updatePaintNode( QSGNode *node, UpdatePaintNodeData *paint_data ) override;

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    void keyEvent(QKeyEvent *event, uint8_t state);

public slots:

    void play();

private:
    Chip8Emu emu;
    QMutex render_mutex;

    QImage vfx_image;
    Chip8Emu::InputKeyBuffer input_key_buffer;

    QTimer emu_timer;
    QThread emu_thread;
};

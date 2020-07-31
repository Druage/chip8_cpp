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

class Chip8Manager : public QQuickItem {
    Q_OBJECT
public:
    explicit Chip8Manager(QQuickItem *parent = nullptr);
    ~Chip8Manager();

    void load(const QString &game_path);

    bool draw_video_frame_cb(const uint8_t *vfx, size_t size);

    QSGNode *updatePaintNode( QSGNode *node, UpdatePaintNodeData *paint_data ) override;

public slots:

    void play();

private:
    Chip8Emu emu;
    QMutex render_mutex;

    QImage vfx_image;

    QTimer emu_timer;
};

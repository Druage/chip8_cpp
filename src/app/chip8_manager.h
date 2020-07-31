//
// Created by lee on 2020-07-31.
//

#pragma once

#include <QtCore/QObject>
#include <chip8_emu.h>
#include <QtCore/QMutex>
#include <QtGui/QImage>

class Chip8Manager : public QObject {
public:
    explicit Chip8Manager(QObject *parent = nullptr);

    void load(const QString &game_path);

    bool draw_video_frame_cb(const uint8_t *vfx, size_t size);

public slots:

    void play();

private:
    Chip8Emu emu;
    QMutex render_mutex;

    QVarLengthArray<uint8_t, Chip8Emu::VFX_HEIGHT * Chip8Emu::VFX_WIDTH> vfx_copy;
    QImage vfx_image;
};

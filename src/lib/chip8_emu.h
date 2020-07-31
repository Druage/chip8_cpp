//
// Created by lee on 2020-07-29.
//

#pragma once

#include <array>
#include <vector>
#include <functional>

using namespace std;

class Chip8Emu {
public:

    static const int VFX_SIZE = 64 * 32;

    Chip8Emu();

    void fetch_op_code();

    void load(const std::string &game_path);

    void run();

    enum KeyState : uint16_t {
        RELEASED = 0,
        PRESSED,
    };

public:
    std::function<bool(const uint8_t *vfx, size_t size)> render_video_frame_cb;
    std::function<bool(void)> play_audio_cb;

private:

    void update_timers();

    void skip_next_instruction();

    void inc_instruction();

    static int extract_x_bit(uint16_t op_code);

    static int extract_y_bit(uint16_t op_code);

public:
//    using input_device_cb =  uint8_t (*)(uint8_t hex_key_code);
    typedef uint8_t (*input_device_cb)(uint8_t hex_key_code);

public:
    std::array<uint8_t, 4096> memory{0x0};
    uint16_t pc{0x200};
    uint16_t i_register{0};

    std::array<uint16_t, 12> stack{0x0};
    uint8_t stack_pointer{0};

    std::array<uint8_t, 16> V{0x0};

    uint8_t delay_timer{60};
    uint8_t sound_timer{60};

    std::array<uint8_t, 16> input_keys{0x0};
    std::array<uint8_t, VFX_SIZE> vfx{0x0};

    std::vector<uint8_t> game_buffer;

public:
    bool game_loaded_flag{false};
};

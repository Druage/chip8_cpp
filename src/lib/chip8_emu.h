//
// Created by lee on 2020-07-29.
//

#pragma once

#include <array>
#include <vector>
#include <functional>

using namespace std;

// This is supposed to do nothing. It is intended for developers to understand what the public API is.
#define EMU_API /**/

// Meant to clarify that these methods and variables are only public as a workaround for testing the code
// and so should be treated as private.
#define EMU_PUBLIC_FOR_TESTING_PURPOSES public

class Chip8Emu {
public:
    EMU_API Chip8Emu();

    EMU_API void load(const std::string &game_path);

    EMU_API void run();

public:
    // - [NOT INHERENTLY THREAD SAFE, YOU MUST GUARD THIS FUNCTION CALLS]
    // - [POINTER IS ALWAYS VALID]
    EMU_API std::function<bool(const uint8_t *vfx, size_t size)> render_video_frame_cb;

    // [NOT INHERENTLY THREAD SAFE, YOU MUST GUARD THIS FUNCTION CALLS]
    EMU_API std::function<bool(void)> play_audio_cb;

    // - [NOT INHERENTLY THREAD SAFE, YOU MUST GUARD THIS FUNCTION CALLS]
    // ARG_1 - The index of the input buffer 0-15.
    // RETURN_TYPE - The state of the input key passed in as an arg. 0 or 1
    EMU_API std::function<uint8_t(size_t input_key)> update_input_key_state_cb;

private:

    void update_timers();

    void update_input_key_buffer();

    void skip_next_instruction();

    void inc_instruction();

    void load_fonts_into_memory();

    static int extract_x_bit(uint16_t op_code);

    static int extract_y_bit(uint16_t op_code);

    uint8_t &VX(uint16_t opcode);

    uint8_t &VY(uint16_t opcode);


EMU_PUBLIC_FOR_TESTING_PURPOSES:

    static const size_t VFX_WIDTH = 64;
    static const size_t VFX_HEIGHT = 32;

    static const size_t FONT_MEMORY_STARTING_LOCATION = 0x50;
    static const size_t FONT_MEMORY_SIZE = 0x0A0;


    using InputKeyBuffer = std::array<uint8_t, 16>;

    enum KeyState : uint16_t {
        RELEASED = 0,
        PRESSED,
    };

    unsigned char fonts[80] =
            {
                    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
                    0x20, 0x60, 0x20, 0x20, 0x70, //1
                    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
                    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
                    0x90, 0x90, 0xF0, 0x10, 0x10, //4
                    0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
                    0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
                    0xF0, 0x10, 0x20, 0x40, 0x40, //7
                    0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
                    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
                    0xF0, 0x90, 0xF0, 0x90, 0x90, //A
                    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
                    0xF0, 0x80, 0x80, 0x80, 0xF0, //C
                    0xE0, 0x90, 0x90, 0x90, 0xE0, //D
                    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
                    0xF0, 0x80, 0xF0, 0x80, 0x80  //F
            };

    void fetch_op_code();

    std::array<uint8_t, 4096> memory{0x0};
    uint16_t pc{0x200};
    uint16_t i_register{0};

    std::array<uint16_t, 16> stack{0x0};
    uint8_t stack_pointer{0};

    std::array<uint8_t, 16> V{0x0};

    uint8_t delay_timer{0};
    uint8_t sound_timer{0};

    InputKeyBuffer input_keys{0x0};
    std::array<uint8_t, VFX_WIDTH * VFX_HEIGHT> vfx{0x0};

    std::vector<uint8_t> game_buffer;

    bool game_loaded_flag{false};

};

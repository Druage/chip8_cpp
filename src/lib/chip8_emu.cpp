//
// Created by lee on 2020-07-29.
//

#include <iostream>
#include "chip8_emu.h"
#include <random>
#include <fstream>
#include <cassert>

Chip8Emu::Chip8Emu() {
    load_fonts_into_memory();
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"

void Chip8Emu::fetch_op_code() {
    auto op_code = memory[pc] << 8 | memory[pc + 1];

    switch (op_code & 0xF000) {

        case 0x0000: {

            switch (op_code & 0x00FF) {
                case 0x00E0: {
                    vfx.fill(0);
                    break;
                }
                case 0x00EE: {
                    stack_pointer--;
                    pc = stack[stack_pointer];
                    return;
                }
                default:
                    break;
            }

            inc_instruction();
            break;
        }

        case 0x1000: {
            pc = (op_code & 0x0FFF);
            break;
        }

        case 0x2000: {

            stack[stack_pointer] = pc;

            stack_pointer++;
            pc = (op_code & 0x0FFF);
            break;
        }

        case 0x3000: {
            const auto x_index = extract_x_bit(op_code);
            if (V[x_index] == (op_code & 0x00FF)) {
                skip_next_instruction();
            } else {
                inc_instruction();
            }
            break;
        }

        case 0x4000: {
            const auto x_index = extract_x_bit(op_code);
            if (V[x_index] != (op_code & 0x00FF)) {
                skip_next_instruction();
            } else {
                inc_instruction();
            }
            break;
        }

        case 0x5000: {
            const auto x_index = extract_x_bit(op_code);
            const auto y_index = extract_y_bit(op_code);

            if (V[x_index] == V[y_index]) {
                skip_next_instruction();
            } else {
                inc_instruction();
            }
            break;
        }

        case 0x6000: {
            const auto x_index = extract_x_bit(op_code);

            V[x_index] = (op_code & 0x00FF);

            inc_instruction();

            break;
        }

        case 0x7000: {
            const auto x_index = extract_x_bit(op_code);

            V[x_index] += (op_code & 0x00FF);

            inc_instruction();

            break;
        }

        case 0x8000: {
            const auto x_index = extract_x_bit(op_code);
            const auto y_index = extract_y_bit(op_code);

            switch (op_code & 0x000F) {
                case 0x0000: {
                    V[x_index] = V[y_index];
                    break;
                }

                case 0x0001: {
                    V[x_index] |= V[y_index];
                    break;
                }

                case 0x0002: {
                    V[x_index] &= V[y_index];
                    break;
                }

                case 0x0003: {
                    V[x_index] ^= V[y_index];
                    break;
                }

                case 0x0004: {

                    auto sum = V[x_index] + V[y_index];

                    if (sum > 0xFF) {
                        // Carry
                        V[0xF] = 1;
                    } else {
                        V[0xF] = 0;
                    }

                    V[x_index] = sum & 0xFF;

                    break;
                }

                case 0x0005: {

                    if (V[y_index] > V[x_index]) {
                        // Borrow
                        V[0xF] = 0;
                    } else {
                        V[0xF] = 1;
                    }

                    V[x_index] -= V[y_index];

                    break;
                }

                case 0x0006: {

                    V[0xF] = (V[x_index] & 0x1);

                    V[x_index] >>= 1;

                    break;
                }

                case 0x0007: {

                    if (V[y_index] > V[x_index]) {
                        // Borrow
                        V[0xF] = 1;
                    } else {
                        V[0xF] = 0;
                    }

                    V[x_index] = V[y_index] - V[x_index];

                    break;
                }

                case 0x000E: {

                    V[0xF] = (V[x_index] & 0x80) >> 7;
                    V[x_index] <<= 1;

                    break;
                }

                default:
                    break;
            }

            inc_instruction();

            break;
        }

        case 0x9000: {
            const auto x_index = extract_x_bit(op_code);
            const auto y_index = extract_y_bit(op_code);

            if (V[x_index] != V[y_index]) {
                skip_next_instruction();
            } else {
                inc_instruction();
            }

            break;
        }

        case 0xA000: {
            i_register = (op_code & 0x0FFF);
            inc_instruction();
            break;
        }
        case 0xB000: {
            pc = V[0] + (op_code & 0x0FFF);
            break;
        }

        case 0xC000: {
            const auto x_index = extract_x_bit(op_code);
            uint8_t xor_byte = (op_code & 0x00FF);

            std::random_device rd;
            std::mt19937 mt(rd());
            std::uniform_int_distribution<std::mt19937::result_type> dist(0, 255);

            V[x_index] = static_cast<uint8_t>(dist(rd) & xor_byte);

            inc_instruction();
            break;
        }

        case 0xD000: {
            const auto x_val = V[extract_x_bit(op_code)];
            const auto y_val = V[extract_y_bit(op_code)];

            uint16_t height = op_code & 0x000F;
            uint16_t pixel_row;

            V[0xF] = 0;

            auto sprite_width = 8;

            for (int y = 0; y < height; ++y) {

                pixel_row = memory[i_register + y];

                for (int r = 0; r < sprite_width; ++r) {
                    if ((pixel_row & (0x80 >> r)) != 0) {

                        // If pixel is on screen, we have a collision detected!
                        if (vfx[(x_val + r + ((y_val + y) * VFX_WIDTH))] == 1) {
                            V[0xF] = 1;
                        }

                        vfx[(x_val + r + ((y_val + y) * VFX_WIDTH))] ^= 1;
                    }
                }

            }

            if (render_video_frame_cb != nullptr) {
                render_video_frame_cb(vfx.data(), vfx.size());
            }

            inc_instruction();
            break;
        }

        case 0xE000: {
            switch (op_code & 0x00FF) {
                case 0x009E:
                    if (input_keys[V[extract_x_bit(op_code)]] == KeyState::PRESSED) {
                        skip_next_instruction();
                    } else {
                        inc_instruction();
                    }
                    break;
                case 0x00A1:
                    if (input_keys[V[extract_x_bit(op_code)]] == KeyState::RELEASED) {
                        skip_next_instruction();
                    } else {
                        inc_instruction();
                    }
                    break;
                default:
                    break;
            }
            break;
        }

        case 0xF000: {

            switch (op_code & 0x00FF) {
                case 0x0007: {
                    V[extract_x_bit(op_code)] = delay_timer;
                    inc_instruction();
                    break;
                }
                case 0x000A: {

                    // Stall waiting for a key press by not incrementing to current instruction.
                    for (int i = 0; i < input_keys.size(); ++i) {
                        if (input_keys[i] == KeyState::PRESSED) {
                            V[extract_x_bit(op_code)] = i;
                            inc_instruction();
                            break;
                        }
                    }

                    break;
                }
                case 0x0015: {
                    delay_timer = V[extract_x_bit(op_code)];
                    inc_instruction();
                    break;
                }

                case 0x0018: {
                    sound_timer = V[extract_x_bit(op_code)];
                    inc_instruction();
                    break;
                }

                case 0x001E: {
                    i_register += V[extract_x_bit(op_code)];
                    inc_instruction();
                    break;
                }

                // TODO - COME BACK TO THIS! IT MAY NOT BE COMPLETE!!!
                case 0x0029: {
                    i_register = V[extract_x_bit(op_code)] * 0x5;
                    inc_instruction();
                    break;
                }

                case 0x0033: {
                    memory[i_register + 0] = (V[extract_x_bit(op_code)] / 100);
                    memory[i_register + 1] = (V[extract_x_bit(op_code)] / 10) % 10;
                    memory[i_register + 2] = (V[extract_x_bit(op_code)] % 100) % 10;

                    inc_instruction();
                    break;
                }

                case 0x0055: {

                    // dump to memory

                    auto x_val = extract_x_bit(op_code);
                    for (int i = 0; i <= x_val; ++i) {
                        memory[i_register + i] = V[i];
                    }

                    inc_instruction();
                    break;
                }

                case 0x0065: {

                    // load from memory

                    auto x_val = extract_x_bit(op_code);
                    for (int i = 0; i <= x_val; ++i) {
                        V[i] = memory[i_register + i];
                    }

                    inc_instruction();
                    break;
                }

                default:
                    break;
            }

            break;
        }
        default:
            break;
    }

}

void Chip8Emu::skip_next_instruction() {
    pc += 4;
}

void Chip8Emu::inc_instruction() {
    pc += 2;
}

// The X-bit is one bit in the OP_CODE bit-mask of the form 0x?X??
int Chip8Emu::extract_x_bit(uint16_t op_code) {
    return (op_code & 0x0F00) >> 8;
}

// The Y-bit is one bit in the OP_CODE bit-mask of the form 0x??Y?
int Chip8Emu::extract_y_bit(uint16_t op_code) {
    return (op_code & 0x00F0) >> 4;
}

void Chip8Emu::load(const string &game_path) {

    std::ifstream game_file(game_path, std::ios::binary);

    if (!game_file.good()) { throw std::runtime_error(game_path + " could not be loaded"); }

    std::vector<char> buffer((std::istreambuf_iterator<char>(game_file)),
                             std::istreambuf_iterator<char>());

    for (int i = 0; i < buffer.size(); ++i) {
        memory[pc + i] = buffer[i];
    }

    game_loaded_flag = !buffer.empty();

}

void Chip8Emu::run() {
    if (!game_loaded_flag) { throw std::runtime_error("A game must be loaded before running"); }

    update_input_key_buffer();

    fetch_op_code();

    update_timers();
}

void Chip8Emu::update_input_key_buffer() {
    if (update_input_key_state_cb != nullptr) {
        for (size_t i = 0; i < input_keys.size(); ++i) {
            uint8_t key_state = update_input_key_state_cb(i);
            if (key_state == 0 || key_state == 1) {
                input_keys[i] = key_state;
            }
        }
    }
}

void Chip8Emu::update_timers() {

    if (delay_timer > 0) {
        --delay_timer;
    }

    if (sound_timer > 0) {
        if (sound_timer == 1) {
            if (play_audio_cb != nullptr) {
                play_audio_cb();
            }
        }
        --sound_timer;
    }
}

void Chip8Emu::load_fonts_into_memory() {

    for (int i = FONT_MEMORY_STARTING_LOCATION; i < FONT_MEMORY_SIZE; ++i) {
        memory[i] = fonts[i];
    }
}

#pragma clang diagnostic pop

//
// Created by lee on 2020-07-29.
//

#pragma once

#include "catch.hpp"

#include <chip8_emu.h>

auto is_zeroed_out = [](const auto &container) -> bool {
    bool all_values_are_zero = true;
    for (const auto c : container) {
        if (c != 0) {
            all_values_are_zero = false;
            break;
        }
    }
    return all_values_are_zero;
};

bool game_memory_locations_are_zeroed_out(const Chip8Emu &emu) {
    bool all_game_memory_locations_are_zeroed_out = true;
    for (size_t i = Chip8Emu::FONT_MEMORY_SIZE; i < emu.memory.size(); ++i) {
        if (emu.memory[i] != 0) {
            all_game_memory_locations_are_zeroed_out = false;
            break;
        }
    }
    return all_game_memory_locations_are_zeroed_out;
}

void is_no_op(const Chip8Emu &emu) {
    REQUIRE(emu.stack_pointer == 0);
    REQUIRE(emu.pc == 0x200);
    REQUIRE(emu.i_register == 0);

    REQUIRE(is_zeroed_out(emu.vfx));
    REQUIRE(is_zeroed_out(emu.input_keys));
    REQUIRE(is_zeroed_out(emu.stack));
}

bool instruction_has_not_changed(Chip8Emu &emu) {
    return emu.pc == 0x200;
}

bool instruction_was_incremented_normally(Chip8Emu &emu) {
    return emu.pc == 0x200 + 2;
}

bool next_instruction_was_skipped(Chip8Emu &emu) {
    return emu.pc == 0x200 + 4;
}

struct FakeGameFile {

    const char *file_name = "test_file";
    const char file_contents[9] = {'T', 'E', 'S', 'T', '_', 'R', 'O', 'M', '\0'};

    FakeGameFile() {
        std::ofstream test_file(file_name);
        test_file << file_contents << std::endl;
        test_file.close();
    }

    ~FakeGameFile() {
        REQUIRE(std::remove(file_name) == 0);
    }
};
//
// Created by lee on 2020-07-29.
//

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#include "include/catch.hpp"
#include "include/utils.h"
#include <chip8_emu.h>

TEST_CASE("All chip8 fields are initialized correctly") {

    Chip8Emu emu;

    REQUIRE(emu.memory.size() == 4096);
    REQUIRE(is_zeroed_out(emu.memory));
    REQUIRE(emu.pc == 0x200);
    REQUIRE(emu.i_register == 0);

    REQUIRE(emu.stack.size() == 12);
    REQUIRE(is_zeroed_out(emu.stack));
    REQUIRE(emu.stack_pointer == 0);

    REQUIRE(emu.V.size() == 16);
    REQUIRE(is_zeroed_out(emu.V));

    REQUIRE(emu.delay_timer == 0);
    REQUIRE(emu.sound_timer == 0);

    REQUIRE(emu.input_keys.size() == 16);
    REQUIRE(is_zeroed_out(emu.input_keys));

    REQUIRE(emu.vfx.size() == 64 * 32);
    REQUIRE(is_zeroed_out(emu.vfx));

    REQUIRE(emu.game_buffer.empty());
}

TEST_CASE("0x0NNN - NO_OP", "[OP_CODE]") {

    Chip8Emu emu;
    emu.memory[emu.pc + 0] = 0x01;
    emu.memory[emu.pc + 1] = 0x11;

    emu.fetch_op_code();

    REQUIRE(emu.i_register == 0);
    REQUIRE(instruction_was_incremented_normally(emu));
}

TEST_CASE("0x00E0 - Clears the screen", "[OP_CODE]") {

    Chip8Emu emu;

    emu.vfx.fill(1);

    emu.memory[emu.pc + 0] = 0x00;
    emu.memory[emu.pc + 1] = 0xE0;

    emu.fetch_op_code();

    REQUIRE(is_zeroed_out(emu.vfx));
    REQUIRE(instruction_was_incremented_normally(emu));
}

TEST_CASE("0x00EE - Returns from a subroutine.", "[OP_CODE]") {

    Chip8Emu emu;

    emu.memory[emu.pc + 0] = 0x00;
    emu.memory[emu.pc + 1] = 0xEE;
    emu.stack_pointer = 2;

    emu.stack[emu.stack_pointer - 1] = 0xFF;

    emu.fetch_op_code();

    REQUIRE(emu.i_register == 0);
    REQUIRE(emu.stack_pointer == 1);
    REQUIRE(emu.pc == 0xFF);
}

TEST_CASE("0x1NNN - Jumps to address NNN.", "[OP_CODE]") {

    Chip8Emu emu;

    emu.memory[emu.pc + 0] = 0x1A;
    emu.memory[emu.pc + 1] = 0xAA;

    emu.fetch_op_code();

    REQUIRE(emu.pc == 0x0AAA);
}

TEST_CASE("0x2NNN - Calls subroutine at NNN.", "[OP_CODE]") {

    Chip8Emu emu;

    emu.memory[emu.pc + 0] = 0x20;
    emu.memory[emu.pc + 1] = 0x01;

    auto original_pc_val = emu.pc;
    emu.fetch_op_code();

    REQUIRE(emu.stack_pointer == 1);
    REQUIRE(emu.stack[emu.stack_pointer - 1] == original_pc_val);
    REQUIRE(emu.pc == 0x0001);
}

TEST_CASE("0x3XNN - Skips the next instruction if VX equals NN.", "[OP_CODE]") {

    Chip8Emu emu;

    emu.memory[emu.pc + 0] = 0x31;
    emu.memory[emu.pc + 1] = 0xBB;

    emu.V[1] = 0xBB;

    emu.fetch_op_code();

    REQUIRE(next_instruction_was_skipped(emu));
}

TEST_CASE("0x3XNN - Does not skip the next instruction if VX does not equal NN.", "[OP_CODE]") {

    Chip8Emu emu;

    emu.memory[emu.pc + 0] = 0x31;
    emu.memory[emu.pc + 1] = 0xBB;

    emu.V[1] = 0xCC;

    emu.fetch_op_code();

    REQUIRE(emu.pc == 0x200 + 2);
}

TEST_CASE("0x4XNN - Skips the next instruction if VX doesn't equal NN.", "[OP_CODE]") {

    Chip8Emu emu;

    emu.memory[emu.pc + 0] = 0x41;
    emu.memory[emu.pc + 1] = 0xBB;

    emu.V[1] = 0xAA;

    emu.fetch_op_code();

    REQUIRE(next_instruction_was_skipped(emu));
}

TEST_CASE("0x4XNN - Does not skip the next instruction if VX equals NN.", "[OP_CODE]") {

    Chip8Emu emu;

    emu.memory[emu.pc + 0] = 0x41;
    emu.memory[emu.pc + 1] = 0xBB;

    emu.V[1] = 0xBB;

    emu.fetch_op_code();

    REQUIRE(instruction_was_incremented_normally(emu));
}

TEST_CASE("0x5XY0 - Skips the next instruction if VX equals VY.", "[OP_CODE]") {

    Chip8Emu emu;

    emu.memory[emu.pc + 0] = 0x51;
    emu.memory[emu.pc + 1] = 0x20;

    emu.V[1] = 0xAA;
    emu.V[2] = 0xAA;

    emu.fetch_op_code();

    REQUIRE(next_instruction_was_skipped(emu));
}


TEST_CASE("0x5XY0 - Does not skip the next instruction if VX does not equal VY.", "[OP_CODE]") {

    Chip8Emu emu;

    emu.memory[emu.pc + 0] = 0x51;
    emu.memory[emu.pc + 1] = 0x20;

    emu.V[1] = 0xAA;
    emu.V[2] = 0xBB;

    emu.fetch_op_code();

    REQUIRE(instruction_was_incremented_normally(emu));
}

TEST_CASE("0x6XNN - Sets VX to NN.", "[OP_CODE]") {

    Chip8Emu emu;

    emu.memory[emu.pc + 0] = 0x61;
    emu.memory[emu.pc + 1] = 0xFF;

    emu.fetch_op_code();

    REQUIRE(emu.V[1] == 0xFF);
    REQUIRE(instruction_was_incremented_normally(emu));
}

TEST_CASE("0x7XNN - Adds NN to VX.", "[OP_CODE]") {

    Chip8Emu emu;

    emu.memory[emu.pc + 0] = 0x71;
    emu.memory[emu.pc + 1] = 0x02;

    emu.V[1] = 0x02;

    emu.fetch_op_code();

    REQUIRE(emu.V[1] == 0x04);
    REQUIRE(instruction_was_incremented_normally(emu));
}

TEST_CASE("0x8XY0 - Sets VX to the value of VY.", "[OP_CODE]") {

    Chip8Emu emu;

    emu.memory[emu.pc + 0] = 0x81;
    emu.memory[emu.pc + 1] = 0x20;

    emu.V[1] = 0x02;
    emu.V[2] = 0x03;

    emu.fetch_op_code();

    REQUIRE(emu.V[1] == 0x03);
    REQUIRE(instruction_was_incremented_normally(emu));
}

TEST_CASE("0x8XY1 - Sets VX to VX or VY.", "[OP_CODE]") {

    Chip8Emu emu;

    emu.memory[emu.pc + 0] = 0x81;
    emu.memory[emu.pc + 1] = 0x21;

    emu.V[1] = 0x02;
    emu.V[2] = 0x04;

    emu.fetch_op_code();

    auto expected_result = 0x02u | 0x04u;
    REQUIRE(emu.V[1] == expected_result);
    REQUIRE(instruction_was_incremented_normally(emu));
}

TEST_CASE("0x8XY2 - Sets VX to VX and VY.", "[OP_CODE]") {

    Chip8Emu emu;

    emu.memory[emu.pc + 0] = 0x81;
    emu.memory[emu.pc + 1] = 0x22;

    emu.V[1] = 0x02;
    emu.V[2] = 0x04;

    emu.fetch_op_code();

    auto expected_result = 0x02u & 0x04u;
    REQUIRE(emu.V[1] == expected_result);
    REQUIRE(instruction_was_incremented_normally(emu));
}

TEST_CASE("0x8XY3 - Sets VX to VX xor VY.", "[OP_CODE]") {

    Chip8Emu emu;

    emu.memory[emu.pc + 0] = 0x81;
    emu.memory[emu.pc + 1] = 0x23;

    emu.V[1] = 0x02;
    emu.V[2] = 0x06;

    emu.fetch_op_code();

    auto expected_result = 0x02u ^0x06u;
    REQUIRE(emu.V[1] == expected_result);
    REQUIRE(instruction_was_incremented_normally(emu));
}

TEST_CASE("0x8XY4 - Adds VY to VX. VF is set to 0 because there is not a carry.", "[OP_CODE]") {

    Chip8Emu emu;

    emu.memory[emu.pc + 0] = 0x81;
    emu.memory[emu.pc + 1] = 0x24;

    emu.V[1] = 0x02;
    emu.V[2] = 0x06;

    emu.fetch_op_code();

    REQUIRE(emu.V[1] == 0x08u);
    REQUIRE(emu.V[0xF] == 0u);
    REQUIRE(instruction_was_incremented_normally(emu));
}

TEST_CASE("0x8XY4 - Adds VY to VX. VF is set to 1 because there is a carry", "[OP_CODE]") {

    Chip8Emu emu;

    emu.memory[emu.pc + 0] = 0x81;
    emu.memory[emu.pc + 1] = 0x24;

    emu.V[1] = 0x02;
    emu.V[2] = 0xFF;

    emu.fetch_op_code();

    REQUIRE(emu.V[1] == 1);
    REQUIRE(emu.V[0xF] == 1u);
    REQUIRE(instruction_was_incremented_normally(emu));
}

TEST_CASE("0x8XY5 - VY is subtracted from VX. VF is set to 0 when there's a borrow", "[OP_CODE]") {

    Chip8Emu emu;

    emu.memory[emu.pc + 0] = 0x81;
    emu.memory[emu.pc + 1] = 0x25;

    emu.V[1] = 0x04;
    emu.V[2] = 0x05;

    emu.fetch_op_code();

    REQUIRE(emu.V[1] == 0xFF);
    REQUIRE(emu.V[0xF] == 0u);
    REQUIRE(instruction_was_incremented_normally(emu));
}

TEST_CASE("0x8XY5 - VY is subtracted from VX. VF is set to 1 when there's not a borrow", "[OP_CODE]") {

    Chip8Emu emu;

    emu.memory[emu.pc + 0] = 0x81;
    emu.memory[emu.pc + 1] = 0x25;

    emu.V[1] = 0x04;
    emu.V[2] = 0x03;

    emu.fetch_op_code();

    REQUIRE(emu.V[1] == 1u);
    REQUIRE(emu.V[0xF] == 1u);
    REQUIRE(instruction_was_incremented_normally(emu));
}

TEST_CASE("0x8XY6 - Stores the least significant bit of VX in VF and then shifts VX to the right by 1.", "[OP_CODE]") {

    Chip8Emu emu;

    emu.memory[emu.pc + 0] = 0x81;
    emu.memory[emu.pc + 1] = 0x26;

    emu.V[1] = 0x40;
    emu.V[2] = 0x03;

    emu.fetch_op_code();

    REQUIRE(emu.V[0xF] == 0x0);
    REQUIRE(emu.V[1] == 0x20);
    REQUIRE(instruction_was_incremented_normally(emu));
}

TEST_CASE("0x8XY6 - Double checks storing the least significant bit of VX in VF", "[OP_CODE]") {

    Chip8Emu emu;

    emu.memory[emu.pc + 0] = 0x81;
    emu.memory[emu.pc + 1] = 0x26;

    emu.V[1] = 0x03;
    emu.V[2] = 0x03;

    emu.fetch_op_code();

    REQUIRE(emu.V[0xF] == 0x1);
}

TEST_CASE("0x8XY7 - Sets VX to VY minus VX. VF is set to 0 because there is a borrow",
          "[OP_CODE]") {

    Chip8Emu emu;

    emu.memory[emu.pc + 0] = 0x81;
    emu.memory[emu.pc + 1] = 0x27;

    emu.V[1] = 0x03;
    emu.V[2] = 0x04;

    emu.fetch_op_code();

    REQUIRE(emu.V[1] == 0x1);
    REQUIRE(emu.V[0xF] == 0x0);
    REQUIRE(instruction_was_incremented_normally(emu));
}

TEST_CASE("0x8XY7 - Sets VX to VY minus VX. VF is set to 1 because there is not a borrow",
          "[OP_CODE]") {

    Chip8Emu emu;

    emu.memory[emu.pc + 0] = 0x81;
    emu.memory[emu.pc + 1] = 0x27;

    emu.V[1] = 0x03;
    emu.V[2] = 0x02;

    emu.fetch_op_code();

    REQUIRE(emu.V[1] == 0xFF);
    REQUIRE(emu.V[0xF] == 0x1);
    REQUIRE(instruction_was_incremented_normally(emu));
}

TEST_CASE("0x8XYE - Stores the most significant bit of VX in VF and then shifts VX to the left by 1.",
          "[OP_CODE]") {

    Chip8Emu emu;

    emu.memory[emu.pc + 0] = 0x81;
    emu.memory[emu.pc + 1] = 0x2E;

    emu.V[1] = 0x40;

    emu.fetch_op_code();

    REQUIRE(emu.V[1] == 0x80);
    REQUIRE(emu.V[0xF] == 0x0);
    REQUIRE(instruction_was_incremented_normally(emu));
}

TEST_CASE("0x9XY0 - Skips the next instruction because VX doesn't equal VY.", "[OP_CODE]") {

    Chip8Emu emu;

    emu.memory[emu.pc + 0] = 0x91;
    emu.memory[emu.pc + 1] = 0x20;

    emu.V[1] = 0x40;
    emu.V[2] = 0x50;

    emu.fetch_op_code();

    REQUIRE(next_instruction_was_skipped(emu));
}

TEST_CASE("0x9XY0 - Does not skip the next instruction because VX equals VY.", "[OP_CODE]") {

    Chip8Emu emu;

    emu.memory[emu.pc + 0] = 0x91;
    emu.memory[emu.pc + 1] = 0x20;

    emu.V[1] = 0x40;
    emu.V[2] = 0x40;

    emu.fetch_op_code();

    REQUIRE(instruction_was_incremented_normally(emu));
}

TEST_CASE("0xANNN - Sets I register to the address NNN.", "[OP_CODE]") {

    Chip8Emu emu;
    emu.memory[emu.pc + 0] = 0xAB;
    emu.memory[emu.pc + 1] = 0xCD;

    emu.fetch_op_code();

    REQUIRE(emu.i_register == 0X0BCD);
    REQUIRE(instruction_was_incremented_normally(emu));
}

TEST_CASE("0xBNNN - Jumps to the address NNN plus V0.", "[OP_CODE]") {

    Chip8Emu emu;
    emu.memory[emu.pc + 0] = 0xBA;
    emu.memory[emu.pc + 1] = 0xAD;

    emu.V[0] = 0x10;

    emu.fetch_op_code();

    REQUIRE(emu.pc == 0x10 + 0x0AAD);
}

TEST_CASE("0xCXNN - Sets VX to the result of a bitwise and operation on a random number [0-255]", "[OP_CODE]") {

    Chip8Emu emu;

    emu.memory[emu.pc + 0] = 0xC1;
    emu.memory[emu.pc + 1] = 0x00;

    emu.V[1] = 0x40;

    emu.fetch_op_code();


    REQUIRE(instruction_was_incremented_normally(emu));
}

// TODO - Backfill this test
TEST_CASE("0xDXYN - Draws a sprite at coordinate (VX, VY). Sprite is 8x8 pixels", "[OP_CODE]") {

    Chip8Emu emu;

    bool video_cb_was_called = false;
    emu.render_video_frame_cb = [&](const uint8_t *vfx, std::size_t size) {
        video_cb_was_called = true;

        REQUIRE(vfx == emu.vfx.data());
        REQUIRE(size == emu.vfx.size());
        return true;
    };

    emu.memory[emu.pc + 0] = 0xD1;
    emu.memory[emu.pc + 1] = 0x22;

    emu.V[1] = 0x00;
    emu.V[2] = 0x00;

    emu.fetch_op_code();

    REQUIRE(instruction_was_incremented_normally(emu));
    REQUIRE(video_cb_was_called);
}

TEST_CASE("0xDXYN - Should not error out if no video callback was provided", "[OP_CODE]") {

    Chip8Emu emu;

    emu.memory[emu.pc + 0] = 0xD1;
    emu.memory[emu.pc + 1] = 0x22;

    emu.V[1] = 0x00;
    emu.V[2] = 0x00;

    REQUIRE_NOTHROW(emu.fetch_op_code());

    REQUIRE(instruction_was_incremented_normally(emu));
}

TEST_CASE("0xEX9E - Skips the next instruction because the key stored in VX is pressed.", "[OP_CODE]") {

    Chip8Emu emu;

    emu.memory[emu.pc + 0] = 0xE1;
    emu.memory[emu.pc + 1] = 0x9E;

    //Store that there was a key change in register V1 for key 0
    emu.V[1] = 0x00;

    // The key 0 was pressed!
    emu.input_keys[0] = Chip8Emu::KeyState::PRESSED;

    emu.fetch_op_code();

    REQUIRE(next_instruction_was_skipped(emu));
}

TEST_CASE("0xEX9E - Does not skip the next instruction because the key stored in VX was not pressed.", "[OP_CODE]") {

    Chip8Emu emu;

    emu.memory[emu.pc + 0] = 0xE1;
    emu.memory[emu.pc + 1] = 0x9E;

    emu.V[1] = 0x00;

    emu.input_keys[0] = Chip8Emu::KeyState::RELEASED;

    emu.fetch_op_code();

    REQUIRE(instruction_was_incremented_normally(emu));
}

TEST_CASE("0xEXA1 - Skips the next instruction because the key stored in VX isn't pressed.", "[OP_CODE]") {

    Chip8Emu emu;

    emu.memory[emu.pc + 0] = 0xE1;
    emu.memory[emu.pc + 1] = 0xA1;

    emu.V[1] = 0x00;

    emu.input_keys[0] = Chip8Emu::KeyState::RELEASED;

    emu.fetch_op_code();

    REQUIRE(next_instruction_was_skipped(emu));
}

TEST_CASE("0xEXA1 - Does not skip the next instruction because the key stored in VX is pressed.", "[OP_CODE]") {

    Chip8Emu emu;

    emu.memory[emu.pc + 0] = 0xE1;
    emu.memory[emu.pc + 1] = 0xA1;

    emu.V[1] = 0x00;

    emu.input_keys[0] = Chip8Emu::KeyState::PRESSED;

    emu.fetch_op_code();

    REQUIRE(instruction_was_incremented_normally(emu));
}

TEST_CASE("0xFX07 - Sets VX to the value of the delay timer.", "[OP_CODE]") {

    Chip8Emu emu;

    emu.memory[emu.pc + 0] = 0xF1;
    emu.memory[emu.pc + 1] = 0x07;

    emu.delay_timer = 0x05;

    emu.fetch_op_code();

    REQUIRE(emu.V[1] == 0x05u);
    REQUIRE(instruction_was_incremented_normally(emu));
}

TEST_CASE("0xFX0A - A key press is awaited, and then stored in VX.", "[OP_CODE]") {

    Chip8Emu emu;

    emu.memory[emu.pc + 0] = 0xF1;
    emu.memory[emu.pc + 1] = 0x0A;

    emu.V[1] = 0x00;

    emu.input_keys[1] = Chip8Emu::KeyState::RELEASED;
    emu.fetch_op_code();

    REQUIRE(emu.V[1] == 0x00);
    REQUIRE(instruction_has_not_changed(emu));

    emu.input_keys[1] = Chip8Emu::KeyState::PRESSED;
    emu.fetch_op_code();

    REQUIRE(emu.V[1] == 0x01);
    REQUIRE(instruction_was_incremented_normally(emu));
}

TEST_CASE("0xFX15 - Sets the delay timer to VX.", "[OP_CODE]") {

    Chip8Emu emu;

    emu.memory[emu.pc + 0] = 0xF1;
    emu.memory[emu.pc + 1] = 0x15;

    emu.V[1] = 0x07;

    emu.fetch_op_code();

    REQUIRE(emu.delay_timer == 0x07);
    REQUIRE(instruction_was_incremented_normally(emu));
}

TEST_CASE("0xFX18 - Sets the sound timer to VX.", "[OP_CODE]") {

    Chip8Emu emu;

    emu.memory[emu.pc + 0] = 0xF1;
    emu.memory[emu.pc + 1] = 0x18;

    emu.V[1] = 0x09;

    emu.fetch_op_code();

    REQUIRE(emu.sound_timer == 0x09);
    REQUIRE(instruction_was_incremented_normally(emu));
}

TEST_CASE("0xFX1E - Adds VX to I. VF is not affected.", "[OP_CODE]") {

    Chip8Emu emu;

    emu.memory[emu.pc + 0] = 0xF1;
    emu.memory[emu.pc + 1] = 0x1E;

    emu.V[1] = 0x09;
    emu.i_register = 0x01;

    emu.fetch_op_code();

    REQUIRE(emu.i_register == 0x09 + 0x01);
    REQUIRE(instruction_was_incremented_normally(emu));
}

TEST_CASE("0xFX29 - Sets I to the location of the sprite for the character in VX.", "[OP_CODE]") {

    Chip8Emu emu;

    emu.memory[emu.pc + 0] = 0xF1;
    emu.memory[emu.pc + 1] = 0x29;

    emu.V[1] = 0x09;
    emu.i_register = 0x01;

    emu.fetch_op_code();

    REQUIRE(emu.i_register == 0x09 * 0x5);
    REQUIRE(instruction_was_incremented_normally(emu));
}

TEST_CASE("0xFX33 - Stores the binary-coded decimal representation of VX.", "[OP_CODE]") {

    Chip8Emu emu;

    emu.memory[emu.pc + 0] = 0xF1;
    emu.memory[emu.pc + 1] = 0x33;

    emu.V[1] = 128;

    emu.fetch_op_code();

    REQUIRE(emu.memory[emu.i_register + 0] == 1);
    REQUIRE(emu.memory[emu.i_register + 1] == 2);
    REQUIRE(emu.memory[emu.i_register + 2] == 8);

    REQUIRE(instruction_was_incremented_normally(emu));
}

TEST_CASE(
        "0xFX55 - Stores V0 to VX (including VX) in memory starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified.",
        "[OP_CODE]") {

    Chip8Emu emu;

    emu.memory[emu.pc + 0] = 0xF3;
    emu.memory[emu.pc + 1] = 0x55;

    emu.V[0] = 0x02;
    emu.V[1] = 0x04;
    emu.V[2] = 0x08;

    emu.fetch_op_code();

    REQUIRE(emu.memory[emu.i_register + 0] == 0x02);
    REQUIRE(emu.memory[emu.i_register + 1] == 0x04);
    REQUIRE(emu.memory[emu.i_register + 2] == 0x08);

    REQUIRE(emu.i_register == 0);
    REQUIRE(instruction_was_incremented_normally(emu));
}

TEST_CASE(
        "0xFX65 - Fills V0 to VX (including VX) with values from memory starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified.",
        "[OP_CODE]") {

    Chip8Emu emu;

    emu.memory[emu.pc + 0] = 0xF3;
    emu.memory[emu.pc + 1] = 0x65;

    emu.memory[emu.i_register + 0] = 0x02;
    emu.memory[emu.i_register + 1] = 0x04;
    emu.memory[emu.i_register + 2] = 0x08;

    emu.fetch_op_code();

    REQUIRE(emu.V[0] == 0x02);
    REQUIRE(emu.V[1] == 0x04);
    REQUIRE(emu.V[2] == 0x08);

    REQUIRE(emu.i_register == 0);
    REQUIRE(instruction_was_incremented_normally(emu));
}

TEST_CASE("A game file can be loaded and copied into memory, starting at 0x200 (pc)", "[LOADING_GAME]") {

    Chip8Emu emu;

    FakeGameFile fakeGameFile;
    REQUIRE_NOTHROW(emu.load(fakeGameFile.file_name));

    REQUIRE(emu.memory[emu.pc + 0] == 'T');
    REQUIRE(emu.memory[emu.pc + 1] == 'E');
    REQUIRE(emu.memory[emu.pc + 2] == 'S');
    REQUIRE(emu.memory[emu.pc + 3] == 'T');
    REQUIRE(emu.memory[emu.pc + 4] == '_');
    REQUIRE(emu.memory[emu.pc + 5] == 'R');
    REQUIRE(emu.memory[emu.pc + 6] == 'O');
    REQUIRE(emu.memory[emu.pc + 7] == 'M');
    REQUIRE(emu.memory[emu.pc + 8] == '\n');
    REQUIRE(emu.memory[emu.pc + 9] == '\0');

    REQUIRE(emu.game_loaded_flag == true);
}

TEST_CASE("Should throw an error if the game file could not be found", "[LOADING_GAME]") {

    Chip8Emu emu;

    REQUIRE_THROWS_AS(emu.load("BAD_FILE_PATH"), std::runtime_error);

    REQUIRE(is_zeroed_out(emu.memory));
}

TEST_CASE("Should throw an error if a game was not loaded before running a cycle", "[RUN]") {

    Chip8Emu emu;

    REQUIRE_THROWS_AS(emu.run(), std::runtime_error);
}

TEST_CASE("Should run the emulator for a single cycle", "[RUN]") {

    Chip8Emu emu;

    FakeGameFile fakeGameFile;
    REQUIRE_NOTHROW(emu.load(fakeGameFile.file_name));

    REQUIRE(emu.game_loaded_flag);
    REQUIRE_NOTHROW(emu.run());
}

TEST_CASE("Should count down the delay and sound timers by 1 each time the emulator is run when values are above zero",
          "[RUN]") {

    Chip8Emu emu;
    emu.delay_timer = 60;
    emu.sound_timer = 60;

    FakeGameFile fakeGameFile;
    REQUIRE_NOTHROW(emu.load(fakeGameFile.file_name));

    REQUIRE(emu.game_loaded_flag);
    REQUIRE_NOTHROW(emu.run());

    REQUIRE(emu.delay_timer == 59);
    REQUIRE(emu.sound_timer == 59);
}

TEST_CASE("Should not count down the delay and sound timers because the timer values are 0", "[RUN]") {

    Chip8Emu emu;
    emu.sound_timer = 0;
    emu.delay_timer = 0;

    FakeGameFile fakeGameFile;
    REQUIRE_NOTHROW(emu.load(fakeGameFile.file_name));

    REQUIRE(emu.game_loaded_flag);
    REQUIRE_NOTHROW(emu.run());

    REQUIRE(emu.delay_timer == 0);
    REQUIRE(emu.sound_timer == 0);
}

TEST_CASE("Should call the audio callback when the sound timer is 1", "[RUN]") {

    Chip8Emu emu;
    emu.sound_timer = 1;

    bool audio_cb_was_called = false;
    emu.play_audio_cb = [&]() {
        audio_cb_was_called = true;
        return true;
    };

    FakeGameFile fakeGameFile;
    REQUIRE_NOTHROW(emu.load(fakeGameFile.file_name));

    REQUIRE(emu.game_loaded_flag);
    REQUIRE_NOTHROW(emu.run());

    REQUIRE(emu.sound_timer == 0);
    REQUIRE(audio_cb_was_called);
}

TEST_CASE("Should not error out when the sound timer is 1 and no audio callback was provided", "[RUN]") {

    Chip8Emu emu;
    emu.sound_timer = 1;

    FakeGameFile fakeGameFile;
    REQUIRE_NOTHROW(emu.load(fakeGameFile.file_name));

    REQUIRE(emu.game_loaded_flag);
    REQUIRE_NOTHROW(emu.run());

    REQUIRE(emu.sound_timer == 0);
}
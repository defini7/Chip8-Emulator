#pragma once

#include <vector>
#include <cstdint>
#include <cstdio>
#include <string>

#pragma warning(disable : 4996)

class chip8
{
public:
	chip8();
	~chip8();

public: // system
	uint8_t memory[0xFFF];
	uint8_t reg[16];
	uint16_t i;
	uint16_t pc;
	std::vector<uint16_t> stack;

	uint16_t opcode;

	uint8_t delay_timer;
	uint8_t sound_timer;

	const int32_t screen_width = 640;
	const int32_t screen_height = 320;

	// graphics
	uint8_t screen[640 * 320];

	// controls
	uint8_t key_state[16];

public:
	void reset();
	bool load_rom(const std::string& name);

	void next_opcode();
	void decrease_timers();

	int32_t get_key_pressed();
	void press_key(int key);
	void release_key(int key);

	bool (*play_sound)();
	void set_audio(bool (*sound_handler)());

public:
	// opcodes
	void op_00E0();
	void op_00EE();
	void op_1NNN();
	void op_2NNN();
	void op_3XNN();
	void op_4XNN();
	void op_5XY0();
	void op_6XNN();
	void op_7XNN();
	void op_8XY0();
	void op_8XY1();
	void op_8XY2();
	void op_8XY3();
	void op_8XY4();
	void op_8XY5();
	void op_8XY6();
	void op_8XY7();
	void op_8XYE();
	void op_9XY0();
	void op_ANNN();
	void op_BNNN();
	void op_CXNN();
	void op_DXYN();
	void op_EX9E();
	void op_EXA1();
	void op_FX07();
	void op_FX0A();
	void op_FX15();
	void op_FX18();
	void op_FX1E();
	void op_FX29();
	void op_FX33();
	void op_FX55();
	void op_FX65();

public:
	void decode_opcode_0();
	void decode_opcode_8();
	void decode_opcode_e();
	void decode_opcode_f();

};


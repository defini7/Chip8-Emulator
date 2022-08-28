#include "chip8.h"

chip8::chip8()
{
	
}

chip8::~chip8()
{

}

bool chip8::load_rom(const std::string& name)
{
	reset();
	op_00E0(); // clear screen

	FILE* f;
	f = fopen(name.c_str(), "rb");

	if (!f)
		return false;

	fread(&memory[0x200], 0xFFF, 1, f);

	fclose(f);

	return true;
}

void chip8::reset()
{
	i = 0;
	pc = 0x200;

	memset(reg, 0, sizeof(reg));
	memset(memory, 0, sizeof(memory));
	memset(key_state, 0, sizeof(key_state));

	delay_timer = 0;
	sound_timer = 0;
}

void chip8::next_opcode()
{
	opcode = 0;

	opcode = memory[pc++];
	opcode <<= 8;
	opcode |= memory[pc++];
}

void chip8::decrease_timers()
{
	if (delay_timer > 0.0f)
		delay_timer--;

	if (sound_timer > 0)
	{
		sound_timer--;
		play_sound();
	}
}

int32_t chip8::get_key_pressed()
{
	for (int k = 0; k < 16; k++)
	{
		if (key_state[k] > 0)
			return k;
	}

	return -1;
}

void chip8::op_00E0()
{
	for (int x = 0; x < screen_width; x++)
		for (int y = 0; y < screen_height; y++)
			screen[y * screen_width + x] = 255;
}

void chip8::op_00EE()
{
	pc = stack.back();
	stack.pop_back();
}

void chip8::op_1NNN()
{
	pc = opcode & 0x0FFF;
}

void chip8::op_2NNN()
{
	stack.push_back(pc);
	pc = opcode & 0x0FFF;
}

void chip8::op_3XNN()
{
	int32_t x, nn;

	nn = opcode & 0x00FF;

	x = opcode & 0x0F00;
	x = x >> 8;

	if (reg[x] == nn)
		pc += 2;
}

void chip8::op_4XNN()
{
	int32_t nn, x;

	nn = opcode & 0x00FF;
	x = opcode & 0x0F00;
	x = x >> 8;

	if (reg[x] != nn)
		pc += 2;
}

void chip8::op_5XY0()
{
	int32_t x, y;

	x = opcode & 0x0F00;
	x = x >> 8;
	
	y = opcode & 0x00F0;
	y = y >> 4;

	if (reg[x] == reg[y])
		pc += 2;
}

void chip8::op_6XNN()
{
	int32_t nn, x;

	nn = opcode & 0x00FF;
	x = opcode & 0x0F00;
	x = x >> 8;

	reg[x] = nn;
}

void chip8::op_7XNN()
{
	int32_t nn, x;

	nn = opcode & 0x00FF;
	x = opcode & 0x0F00;
	x = x >> 8;

	reg[x] += nn;
}

void chip8::op_8XY0()
{
	int32_t x, y;

	x = opcode & 0x0F00;
	x = x >> 8;

	y = opcode & 0x00F0;
	y = y >> 4;

	reg[x] = reg[y];
}

void chip8::op_8XY1()
{
	int32_t x, y;

	x = opcode & 0x0F00;
	x = x >> 8;

	y = opcode & 0x00F0;
	y = y >> 4;

	reg[x] |= reg[y];
}

void chip8::op_8XY2()
{
	int32_t x, y;

	x = opcode & 0x0F00;
	x = x >> 8;

	y = opcode & 0x00F0;
	y = y >> 4;

	reg[x] &= reg[y];
}

void chip8::op_8XY3()
{
	int32_t x, y;

	x = opcode & 0x0F00;
	x = x >> 8;

	y = opcode & 0x00F0;
	y = y >> 4;

	reg[x] ^= reg[y];
}

void chip8::op_8XY4()
{
	int32_t x, y;

	x = opcode & 0x0F00;
	x = x >> 8;

	y = opcode & 0x00F0;
	y = y >> 4;

	if (reg[x] + reg[y] > 255)
		reg[0xF] = 1;

	reg[x] += reg[y];
}

void chip8::op_8XY5()
{
	reg[0xF] = 1;

	int32_t x, y;

	x = opcode & 0x0F00;
	x = x >> 8;

	y = opcode & 0x00F0;
	y = y >> 4;

	if (reg[x] < reg[y])
		reg[0xF] = 0;

	reg[x] -= reg[y];
}

void chip8::op_8XY6()
{
	int32_t x;

	x = opcode & 0x0F00;
	x = x >> 8;

	reg[0xF] = reg[x] & 0x1;
	reg[x] = reg[x] >> 1;
}

void chip8::op_8XY7()
{
	reg[0xF] = 1;

	int32_t x, y;

	x = opcode & 0x0F00;
	x = x >> 8;

	y = opcode & 0x00F0;
	y = y >> 4;

	if (reg[x] > reg[y])
		reg[0xF] = 0;

	reg[x] = reg[y] - reg[x];
}

void chip8::op_8XYE()
{
	int32_t x;
	
	x = opcode & 0x0F00;
	x = x >> 8;

	reg[0xF] = reg[x] >> 7;
	reg[x] = reg[x] << 1;
}

void chip8::op_9XY0()
{
	int32_t x, y;

	x = opcode & 0x0F00;
	x = x >> 8;

	y = opcode & 0x00F0;
	y = y >> 4;

	if (reg[x] != reg[y])
		pc += 2;
}

void chip8::op_ANNN()
{
	i = opcode & 0x0FFF;
}

void chip8::op_BNNN()
{
	int32_t nnn = opcode & 0x0FFF;
	
	pc = reg[0] + nnn;
}

void chip8::op_CXNN()
{
	int32_t nn, x;

	nn = opcode & 0x00FF;

	x = opcode & 0x0F00;
	x = x >> 8;

	reg[x] = rand() & nn;
}

void chip8::op_DXYN()
{
	const int32_t scale = 10;

	int32_t x, y;

	x = opcode & 0x0F00;
	x = x >> 8;

	y = opcode & 0x00F0;
	y = y >> 4;

	int32_t coord_x, coord_y, height;

	coord_x = reg[x] * scale;
	coord_y = reg[y] * scale;

	height = opcode & 0x000F;

	reg[0xF] = 0;

	for (int yline = 0; yline < height; yline++)
	{
		uint8_t data = memory[i + yline];
		int32_t xpixelinv = 7;

		for (int xpixel = 0; xpixel < 8; xpixel++, xpixelinv--)
		{
			int32_t mask = 1 << xpixelinv;
			if (data & mask)
			{
				int32_t spr_x = xpixel * scale + coord_x;
				int32_t spr_y = yline * scale + coord_y;

				int32_t colour = 0;

				if (screen[spr_y * screen_width + spr_x] == 0)
				{
					colour = 255;
					reg[15] = 1;
				}

				for (int c = 0; c < scale; c++)
				{
					for (int r = 0; r < scale; r++)
					{
						if (spr_y + r < screen_height && spr_x + c < screen_width && spr_x + c >= 0 && spr_y + r >= 0)
							screen[(spr_y + r) * screen_width + (spr_x + c)] = (uint8_t)colour;
					}
				}

			}
		}
	}
}

void chip8::op_EX9E()
{
	int32_t x, key;

	x = opcode & 0x0F00;
	x = x >> 8;

	key = reg[x];

	if (key_state[key] == 1)
		pc += 2;
}

void chip8::op_EXA1()
{
	int32_t x, key;

	x = opcode & 0x0F00;
	x = x >> 8;

	key = reg[x];

	if (key_state[key] == 0)
		pc += 2;
}

void chip8::op_FX07()
{
	int32_t x;

	x = opcode & 0x0F00;
	x = x >> 8;

	reg[x] = delay_timer;
}

void chip8::op_FX0A()
{
	int32_t x;

	x = opcode & 0x0F00;
	x = x >> 8;

	int32_t keypressed = get_key_pressed();

	if (keypressed == -1)
		pc -= 2;
	else
		reg[x] = keypressed;
}

void chip8::op_FX15()
{
	int32_t x;

	x = opcode & 0x0F00;
	x = x >> 8;

	delay_timer = reg[x];
}

void chip8::op_FX18()
{
	int32_t x;

	x = opcode & 0x0F00;
	x = x >> 8;

	sound_timer = reg[x];
}

void chip8::op_FX1E()
{
	int32_t x;

	x = opcode & 0x0F00;
	x = x >> 8;

	i += reg[x];
}

void chip8::op_FX29()
{
	int32_t x;

	x = opcode & 0x0F00;
	x = x >> 8;

	i = reg[x] * 5;
}

void chip8::op_FX33()
{
	int32_t x;

	x = opcode & 0x0F00;
	x = x >> 8;

	int32_t value;

	value = reg[x];

	int32_t hundreds, tens, units;

	hundreds = value / 100;
	tens = (value / 10) % 10;
	units = value % 10;

	memory[i] = hundreds;
	memory[i + 1] = tens;
	memory[i + 2] = units;
}

void chip8::op_FX55()
{
	int32_t x;
	
	x = opcode & 0x0F00;
	x = x >> 8;
	
	for (int k = 0; k <= x; k++)
		memory[i + k] = reg[k];

	i += x + 1;
}

void chip8::op_FX65()
{
	int32_t x;

	x = opcode & 0x0F00;
	x = x >> 8;

	for (int k = 0; k <= x; k++)
		reg[k] = memory[i + k];

	i += x + 1;
}

void chip8::decode_opcode_0()
{
	switch (opcode & 0xF)
	{
	case 0x0: op_00E0(); return;
	case 0xE: op_00EE(); return;
	}
}

void chip8::decode_opcode_8()
{
	switch (opcode & 0xF)
	{
	case 0x0: op_8XY0(); return;
	case 0x1: op_8XY1(); return;
	case 0x2: op_8XY2(); return;
	case 0x3: op_8XY3(); return;
	case 0x4: op_8XY4(); return;
	case 0x5: op_8XY5(); return;
	case 0x6: op_8XY6(); return;
	case 0x7: op_8XY7(); return;
	case 0xE: op_8XYE(); return;
	}
}

void chip8::decode_opcode_e()
{
	switch (opcode & 0xF)
	{
	case 0xE: op_EX9E(); return;
	case 0x1: op_EXA1(); return;
	}
}

void chip8::decode_opcode_f()
{
	switch (opcode & 0xFF)
	{
	case 0x07: op_FX07(); return;
	case 0x0A: op_FX0A(); return;
	case 0x15: op_FX15(); return;
	case 0x18: op_FX18(); return;
	case 0x1E: op_FX1E(); return;
	case 0x29: op_FX29(); return;
	case 0x33: op_FX33(); return;
	case 0x55: op_FX55(); return;
	case 0x65: op_FX65(); return;
	}
}

void chip8::press_key(int key)
{
	key_state[key] = 1;
}

void chip8::release_key(int key)
{
	key_state[key] = 0;
}

void chip8::set_audio(bool (*sound_handler)())
{
	play_sound = sound_handler;
}

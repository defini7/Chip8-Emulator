#include <iostream>

#include "chip8.h"

#include "ConsolaProd.h"

using namespace def;

class Example : public def::ConsolaProd
{
public:
	Example()
	{
		sAppName = L"Chip8 Emulator";
	}

private:
	void UpdateKeys()
	{
		int32_t key = -1;

		if (GetKey(L'X').bHeld) key = 0;
		if (GetKey(L'1').bHeld) key = 1;
		if (GetKey(L'2').bHeld) key = 2;
		if (GetKey(L'3').bHeld) key = 3;
		if (GetKey(L'Q').bHeld) key = 4;
		if (GetKey(L'W').bHeld) key = 5;
		if (GetKey(L'E').bHeld) key = 6;
		if (GetKey(L'A').bHeld) key = 7;
		if (GetKey(L'S').bHeld) key = 8;
		if (GetKey(L'D').bHeld) key = 9;
		if (GetKey(L'Z').bHeld) key = 10;
		if (GetKey(L'C').bHeld) key = 11;
		if (GetKey(L'K').bHeld) key = 12;
		if (GetKey(L'R').bHeld) key = 13;
		if (GetKey(L'F').bHeld) key = 14;
		if (GetKey(L'V').bHeld) key = 15;

		if (key != -1)
			cpu.key_pressed(key);

		if (GetKey(L'X').bReleased) key = 0;
		if (GetKey(L'K').bReleased) key = 1;
		if (GetKey(L'K').bReleased) key = 2;
		if (GetKey(L'K').bReleased) key = 3;
		if (GetKey(L'Q').bReleased) key = 4;
		if (GetKey(L'W').bReleased) key = 5;
		if (GetKey(L'E').bReleased) key = 6;
		if (GetKey(L'A').bReleased) key = 7;
		if (GetKey(L'S').bReleased) key = 8;
		if (GetKey(L'D').bReleased) key = 9;
		if (GetKey(L'Z').bReleased) key = 10;
		if (GetKey(L'C').bReleased) key = 11;
		if (GetKey(L'K').bReleased) key = 12;
		if (GetKey(L'R').bReleased) key = 13;
		if (GetKey(L'F').bReleased) key = 14;
		if (GetKey(L'V').bReleased) key = 15;

		if (key != -1)
			cpu.key_released(key);
	}

	void ClassifyPixel(float r, float g, float b, wchar_t& sym, short& fg_col, short& bg_col)
	{
		float luminance = 0.2987f * r + 0.5870f * g + 0.1140f * b;
		int pixel_bw = (int)(luminance * 13.0f);
		switch (pixel_bw)
		{
		case 0: bg_col = BG_BLACK; fg_col = FG_BLACK; sym = PIXEL_SOLID; break;

		case 1: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_QUARTER; break;
		case 2: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_HALF; break;
		case 3: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_THREEQUARTERS; break;
		case 4: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_SOLID; break;

		case 5: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_QUARTER; break;
		case 6: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_HALF; break;
		case 7: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_THREEQUARTERS; break;
		case 8: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_SOLID; break;

		case 9:  bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_QUARTER; break;
		case 10: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_HALF; break;
		case 11: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_THREEQUARTERS; break;
		case 12: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_SOLID; break;
		}
	}

protected:
	bool OnUserCreate() override
	{
		if (!cpu.load_rom("pong.ch8"))
			return false;

		return true;
	}

	bool OnUserUpdate(float fDeltaTime) override
	{
		UpdateKeys();

		cpu.decrease_timers();

		cpu.next_opcode();

		switch (cpu.opcode & 0xF000)
		{
		case 0x0000: cpu.decode_opcode_0(); break;
		case 0x1000: cpu.op_1NNN();			break;
		case 0x2000: cpu.op_2NNN();			break;
		case 0x3000: cpu.op_3XNN();			break;
		case 0x4000: cpu.op_4XNN();			break;
		case 0x5000: cpu.op_5XY0();			break;
		case 0x6000: cpu.op_6XNN();			break;
		case 0x7000: cpu.op_7XNN();			break;
		case 0x8000: cpu.decode_opcode_8(); break;
		case 0x9000: cpu.op_9XY0();			break;
		case 0xA000: cpu.op_ANNN();			break;
		case 0xB000: cpu.op_BNNN();			break;
		case 0xC000: cpu.op_CXNN();			break;
		case 0xD000: cpu.op_DXYN();			break;
		case 0xE000: cpu.decode_opcode_e(); break;
		case 0xF000: cpu.decode_opcode_f(); break;
		}

		for (int x = 0; x < cpu.screen_width; x++)
			for (int y = 0; y < cpu.screen_height; y++)
			{
				wchar_t sym;
				short bg_col;
				short fg_col;

				ClassifyPixel(
					cpu.screen[y * cpu.screen_width + x].r / 255.0f,
					cpu.screen[y * cpu.screen_width + x].g / 255.0f,
					cpu.screen[y * cpu.screen_width + x].b / 255.0f,
					sym, fg_col, bg_col
				);

				Draw(x, y, sym, fg_col | bg_col);
			}

		return true;
	}

private:
	chip8 cpu;
	
};

int main()
{
	Example demo;

	if (demo.ConstructConsole(640, 320, 2, 2).ok)
		demo.Run();

	return 0;
}

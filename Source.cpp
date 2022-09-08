#include <iostream>

#include "chip8.h"

#include "ConsoleGameEngine.h"

using namespace def;

bool PlaySoundWrap()
{
	return (bool)PlaySoundW(L"beep.wav", nullptr, SND_ASYNC | SND_FILENAME);
}

class Example : public def::ConsoleGameEngine
{
public:
	Example()
	{
		sAppName = L"Chip8 Emulator";
	}

protected:
	bool OnUserCreate() override
	{
		if (!emu.load_rom("roms/invaders.ch8"))
			return false;

		emu.set_audio(PlaySoundWrap);

		return true;
	}

	bool OnUserUpdate(float fDeltaTime) override
	{
		int32_t key = -1;

		if (GetKey(L'X').bPressed) key = 0;
		if (GetKey(L'1').bPressed) key = 1;
		if (GetKey(L'2').bPressed) key = 2;
		if (GetKey(L'3').bPressed) key = 3;
		if (GetKey(L'Q').bPressed) key = 4;
		if (GetKey(L'W').bPressed) key = 5;
		if (GetKey(L'E').bPressed) key = 6;
		if (GetKey(L'A').bPressed) key = 7;
		if (GetKey(L'S').bPressed) key = 8;
		if (GetKey(L'D').bPressed) key = 9;
		if (GetKey(L'Z').bPressed) key = 10;
		if (GetKey(L'C').bPressed) key = 11;
		if (GetKey(L'K').bPressed) key = 12;
		if (GetKey(L'R').bPressed) key = 13;
		if (GetKey(L'F').bPressed) key = 14;
		if (GetKey(L'V').bPressed) key = 15;

		if (key != -1)
			emu.press_key(key);

		key = -1;

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
			emu.release_key(key);

		emu.decrease_timers();

		emu.next_opcode();

		switch (emu.opcode & 0xF000)
		{
		case 0x0000: emu.decode_opcode_0(); break;
		case 0x1000: emu.op_1NNN();			break;
		case 0x2000: emu.op_2NNN();			break;
		case 0x3000: emu.op_3XNN();			break;
		case 0x4000: emu.op_4XNN();			break;
		case 0x5000: emu.op_5XY0();			break;
		case 0x6000: emu.op_6XNN();			break;
		case 0x7000: emu.op_7XNN();			break;
		case 0x8000: emu.decode_opcode_8(); break;
		case 0x9000: emu.op_9XY0();			break;
		case 0xA000: emu.op_ANNN();			break;
		case 0xB000: emu.op_BNNN();			break;
		case 0xC000: emu.op_CXNN();			break;
		case 0xD000: emu.op_DXYN();			break;
		case 0xE000: emu.decode_opcode_e(); break;
		case 0xF000: emu.decode_opcode_f(); break;
		}

		for (int x = 0; x < emu.screen_width; x++)
			for (int y = 0; y < emu.screen_height; y++)
			{
				Draw(
					x, y, PIXEL_SOLID,
					(emu.screen[y * emu.screen_width + x] > 125) ? (FG_WHITE | BG_WHITE) : (FG_BLACK | BG_BLACK)
				);
			}

		return true;
	}

private:
	chip8 emu;
	
};

int main()
{
	Example demo;

	rcode rc = demo.ConstructConsole(640, 320, 2, 2);

	if (rc.ok)
		demo.Run();
	else
		std::cerr << rc.info << '\n';

	return 0;
}

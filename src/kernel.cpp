
#include <common/types.h>
#include <gdt.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/pci.h>
#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <drivers/vga.h>

using namespace lyos;
using namespace lyos::common;
using namespace lyos::drivers;
using namespace lyos::hardwarecommunication;

void printf(char *str)
{
	static uint16_t *VideoMemory = (uint16_t *)0xb8000;

	static uint8_t x = 0, y = 0;

	for (int i = 0; str[i] != '\0'; ++i)
	{

		switch (str[i])
		{
		case '\n':
			y++;
			x = 0;
			break;
		default:
			VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0xFF00) | str[i];
			x++;
			break;
		}

		if (x >= 80)
		{
			y++;
			x = 0;
		}

		if (y >= 25)
		{
			for (y = 0; y < 25; y++)
				for (x = 0; x < 80; x++)
					VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0xFF00) | ' ';
			x = 0;
			y = 0;
		}
	}
}

void printfHex(uint8_t key)
{
	char *foo = "00";
	char *hex = "0123456789ABCDEF";
	foo[0] = hex[(key >> 4) & 0xF];
	foo[1] = hex[key & 0xF];
	printf(foo);
}

class printfKeyboardEventHandler : public KeyboardEventHandler
{
  public:
	void OnkeyDown(char c)
	{
		char *foo = " ";
		foo[0] = c;
		printf(foo);
	}
};

class MouseToConsole : public MouseEventHandler
{
	int8_t x, y;

  public:
	MouseToConsole()
	{
		uint16_t *VideoMemory = (uint16_t *)0xb8000;
		x = 40;
		y = 12;
		VideoMemory[80 * y + x] = ((VideoMemory[80 * y + x] & 0xF000) >> 4) | ((VideoMemory[80 * y + x] & 0x0F00) << 4) | ((VideoMemory[80 * y + x] & 0x00FF));
	}
	void OnMouseMove(int xoffset, int yoffset)
	{
		static uint16_t *VideoMemory = (uint16_t *)0xb8000;

		VideoMemory[80 * y + x] = ((VideoMemory[80 * y + x] & 0xF000) >> 4) | ((VideoMemory[80 * y + x] & 0x0F00) << 4) | ((VideoMemory[80 * y + x] & 0x00FF));

		x += xoffset;

		if (x < 0)
			x = 0;
		if (x >= 80)
			x = 79;

		y -= yoffset;
		if (y < 0)
			y = 0;
		if (y >= 25)
			y = 24;
		VideoMemory[80 * y + x] = ((VideoMemory[80 * y + x] & 0xF000) >> 4) | ((VideoMemory[80 * y + x] & 0x0F00) << 4) | ((VideoMemory[80 * y + x] & 0x00FF));
	}
};

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors()
{
	for (constructor *i = &start_ctors; i != &end_ctors; i++)
		(*i)();
}

extern "C" void kernelMain(void *multiboot_structure, uint32_t magicnumber)
{
	printf("Hello World! --- http:/www.qvjunping.me\n");

	GlobalDescriptorTable gdt;
	InterruptManager interrupts(&gdt);

	printf("Initializing Hardwae, Stage 1\n");

	DriverManager drvManager;

	printfKeyboardEventHandler kbhandler;
	KeyboardDriver keyboard(&interrupts, &kbhandler);
	drvManager.AddDriver(&keyboard);

	MouseToConsole mousehandler;
	MouseDriver mouse(&interrupts, &mousehandler);
	drvManager.AddDriver(&mouse);

	PeripheralComponentInterconectController PCIController;
	PCIController.SelectDrivers(&drvManager, &interrupts);

	VideoGraphicsArray vga;

	printf("Initializing Hardwae, Stage 2\n");
	drvManager.ActivateAll();

	printf("Initializing Hardwae, Stage 3\n");
	interrupts.Activate();

	vga.SetMode(320, 200, 8);
	for (uint32_t y = 0; y < 200; y++)
		for (uint32_t x = 0; x < 320; x++)
		{
			vga.PutPixel(x, y, 0x00, 0x00, 0xA8);
		}

	while (1)
		;
}

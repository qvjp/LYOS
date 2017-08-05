
#include <common/types.h>
#include <gdt.h>
#include <memorymanagement.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/pci.h>
#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <drivers/vga.h>
#include <drivers/ata.h>
#include <gui/desktop.h>
#include <gui/window.h>
#include <multitasking.h>

#include <drivers/amd_am79c973.h>
// #define GRAPHICSMODE

using namespace lyos;
using namespace lyos::common;
using namespace lyos::drivers;
using namespace lyos::hardwarecommunication;
using namespace lyos::gui;

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

void taskA()
{
	while (true)
	{
		printf("A");
	}
}
void taskB()
{
	while (true)
	{
		printf("B");
	}
}

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

	uint32_t *memupper = (uint32_t *)(((size_t)multiboot_structure) + 8);
	size_t heap = 10 * 1024 * 1024;
	MemoryManager memoryManager(heap, (*memupper) * 1024 - heap - 10 * 1024);

	printf("heap: 0x");
	printfHex((heap >> 24) & 0xFF);
	printfHex((heap >> 16) & 0xFF);
	printfHex((heap >> 8) & 0xFF);
	printfHex((heap)&0xFF);

	void *allocated = memoryManager.malloc(1024);
	printf("\nallocated: 0x");
	printfHex(((size_t)allocated >> 24) & 0xFF);
	printfHex(((size_t)allocated >> 16) & 0xFF);
	printfHex(((size_t)allocated >> 8) & 0xFF);
	printfHex(((size_t)allocated) & 0xFF);
	printf("\n");

	TaskManager taskManager;
	/*
	Task task1(&gdt, taskA);
	Task task2(&gdt, taskB);
	taskManager.AddTask(&task1);
	taskManager.AddTask(&task2);
	*/
	InterruptManager interrupts(0x20, &gdt, &taskManager);

	printf("Initializing Hardwae, Stage 1\n");
#ifdef GRAPHICSMODE
	Desktop desktop(320, 200, 0x00, 0xA8, 0x00);
#endif
	DriverManager drvManager;
#ifdef GRAPHICSMODE

	KeyboardDriver keyboard(&interrupts, &desktop);
#else
	printfKeyboardEventHandler kbhandler;
	KeyboardDriver keyboard(&interrupts, &kbhandler);
#endif
	drvManager.AddDriver(&keyboard);

#ifdef GRAPHICSMODE
	MouseDriver mouse(&interrupts, &desktop);
#else
	MouseToConsole mousehandler;
	MouseDriver mouse(&interrupts, &mousehandler);
#endif
	drvManager.AddDriver(&mouse);

	PeripheralComponentInterconectController PCIController;
	PCIController.SelectDrivers(&drvManager, &interrupts);

	VideoGraphicsArray vga;

	printf("Initializing Hardwae, Stage 2\n");
	drvManager.ActivateAll();

	printf("Initializing Hardwae, Stage 3\n");
#ifdef GRAPHICSMODE
	vga.SetMode(320, 200, 8);

	Window win1(&desktop, 10, 10, 20, 20, 0xA8, 0x00, 0x00);
	desktop.AddChild(&win1);
	Window win2(&desktop, 150, 150, 30, 40, 0x00, 0x00, 0xA8);
	desktop.AddChild(&win2);
#endif

	// interrupt 14
	AdvancedTechnologyAttachment ata0m(0x1F0, true);
	printf("ATA Primary Master: ");
	ata0m.Identify();

	AdvancedTechnologyAttachment ata0s(0x1F0, false);
	printf("ATA Primary Slave: ");
	ata0s.Identify();

	char *atabuffer = "http://www.qvjunping.me";
	ata0s.Write28(0, (uint8_t *)atabuffer, 23);
	ata0s.Flush();

	ata0s.Read28(0, (uint8_t *)atabuffer, 23);

	// interrupt 15
	AdvancedTechnologyAttachment atalm(0x170, true);
	AdvancedTechnologyAttachment atals(0x170, false);

	/*
	amd_am79c973 *eth0 = (amd_am79c973 *)(drvManager.drivers[2]);
	eth0->Send((uint8_t *)"HELLO NETWORK", 13); 
	*/
	interrupts.Activate();

	while (1)
	{
#ifdef GRAPHICSMODE
		desktop.Draw(&vga);
#endif
	}
}

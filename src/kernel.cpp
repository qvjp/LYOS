#include <common/types.h>
#include <gdt.h>
#include <hardwarecommunication/interrupts.h>

using namespace lyos;
using namespace lyos::common;
using namespace lyos::hardwarecommunication;

/*
先前的printf不能换行、换页，所以重写

void printf(char *str) //由于不能使用标准库，所以要自已写一个简易的printf()
{
		unsigned short* VideoMemory = (unsigned short*)0xB8000; //VGA所使用的显存，通过一个窗口对应于PC的主存，典型地来说地址的开始点是0xB800
		for(int i = 0; str[i] != '\0'; ++i)
		{
				VideoMemory[i] = (VideoMemory[i] & 0xFF00) | str[i];//16色 设置为黑底白字
		}
}
*/
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
//16进制打印
void printfHex(uint8_t key)
{
	char *foo = "00";
	char *hex = "0123456789ABCDEF";
	foo[0] = hex[(key >> 4) & 0xF];
	foo[1] = hex[key & 0xF];
	printf(foo);
}

typedef void (*constructor)(); //定义函数指针类型constructor 返回值为void、无参数
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors()
{
	for (constructor *i = &start_ctors; i != &end_ctors; i++)
	{
		(*i)();
	}
}

extern "C" void kernelMain(void *multiboot_structure, unsigned int magicnumber) //kernel的主函数，入口
{																				//extern "C"：按照类C的编译和连接规约来编译和连接，而不是C++的编译的连接规约。因为按C++的规约编译和连接会使loader.s中call kernelMain发生错误。
	printf("Hello World!");
	GlobalDescriptorTable gdt;					//初始化全局描述符表
	InterruptManager interrupts(0x20, &gdt);	//初始化中断管理器

	interrupts.Activate();						//开启中断
	while (1)
		; //让系统一直执行
}
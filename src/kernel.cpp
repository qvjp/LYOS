#include<common/types.h>
#include<gdt.h>
using namespace lyos;
using namespace lyos::common;

void printf(char *str) //由于不能使用标准库，所以要自已写一个简易的printf()
{
		unsigned short* VideoMemory = (unsigned short*)0xB8000; //VGA所使用的显存，通过一个窗口对应于PC的主存，典型地来说地址的开始点是0xB800
		for(int i = 0; str[i] != '\0'; ++i)
		{
				VideoMemory[i] = (VideoMemory[i] & 0xFF00) | str[i];//16色 设置为黑底白字
		}
}

typedef void (*constructor)();//定义函数指针类型constructor 返回值为void、无参数
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors()
{
		for(constructor* i = &start_ctors; i != &end_ctors; i++)
		{
			(*i)();
		}
}

extern "C" void kernelMain(void *multiboot_structure, unsigned int magicnumber) //kernel的主函数，入口
{	//extern "C"：按照类C的编译和连接规约来编译和连接，而不是C++的编译的连接规约。因为按C++的规约编译和连接会使loader.s中call kernelMain发生错误。
	printf("Hello World!");
	GlobalDescriptorTable gdt;
	while(1);//让系统一直执行	
}


#include <gdt.h>
using namespace lyos;
using namespace lyos::common;

GlobalDescriptorTable::GlobalDescriptorTable()
	: nullSegmentSelector(0, 0, 0),
	  unusedSegmentSelector(0, 0, 0),				  //Selector 0x00 不能被使用
	  codeSegmentSelector(0, 64 * 1024 * 1024, 0x9A), //Selector 0x08 作为代码段
	  dataSegmentSelector(0, 64 * 1024 * 1024, 0x92)  //Selector 0x10 作为数据段
{
	uint32_t i[2];									  //LGDT参数
	i[1] = (uint32_t)this;							  //入口地址
	i[0] = (sizeof(GlobalDescriptorTable) - 1) << 16; //大小

	asm volatile("lgdt (%0)" //在构造函数中加载gdt
				 :
				 : "p"(((uint8_t *)i) + 2));
}

GlobalDescriptorTable::~GlobalDescriptorTable()
{
}

uint16_t GlobalDescriptorTable::DataSegmentSelector() //数据段选择子地址偏移量
{
	return (uint8_t *)&dataSegmentSelector - (uint8_t *)this;
}

uint16_t GlobalDescriptorTable::CodeSegmentSelector() //代码段选择子地址偏移量
{
	return (uint8_t *)&codeSegmentSelector - (uint8_t *)this;
}

//段描述符
GlobalDescriptorTable::SegmentDescriptor::SegmentDescriptor(uint32_t base, uint32_t limit, uint8_t access)
{
	uint8_t *target = (uint8_t *)this;
	if (limit <= 65536)
	{
		target[6] = 0x40; //设置Gr为0
	}
	else
	{
		if ((limit & 0xFFF) != 0xFFF) //大于4G
			limit = (limit >> 12) - 1;
		else //小于4G
			limit = limit >> 12;

		target[6] = 0xC0; //设置Gr为1
	}
	//limit
	target[0] = limit & 0xFF;
	target[1] = (limit >> 8) & 0xFF;
	target[6] |= (limit >> 16) & 0xF;

	//base
	target[2] = base & 0xFF;
	target[3] = (base >> 8) & 0xFF;
	target[4] = (base >> 16) & 0xFF;
	target[7] = (base >> 24) & 0xFF;

	//type
	target[5] = access;
}

uint32_t GlobalDescriptorTable::SegmentDescriptor::Base() //返回Base值
{
	uint8_t *target = (uint8_t *)this;
	uint32_t result = target[7];
	result = (result << 8) + target[4];
	result = (result << 8) + target[3];
	result = (result << 8) + target[2];
	return result;
}

uint32_t GlobalDescriptorTable::SegmentDescriptor::Limit() //返回Limit值
{
	uint8_t *target = (uint8_t *)this;
	uint32_t result = target[6] & 0xF;
	result = (result << 8) + target[1];
	result = (result << 8) + target[0];

	if ((target[6] & 0xC0) == 0xC0)
		result = (result << 12) | 0xFFF;
	return result;
}

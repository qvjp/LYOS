#ifndef __LYOS__GDT_H
#define __LYOS__GDT_H

#include <common/types.h>

namespace lyos
{
	class GlobalDescriptorTable 			//全局描述表
	{
		public:
			class SegmentDescriptor //段描述符
			{
				private:
					common::uint16_t limit_lo;				//limit的低16位
					common::uint16_t base_lo;				//base的低16位
					common::uint8_t base_hi;				//base的高16位
					common::uint8_t access;					//8位对自身的访问权限
					common::uint8_t flags_limit_hi; 		//limit的高4位和flags所占4位
					common::uint8_t base_vhi;				//base的最高8位

				public:
					SegmentDescriptor(common::uint32_t base, common::uint32_t limit, common::uint8_t access);
					common::uint32_t Base(); 	//返回基址地址
					common::uint32_t Limit(); 	//返回limit大小

			} __attribute__((packed));

		private:
			SegmentDescriptor nullSegmentSelector;
			SegmentDescriptor unusedSegmentSelector;
			SegmentDescriptor codeSegmentSelector;
			SegmentDescriptor dataSegmentSelector;

		public:
			GlobalDescriptorTable();
			~GlobalDescriptorTable();

			common::uint16_t CodeSegmentSelector(); //代码段选择子偏移量
			common::uint16_t DataSegmentSelector(); //数据段选择子偏移量
	};
}
#endif

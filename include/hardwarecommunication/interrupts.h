#ifndef __LYOS__INTERRUPTS_H
#define __LYOS__INTERRUPTS_H

#include <common/types.h>
#include <hardwarecommunication/port.h>
#include <gdt.h>

namespace lyos
{
		namespace hardwarecommunication
		{
				class InterruptManager;

				class InterruptHandler
				{
						protected:
								common::uint8_t interruptNumber;
								InterruptManager *interruptManager;
								InterruptHandler(InterruptManager *interruptmanager, common::uint8_t InterruptNumber);
								~InterruptHandler();

						public:
								virtual common::uint32_t HandleInterrupt(common::uint32_t esp);
				};

				class InterruptManager
				{
						friend class InterruptHandler;

						protected:
						static InterruptManager *ActiveInterruptManager;
						InterruptHandler *handlers[256];

						struct GateDescriptor
						{
								common::uint16_t handlerAddressLowBits;
								common::uint16_t gdt_codeSegmentSelector;
								common::uint8_t reserved;
								common::uint8_t access;
								common::uint16_t handlerAddressHighBits;

						} __attribute__((packed));

						static GateDescriptor interruptDescriptorTable[256];

						struct InterruptDescriptorTablePointer
						{
								common::uint16_t size;
								common::uint32_t base;
						} __attribute__((packed));

						common::uint16_t hardwareInterruptOffset;

						static void SetInterruptDescriptorTableEntry(
										common::uint8_t interruptNumber,
										common::uint16_t codeSegmentSelectorOffset,
										void (*handler)(),
										common::uint8_t DescriptorPrivilegeLevel,
										common::uint8_t DescriptorType);

						static common::uint32_t HandleInterrupt(common::uint8_t interruptNumber, common::uint32_t esp);
						common::uint32_t DoHandleInterrupt(common::uint8_t interrupt, common::uint32_t esp);

						static void IgnoreInterruptRequest();

						static void HandleInterruptRequest0x00(); 
						static void HandleInterruptRequest0x01(); 

						static void HandleException0x00();
						static void HandleException0x01();

						Port8BitSlow picMasterCommandPort;
						Port8BitSlow picMasterDataPort;
						Port8BitSlow picSlaveCommandPort;
						Port8BitSlow picSlaveDataPort;

						public:
						InterruptManager(common::uint16_t hardwareInterruptOffset,GlobalDescriptorTable *gdt);
						~InterruptManager();
						common::uint16_t HardwareInterruptoffset();
						void Activate();
						void Deactivate();
				};
		}
}

#endif

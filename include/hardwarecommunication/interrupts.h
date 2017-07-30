#ifndef __LYOS__HARDWARECOMMUNICATION__INTERRUPTS_H
#define __LYOS__HARDWARECOMMUNICATION__INTERRUPTS_H

#include <gdt.h>
#include <multitasking.h>
#include <common/types.h>
#include <hardwarecommunication/port.h>

namespace lyos
{
namespace hardwarecommunication
{
class InterruptManager;

class InterruptHandler
{
  protected:
    lyos::common::uint8_t interruptNumber;
    InterruptManager *interruptManager;

    InterruptHandler(lyos::common::uint8_t interruptNumber, InterruptManager *interruptManager);

    ~InterruptHandler();

  public:
    virtual lyos::common::uint32_t HandleInterrupt(lyos::common::uint32_t esp);
};

class InterruptManager
{
    friend class InterruptHandler;

  protected:
    static InterruptManager *ActiveInterruptManager;
    InterruptHandler *handlers[256];
    TaskManager *taskManager;

    struct GateDescriptor
    {
        lyos::common::uint16_t handlerAddressLowBits;
        lyos::common::uint16_t gdt_codeSegmentSelector;
        lyos::common::uint8_t reserved;
        lyos::common::uint8_t access;
        lyos::common::uint16_t handlerAddressHighBits;

    } __attribute__((packed));

    static GateDescriptor interruptDescriptorTable[256];

    struct InterruptDescriptorTablePointer
    {
        lyos::common::uint16_t size;
        lyos::common::uint32_t base;
    } __attribute__((packed));

    static void SetInterruptDescriptorTableEntry(
        lyos::common::uint8_t interruptNumber,
        lyos::common::uint16_t codeSegmentSelectorOffset,
        void (*handler)(),
        lyos::common::uint8_t DescriptorPrivilegeLevel,
        lyos::common::uint8_t DescriptorType);

    Port8BitSlow picMasterCommand;
    Port8BitSlow picMasterData;
    Port8BitSlow picSlaveCommand;
    Port8BitSlow picSlaveData;

  public:
    InterruptManager(lyos::GlobalDescriptorTable *gdt, lyos::TaskManager* taskManager);
    ~InterruptManager();

    void Activate();
    void Deactivate();

    static lyos::common::uint32_t HandleInterrupt(lyos::common::uint8_t interruptNumber, lyos::common::uint32_t esp);
    lyos::common::uint32_t DoHandleInterrupt(lyos::common::uint8_t interruptNumer, lyos::common::uint32_t esp);
    static void IgnoreInterruptRequest();
    static void HandleInterruptRequest0x00();
    static void HandleInterruptRequest0x01();
    static void HandleInterruptRequest0x0C();
};
}
}

#endif
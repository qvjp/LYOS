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
    lyos::common::uint8_t InterruptNumber;
    InterruptManager *interruptManager;

    InterruptHandler(InterruptManager *interruptManager, lyos::common::uint8_t InterruptNumber);

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

    lyos::common::uint16_t hardwareInterruptOffset;
    static void SetInterruptDescriptorTableEntry(
        lyos::common::uint8_t interrupt,
        lyos::common::uint16_t codeSegmentSelectorOffset,
        void (*handler)(),
        lyos::common::uint8_t DescriptorPrivilegeLevel,
        lyos::common::uint8_t DescriptorType);

    static void InterruptIgnore();

    static void HandleInterruptRequest0x00();
    static void HandleInterruptRequest0x01();
    static void HandleInterruptRequest0x02();
    static void HandleInterruptRequest0x03();
    static void HandleInterruptRequest0x04();
    static void HandleInterruptRequest0x05();
    static void HandleInterruptRequest0x06();
    static void HandleInterruptRequest0x07();
    static void HandleInterruptRequest0x08();
    static void HandleInterruptRequest0x09();
    static void HandleInterruptRequest0x0A();
    static void HandleInterruptRequest0x0B();
    static void HandleInterruptRequest0x0C();
    static void HandleInterruptRequest0x0D();
    static void HandleInterruptRequest0x0E();
    static void HandleInterruptRequest0x0F();
    static void HandleInterruptRequest0x31();

    static void HandleException0x00();
    static void HandleException0x01();
    static void HandleException0x02();
    static void HandleException0x03();
    static void HandleException0x04();
    static void HandleException0x05();
    static void HandleException0x06();
    static void HandleException0x07();
    static void HandleException0x08();
    static void HandleException0x09();
    static void HandleException0x0A();
    static void HandleException0x0B();
    static void HandleException0x0C();
    static void HandleException0x0D();
    static void HandleException0x0E();
    static void HandleException0x0F();
    static void HandleException0x10();
    static void HandleException0x11();
    static void HandleException0x12();
    static void HandleException0x13();

    static lyos::common::uint32_t HandleInterrupt(lyos::common::uint8_t interrupt, lyos::common::uint32_t esp);
    lyos::common::uint32_t DoHandleInterrupt(lyos::common::uint8_t interrupt, lyos::common::uint32_t esp);

    Port8BitSlow programmableInterruptControllerMasterCommandPort;
    Port8BitSlow programmableInterruptControllerMasterDataPort;
    Port8BitSlow programmableInterruptControllerSlaveCommandPort;
    Port8BitSlow programmableInterruptControllerSlaveDataPort;

  public:
    InterruptManager(lyos::common::uint16_t hardwareInterruptOffset, lyos::GlobalDescriptorTable *globalDescriptorTable, lyos::TaskManager *taskManager);
    ~InterruptManager();
    lyos::common::uint16_t HardwareInterruptOffset();
    void Activate();
    void Deactivate();
};
}
}

#endif
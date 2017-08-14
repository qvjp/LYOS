#include <hardwarecommunication/interrupts.h>
using namespace lyos;
using namespace lyos::common;
using namespace lyos::hardwarecommunication;

void printf(char *str);
void printfHex(uint8_t);

InterruptHandler::InterruptHandler(InterruptManager *interruptManager, uint8_t interruptNumber)
{
    this->interruptNumber = interruptNumber;
    this->interruptManager = interruptManager;
    interruptManager->handlers[interruptNumber] = this;
}

InterruptHandler::~InterruptHandler()
{
    if (interruptManager->handlers[interruptNumber] == this)
    {
        interruptManager->handlers[interruptNumber] = 0;
    }
}
uint32_t InterruptHandler::HandleInterrupt(uint32_t esp)
{
    return esp;
}

InterruptManager::GateDescriptor InterruptManager::interruptDescriptorTable[256];
InterruptManager *InterruptManager::ActiveInterruptManager = 0;

void InterruptManager::SetInterruptDescriptorTableEntry(
    uint8_t interruptNumber,
    uint16_t codeSegmentSelectorOffset,
    void (*handler)(),
    uint8_t DescriptorPrivilegeLevel,
    uint8_t DescriptorType)
{

    const uint8_t IDT_DESC_PRESENT = 0x80;
    interruptDescriptorTable[interruptNumber].handlerAddressLowBits = ((uint32_t)handler) & 0xFFFF;
    interruptDescriptorTable[interruptNumber].handlerAddressHighBits = (((uint32_t)handler) >> 16) & 0xFFFF;
    interruptDescriptorTable[interruptNumber].gdt_codeSegmentSelector = codeSegmentSelectorOffset;
    interruptDescriptorTable[interruptNumber].access = IDT_DESC_PRESENT | DescriptorType | ((DescriptorPrivilegeLevel & 3) << 5);
    interruptDescriptorTable[interruptNumber].reserved = 0;
}
InterruptManager::InterruptManager(uint16_t hardwareInterruptOffset, GlobalDescriptorTable *gdt)
    : picMasterCommandPort(0x20),
      picMasterDataPort(0x21),
      picSlaveCommandPort(0xA0),
      picSlaveDataPort(0xA1)
{
    this->hardwareInterruptOffset = hardwareInterruptOffset;
    uint16_t CodeSegment = gdt->CodeSegmentSelector();
    const uint8_t IDT_INTERRUPT_GATE = 0xE;

    for (uint16_t i = 0; i < 256; i++)
    {
        SetInterruptDescriptorTableEntry(i, CodeSegment, &IgnoreInterruptRequest, 0, IDT_INTERRUPT_GATE);
        handlers[i] = 0;
    }
    SetInterruptDescriptorTableEntry(0, CodeSegment, &IgnoreInterruptRequest, 0, IDT_INTERRUPT_GATE);

    SetInterruptDescriptorTableEntry(0x00, CodeSegment, &HandleException0x00, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x01, CodeSegment, &HandleException0x01, 0, IDT_INTERRUPT_GATE);

    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x00, CodeSegment, &HandleInterruptRequest0x00, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x01, CodeSegment, &HandleInterruptRequest0x01, 0, IDT_INTERRUPT_GATE);

    picMasterCommandPort.Write(0x11);
    picSlaveCommandPort.Write(0x11);

    picMasterDataPort.Write(hardwareInterruptOffset);
    picSlaveDataPort.Write(hardwareInterruptOffset + 8);

    picMasterDataPort.Write(0x04);
    picSlaveDataPort.Write(0x02);

    picMasterDataPort.Write(0x01);
    picSlaveDataPort.Write(0x01);

    picMasterDataPort.Write(0x00);
    picSlaveDataPort.Write(0x00);

    InterruptDescriptorTablePointer idt_pointer;
    idt_pointer.size = 256 * sizeof(GateDescriptor) - 1;
    idt_pointer.base = (uint32_t)interruptDescriptorTable;
    asm volatile("lidt %0"
                 :
                 : "m"(idt_pointer));
}
InterruptManager::~InterruptManager()
{
    Deactivate();
}

uint16_t InterruptManager::HardwareInterruptoffset()
{
    return hardwareInterruptOffset;
}

void InterruptManager::Activate()
{
    if (ActiveInterruptManager != 0)
        ActiveInterruptManager->Deactivate();

    ActiveInterruptManager = this;
    asm("sti");
}

void InterruptManager::Deactivate()
{
    if (ActiveInterruptManager == this)
    {
        ActiveInterruptManager = 0;
        asm("cli");
    }
}

uint32_t InterruptManager::HandleInterrupt(uint8_t interrupt, uint32_t esp)
{
    if (ActiveInterruptManager != 0)
        printf("\nInterrupt");
    return ActiveInterruptManager->DoHandleInterrupt(interrupt, esp);
    return esp;
}

uint32_t InterruptManager::DoHandleInterrupt(uint8_t interrupt, uint32_t esp)
{
    if (handlers[interrupt] != 0)
    {
        esp = handlers[interrupt]->HandleInterrupt(esp);
    }
    else if (interrupt != hardwareInterruptOffset)
    {
        printf("UNHANDLED INTERRUPT 0x");
        printfHex(interrupt);
    }

    // if(interrupt == hardwareInterruptOffset)
    // {
    //     esp = (uint32_t)taskManager->Schedule((CPUState*)esp);
    // }

    // hardware interrupts must be acknowledged
    if (hardwareInterruptOffset <= interrupt && interrupt < hardwareInterruptOffset + 16)
    {
        picMasterCommandPort.Write(0x20);
        if (hardwareInterruptOffset + 8 <= interrupt)
            picSlaveCommandPort.Write(0x20);
    }
    return esp;
}
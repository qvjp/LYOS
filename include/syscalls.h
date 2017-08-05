#ifndef __LYOS__SYSCALLS_H
#define __LYOS__SYSCALLS_H
#include <common/types.h>
#include <hardwarecommunication/interrupts.h>
#include <multitasking.h>

namespace lyos
{
class SyscallHandler : public hardwarecommunication::InterruptHandler
{
  public:
    SyscallHandler(hardwarecommunication::InterruptManager *interruptManager, lyos::common::uint8_t InterruptNumber);
    ~SyscallHandler();
    virtual lyos::common::uint32_t HandleInterrupt(lyos::common::uint32_t esp);
};
}

#endif
#ifndef __LYOS__DRIVERS__KEYBOARD_H
#define __LYOS__DRIVERS__KEYBOARD_H

#include <common/types.h>
#include <hardwarecommunication/interrupts.h>
#include <drivers/driver.h>
#include <hardwarecommunication/port.h>

namespace lyos
{
namespace drivers
{
class KeyboardEventHandler
{
public:
  KeyboardEventHandler();

  virtual void OnkeyDown(char);
  virtual void OnkeyUp(char);
};

class KeyboardDriver : public lyos::hardwarecommunication::InterruptHandler,
                       public Driver
{
  lyos::hardwarecommunication::Port8Bit dataport;
  lyos::hardwarecommunication::Port8Bit commandport;
  KeyboardEventHandler *handler;

public:
  KeyboardDriver(lyos::hardwarecommunication::InterruptManager *manager, KeyboardEventHandler *handler);
  ~KeyboardDriver();
  virtual lyos::common::uint32_t HandleInterrupt(lyos::common::uint32_t esp);
  virtual void Activate();
};
}
}

#endif
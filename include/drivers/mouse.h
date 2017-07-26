#ifndef __LYOS__DRIVERS__MOUSE_H
#define __LYOS__DRIVERS__MOUSE_H

#include <common/types.h>
#include <hardwarecommunication/port.h>
#include <drivers/driver.h>
#include <hardwarecommunication/interrupts.h>

namespace lyos
{
  namespace drivers
  {
    class MouseEventHandler
    {
    public:
      MouseEventHandler();

      virtual void OnActivate();
      virtual void OnMouseDown(lyos::common::uint8_t button);
      virtual void OnMouseUp(lyos::common::uint8_t button);
      virtual void OnMouseMove(int x, int y);
    };

    class MouseDriver : public lyos::hardwarecommunication::InterruptHandler, public Driver
    {
      lyos::hardwarecommunication::Port8Bit dataport;
      lyos::hardwarecommunication::Port8Bit commandport;
      lyos::common::uint8_t buffer[3];
      lyos::common::uint8_t offset;
      lyos::common::uint8_t buttons;
      MouseEventHandler *handler;

    public:
      MouseDriver(lyos::hardwarecommunication::InterruptManager *manager, MouseEventHandler *handler);
      ~MouseDriver();
      virtual lyos::common::uint32_t HandleInterrupt(lyos::common::uint32_t esp);
      virtual void Activate();
    };
  }
}

#endif
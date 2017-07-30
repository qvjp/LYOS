#include <drivers/mouse.h>

using namespace lyos::common;
using namespace lyos::drivers;
using namespace lyos::hardwarecommunication;

MouseEventHandler::MouseEventHandler()
{
}
void MouseEventHandler::OnActivate()
{
}
void MouseEventHandler::OnMouseDown(uint8_t button)
{
}
void MouseEventHandler::OnMouseUp(uint8_t button)
{
}
void MouseEventHandler::OnMouseMove(int x, int y)
{
}

MouseDriver::MouseDriver(InterruptManager *manager, MouseEventHandler *handler)
    : InterruptHandler(0x2C, manager),
      dataport(0x60),
      commandport(0x64)
{
    this->handler = handler;
}

MouseDriver::~MouseDriver()
{
}

void printf(char *);

void MouseDriver::Activate()
{
    offset = 0;
    buttons = 0;

    commandport.Write(0xA8);
    commandport.Write(0x20);
    uint8_t status = dataport.Read() | 2;
    commandport.Write(0x60);
    dataport.Write(status);
    commandport.Write(0xD4);
    dataport.Write(0xF4);
    dataport.Read();
}

uint32_t MouseDriver::HandleInterrupt(uint32_t esp)
{
    uint8_t status = commandport.Read();
    if (!(status & 0x20))
    {
        return esp;
    }

    // static int8_t x = 40, y = 12;

    buffer[offset] = dataport.Read();
    if(handler == 0)
    {
        return esp;
    }
    offset = (offset + 1) % 3;

    if (offset == 0)
    {
        if (buffer[1] != 0 || buffer[2] != 0)
        {
            handler->OnMouseMove((int8_t)buffer[1], -((int8_t)buffer[2]));
        }

        for (uint8_t i = 0; i < 3; i++)
        {
            if ((buffer[0] & (0x01 << i)) != (buttons & (0x01 << i)))
            {
                if(buttons & (0x1<<i))
                handler->OnMouseUp(i+1);
                else
                handler->OnMouseDown(i+1);
            }
        }
        buttons = buffer[0];
    }

    return esp;
}

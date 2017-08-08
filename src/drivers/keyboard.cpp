#include <drivers/keyboard.h>

using namespace lyos::common;
using namespace lyos::drivers;
using namespace lyos::hardwarecommunication;

KeyboardEventHandler::KeyboardEventHandler()
{
}
void KeyboardEventHandler::OnkeyDown(char)
{
}
void KeyboardEventHandler::OnkeyUp(char)
{
}

KeyboardDriver::KeyboardDriver(InterruptManager *manager,KeyboardEventHandler *handler)
    : InterruptHandler(manager, 0x21),
      dataport(0x60),
      commandport(0x64)
{
    this->handler = handler;
}

KeyboardDriver::~KeyboardDriver()
{
}

void printf(char *);
void printfHex(uint8_t);
void KeyboardDriver::Activate()
{
    while (commandport.Read() & 0x1)
    {
        dataport.Read();
    }
    commandport.Write(0xAE);
    commandport.Write(0x20);
    uint8_t status = (dataport.Read() | 1) & ~0x10;
    commandport.Write(0x60);
    dataport.Write(status);

    dataport.Write(0xF4);
}

uint32_t KeyboardDriver::HandleInterrupt(uint32_t esp)
{
    uint8_t key = dataport.Read();
    static bool Shift = false;
    if(handler == 0)
    {
        return esp;
    }
    switch (key)
    {
    case 0x02:
        if (Shift)
            handler->OnkeyDown('!');
        else
            handler->OnkeyDown('1');
        break;
    case 0x03:
        if (Shift)
            handler->OnkeyDown('@');
        else
            handler->OnkeyDown('2');
        break;
    case 0x04:
        if (Shift)
            handler->OnkeyDown('#');
        else
            handler->OnkeyDown('3');
        break;
    case 0x05:
        if (Shift)
            handler->OnkeyDown('$');
        else
            handler->OnkeyDown('4');
        break;
    case 0x06:
        if (Shift)
            handler->OnkeyDown('%');
        else
            handler->OnkeyDown('5');
        break;
    case 0x07:
        if (Shift)
            handler->OnkeyDown('^');
        else
            handler->OnkeyDown('6');
        break;
    case 0x08:
        if (Shift)
            handler->OnkeyDown('&');
        else
            handler->OnkeyDown('7');
        break;
    case 0x09:
        if (Shift)
            handler->OnkeyDown('*');
        else
            handler->OnkeyDown('8');
        break;
    case 0x0A:
        if (Shift)
            handler->OnkeyDown('(');
        else
            handler->OnkeyDown('9');
        break;
    case 0x0B:
        if (Shift)
            handler->OnkeyDown(')');
        else
            handler->OnkeyDown('0');
        break;

    case 0x10:
        if (Shift)
            handler->OnkeyDown('Q');
        else
            handler->OnkeyDown('q');
        break;
    case 0x11:
        if (Shift)
            handler->OnkeyDown('W');
        else
            handler->OnkeyDown('w');
        break;
    case 0x12:
        if (Shift)
            handler->OnkeyDown('E');
        else
            handler->OnkeyDown('e');
        break;
    case 0x13:
        if (Shift)
            handler->OnkeyDown('R');
        else
            handler->OnkeyDown('r');
        break;
    case 0x14:
        if (Shift)
            handler->OnkeyDown('T');
        else
            handler->OnkeyDown('t');
        break;
    case 0x15:
        if (Shift)
            handler->OnkeyDown('Y');
        else
            handler->OnkeyDown('y');
        break;
    case 0x16:
        if (Shift)
            handler->OnkeyDown('U');
        else
            handler->OnkeyDown('u');
        break;
    case 0x17:
        if (Shift)
            handler->OnkeyDown('I');
        else
            handler->OnkeyDown('i');
        break;
    case 0x18:
        if (Shift)
            handler->OnkeyDown('O');
        else
            handler->OnkeyDown('o');
        break;
    case 0x19:
        if (Shift)
            handler->OnkeyDown('P');
        else
            handler->OnkeyDown('p');
        break;

    case 0x1E:
        if (Shift)
            handler->OnkeyDown('A');
        else
            handler->OnkeyDown('a');
        break;
    case 0x1F:
        if (Shift)
            handler->OnkeyDown('S');
        else
            handler->OnkeyDown('s');
        break;
    case 0x20:
        if (Shift)
            handler->OnkeyDown('D');
        else
            handler->OnkeyDown('d');
        break;
    case 0x21:
        if (Shift)
            handler->OnkeyDown('F');
        else
            handler->OnkeyDown('f');
        break;
    case 0x22:
        if (Shift)
            handler->OnkeyDown('G');
        else
            handler->OnkeyDown('g');
        break;
    case 0x23:
        if (Shift)
            handler->OnkeyDown('H');
        else
            handler->OnkeyDown('h');
        break;
    case 0x24:
        if (Shift)
            handler->OnkeyDown('J');
        else
            handler->OnkeyDown('j');
        break;
    case 0x25:
        if (Shift)
            handler->OnkeyDown('K');
        else
            handler->OnkeyDown('k');
        break;
    case 0x26:
        if (Shift)
            handler->OnkeyDown('L');
        else
            handler->OnkeyDown('l');
        break;

    case 0x2C:
        if (Shift)
            handler->OnkeyDown('Z');
        else
            handler->OnkeyDown('z');
        break;
    case 0x2D:
        if (Shift)
            handler->OnkeyDown('X');
        else
            handler->OnkeyDown('x');
        break;
    case 0x2E:
        if (Shift)
            handler->OnkeyDown('C');
        else
            handler->OnkeyDown('c');
        break;
    case 0x2F:
        if (Shift)
            handler->OnkeyDown('V');
        else
            handler->OnkeyDown('v');
        break;
    case 0x30:
        if (Shift)
            handler->OnkeyDown('B');
        else
            handler->OnkeyDown('b');
        break;
    case 0x31:
        if (Shift)
            handler->OnkeyDown('N');
        else
            handler->OnkeyDown('n');
        break;
    case 0x32:
        if (Shift)
            handler->OnkeyDown('M');
        else
            handler->OnkeyDown('m');
        break;
    case 0x33:
        if (Shift)
            handler->OnkeyDown('<');
        else
            handler->OnkeyDown(',');
        break;
    case 0x34:
        if (Shift)
            handler->OnkeyDown('>');
        else
            handler->OnkeyDown('.');
        break;
    case 0x35:
        if (Shift)
            handler->OnkeyDown('?');
        else
            handler->OnkeyDown('/');
        break;

    case 0x1C:
        handler->OnkeyDown('\n');
        break;
    case 0x39:
        handler->OnkeyDown(' ');
        break;
    case 0x2A:
    case 0x36:
        Shift = true;
        break;
    case 0xAA:
    case 0xB6:
        Shift = false;
        break;

    case 0x45:
        break;

    default:
        if (key < 0x80)
        {
            printf("KEYBOARD 0x");
            printfHex(key);
        }
        break;
    }
    return esp;
}

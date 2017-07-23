#ifndef LYOS__HARDWARECOMMUNICATION__PORT_H
#define LYOS__HARDWARECOMMUNICATION__PORT_H

#include <common/types.h>
namespace lyos
{
namespace hardwarecommunication
{
class Port
{
protected:
  lyos::common::uint16_t portnumber;
  Port(lyos::common::uint16_t portnumber);
  ~Port();
};

class Port8Bit : public Port
{
public:
  Port8Bit(lyos::common::uint16_t portnumber);
  ~Port8Bit();
  virtual void Write(lyos::common::uint8_t data);
  virtual lyos::common::uint8_t Read();
};

class Port8BitSlow : public Port8Bit
{
public:
  Port8BitSlow(lyos::common::uint16_t portnumber);
  ~Port8BitSlow();
  virtual void Write(lyos::common::uint8_t data);
};

class Port16Bit : public Port
{
public:
  Port16Bit(lyos::common::uint16_t portnumber);
  ~Port16Bit();
  virtual void Write(lyos::common::uint16_t data);
  virtual lyos::common::uint16_t Read();
};

class Port32Bit : public Port
{
public:
  Port32Bit(lyos::common::uint16_t portnumber);
  ~Port32Bit();
  virtual void Write(lyos::common::uint32_t data);
  virtual lyos::common::uint32_t Read();
};
}
}

#endif
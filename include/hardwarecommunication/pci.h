#ifndef __LYOS__HARDWARECOMMUNICATION__PCI_H
#define __LYOS__HARDWARECOMMUNICATION__PCI_H

#include <hardwarecommunication/port.h>
#include <drivers/driver.h>
#include <common/types.h>
#include <hardwarecommunication/interrupts.h>
#include <memorymanagement.h>

namespace lyos
{
namespace hardwarecommunication
{
enum BaseAddressRegisterType
{
  MemoryMapping = 0,
  InputOutput = 1
};
class BaseAddressRegister
{
public:
  bool prefetchable;
  lyos::common::uint8_t *address;
  lyos::common::uint32_t size;
  BaseAddressRegisterType type;
};
class PeripheralComponentInterconectDeviceDescriptor
{
public:
  lyos::common::uint32_t portBase;
  lyos::common::uint32_t interrupt;

  lyos::common::uint16_t bus;
  lyos::common::uint16_t device;
  lyos::common::uint16_t function;

  lyos::common::uint16_t vendor_id;
  lyos::common::uint16_t device_id;

  lyos::common::uint8_t class_id;
  lyos::common::uint8_t subclass_id;
  lyos::common::uint8_t interface_id;
  lyos::common::uint8_t revision;

  PeripheralComponentInterconectDeviceDescriptor();
  ~PeripheralComponentInterconectDeviceDescriptor();
};

class PeripheralComponentInterconectController
{
  Port32Bit dataPort;
  Port32Bit commandPort;

public:
  PeripheralComponentInterconectController();
  ~PeripheralComponentInterconectController();

  lyos::common::uint32_t Read(lyos::common::uint16_t bus,
                              lyos::common::uint16_t device, lyos::common::uint16_t function,
                              lyos::common::uint32_t registeroffset);
  void Write(lyos::common::uint16_t bus, lyos::common::uint16_t device,
             lyos::common::uint16_t function, lyos::common::uint32_t registeroffset,
             lyos::common::uint32_t value);
  bool DeviceHasFunctions(lyos::common::uint16_t bus, lyos::common::uint16_t device);

  void SelectDrivers(lyos::drivers::DriverManager *driverManager,
                     lyos::hardwarecommunication::InterruptManager *interrupts);
  lyos::drivers::Driver *GetDriver(PeripheralComponentInterconectDeviceDescriptor dev,
                                   lyos::hardwarecommunication::InterruptManager *interrupts);
  PeripheralComponentInterconectDeviceDescriptor GetDeviceDescriptor(lyos::common::uint16_t bus,
                                                                     lyos::common::uint16_t device, lyos::common::uint16_t function);
  BaseAddressRegister GetBaseAddressRegister(lyos::common::uint16_t bus,
                                             lyos::common::uint16_t device, lyos::common::uint16_t function,
                                             lyos::common::uint16_t bar);
};
}
}

#endif
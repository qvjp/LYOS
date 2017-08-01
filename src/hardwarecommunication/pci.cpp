#include <hardwarecommunication/pci.h>
using namespace lyos::common;
using namespace lyos::hardwarecommunication;
using namespace lyos::drivers;

PeripheralComponentInterconectDeviceDescriptor::PeripheralComponentInterconectDeviceDescriptor()
{
}
PeripheralComponentInterconectDeviceDescriptor::~PeripheralComponentInterconectDeviceDescriptor()
{
}

PeripheralComponentInterconectController::PeripheralComponentInterconectController()
    : dataPort(0xCFC),
      commandPort(0xCF8)
{
}
PeripheralComponentInterconectController::~PeripheralComponentInterconectController()
{
}

uint32_t PeripheralComponentInterconectController::Read(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset)
{
    uint32_t id =
        0x1 << 31 | ((bus & 0xFF) << 16) | ((device & 0x1F) << 11) | ((function & 0x07) << 8) | (registeroffset & 0xFC);
    commandPort.Write(id);
    uint32_t result = dataPort.Read();
    return result >> (8 * (registeroffset % 4));
}
void PeripheralComponentInterconectController::Write(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset, uint32_t value)
{
    uint32_t id =
        0x1 << 31 | ((bus & 0xFF) << 16) | ((device & 0x1F) << 11) | ((function & 0x07) << 8) | (registeroffset & 0xFC);
    commandPort.Write(id);
    dataPort.Write(value);
}

bool PeripheralComponentInterconectController::DeviceHasFunctions(uint16_t bus, uint16_t device)
{
    return Read(bus, device, 0, 0x0E) & (1 << 7);
}

void printf(char *str);
void printfHex(uint8_t);

void PeripheralComponentInterconectController::SelectDrivers(DriverManager *driverManager, lyos::hardwarecommunication::InterruptManager *interrupts)
{
    for (int bus = 0; bus < 8; bus++)
    {
        for (int device = 0; device < 32; device++)
        {
            int numFunctins = DeviceHasFunctions(bus, device) ? 8 : 1;
            for (int function = 0; function < numFunctins; function++)
            {
                PeripheralComponentInterconectDeviceDescriptor dev = GetDeviceDescriptor(bus, device, function);
                if (dev.vendor_id == 0x0000 || dev.vendor_id == 0xFFFF)
                    continue;

                for (int barNum = 0; barNum < 6; barNum++)
                {
                    BaseAddressRegister bar = GetBaseAddressRegister(bus, device, function, barNum);
                    if (bar.address && (bar.type == InputOutput))
                    {
                        dev.portBase = (uint32_t)bar.address;
                    }
                    Driver *driver = GetDriver(dev, interrupts);
                    if (driver != 0)
                    {
                        driverManager->AddDriver(driver);
                    }
                }

                printf("PCI BUS ");
                printfHex(bus & 0xFF);

                printf(", DEVICE ");
                printfHex(device & 0xFF);

                printf(", FUNCTION ");
                printfHex(function & 0xFF);

                printf(" = VENDOR ");
                printfHex((dev.vendor_id & 0xFF00) >> 8);
                printfHex((dev.vendor_id & 0xFF00));

                printf(", DEVICE  ");
                printfHex((dev.device_id & 0xFF00) >> 8);
                printfHex(dev.device_id & 0xFF);
                printf("\n");
            }
        }
    }
}

Driver *PeripheralComponentInterconectController::GetDriver(PeripheralComponentInterconectDeviceDescriptor dev, InterruptManager *interrupts)
{
    Driver *driver = 0;
    switch (dev.vendor_id)
    {
    case 0x1022:
        switch (dev.device_id)
        {
        case 0x2000:
            // driver = (amd_am79c973 *)MemoryManager::activeMemoryManager->malloc(sizeof(amd_am79c973));
            // if (driver != 0)
            //     new (driver) amd_am79c973(..);
            printf("AMD am79c973");
            break;
        }
        break;
    }
    switch (dev.class_id)
    {
    case 0x03:
        switch (dev.subclass_id)
        {
        case 0x00:
            printf("VGA");
            break;
        }
    }
    return driver;
}

BaseAddressRegister PeripheralComponentInterconectController::GetBaseAddressRegister(uint16_t bus, uint16_t device, uint16_t function, uint16_t bar)
{
    BaseAddressRegister result;

    uint32_t headertype = Read(bus, device, function, 0x0E) & 0x7F;
    int maxBARs = 6 - (4 * headertype);
    if (bar >= maxBARs)
    {
        return result;
    }
    uint32_t bar_value = Read(bus, device, function, 0x10 + 4 * bar);
    result.type = (bar_value & 0x1) ? InputOutput : MemoryMapping;
    uint32_t temp;

    if (result.type == MemoryMapping)
    {
        switch ((bar_value >> 1) & 0x3)
        {
        case 0:
        case 1:
        case 2:
            break;
        }
        result.prefetchable = ((bar_value >> 3) & 0x1) == 0x1;
    }
    else
    {
        result.address = (uint8_t *)(bar_value & ~0x3);
        result.prefetchable = false;
    }

    return result;
}

PeripheralComponentInterconectDeviceDescriptor PeripheralComponentInterconectController::GetDeviceDescriptor(uint16_t bus, uint16_t device, uint16_t function)
{
    PeripheralComponentInterconectDeviceDescriptor result;
    result.bus = bus;
    result.device = device;
    result.function = function;

    result.vendor_id = Read(bus, device, function, 0x00);
    result.device_id = Read(bus, device, function, 0x02);

    result.class_id = Read(bus, device, function, 0x0b);
    result.subclass_id = Read(bus, device, function, 0x0a);
    result.interface_id = Read(bus, device, function, 0x09);
    result.revision = Read(bus, device, function, 0x08);
    result.interrupt = Read(bus, device, function, 0x3c);

    return result;
}
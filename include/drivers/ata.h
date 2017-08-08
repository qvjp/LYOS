#ifndef __LYOS__DRIVERS__ATA_H
#define __LYOS__DRIVERS__ATA_H

#include <hardwarecommunication/port.h>
#include <common/types.h>

namespace lyos
{
namespace drivers
{
class AdvancedTechnologyAttachment
{
  protected:
    hardwarecommunication::Port16Bit dataPort;
    hardwarecommunication::Port8Bit errorPort;
    hardwarecommunication::Port8Bit sectorCountPort;
    hardwarecommunication::Port8Bit lbaLowPort;
    hardwarecommunication::Port8Bit lbaMidPort;
    hardwarecommunication::Port8Bit lbaHiPort;
    hardwarecommunication::Port8Bit devicePort;
    hardwarecommunication::Port8Bit commandPort;
    hardwarecommunication::Port8Bit controlPort;
    bool master;
    // common::uint16_t bytesPerSector;

  public:
    AdvancedTechnologyAttachment(bool master,common::uint16_t protBase);
    ~AdvancedTechnologyAttachment();

    void Identify();
    void Read28(common::uint32_t sectorNum, int count = 512);
    void Write28(common::uint32_t sectorNum, common::uint8_t *data, int count);
    void Flush();
};
}
}

#endif
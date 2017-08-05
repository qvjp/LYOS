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
    hardwarecommunication::Port16Bit errorPort;
    hardwarecommunication::Port16Bit sectorCountPort;
    hardwarecommunication::Port16Bit lbaLowPort;
    hardwarecommunication::Port16Bit lbaMidPort;
    hardwarecommunication::Port16Bit lbaHiPort;
    hardwarecommunication::Port16Bit devicePort;
    hardwarecommunication::Port16Bit commandPort;
    hardwarecommunication::Port16Bit controlPort;
    bool master;
    common::uint16_t bytesPerSector;

  public:
    AdvancedTechnologyAttachment(common::uint16_t protBase, bool master);
    ~AdvancedTechnologyAttachment();

    void Identify();
    void Read28(common::uint32_t sector, common::uint8_t *data, int count);
    void Write28(common::uint32_t sector, common::uint8_t *data, int count);
    void Flush();
};
}
}

#endif
#ifndef __LYOS__GDT_H
#define __LYOS__GDT_H

#include <common/types.h>
namespace lyos
{
class GlobalDescriptorTable
{
public:
  class SegmentDescriptor
  {
  private:
    lyos::common::uint16_t limit_lo;
    lyos::common::uint16_t base_lo;
    lyos::common::uint8_t base_hi;
    lyos::common::uint8_t type;
    lyos::common::uint8_t flags_limit_hi;
    lyos::common::uint8_t base_vhi;

  public:
    SegmentDescriptor(lyos::common::uint32_t base, lyos::common::uint32_t limit, lyos::common::uint8_t type);
    lyos::common::uint32_t Base();
    lyos::common::uint32_t Limit();

  } __attribute__((packed));

private:
  SegmentDescriptor nullSegmentSelector;
  SegmentDescriptor unusedSegmentSelector;
  SegmentDescriptor codeSegmentSelector;
  SegmentDescriptor dataSegmentSelector;

public:
  GlobalDescriptorTable();
  ~GlobalDescriptorTable();

  lyos::common::uint16_t CodeSegmentSelector();
  lyos::common::uint16_t DataSegmentSelector();
};
}

#endif
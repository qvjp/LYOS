#include <fs/msdospart.h>
#include <fs/fat.h>

using namespace lyos;
using namespace lyos::common;
using namespace lyos::drivers;
using namespace lyos::fs;

void printf(char *);
void printfHex(uint8_t);

void MSDOSPartitionTable::ReadPartitions(AdvancedTechnologyAttachment *hd)
{
    MasterBootRecord mbr;
    hd->Read28(0, (uint8_t *)&mbr, sizeof(MasterBootRecord));
    printf("MBR: ");
    /*
    for (int i = 0x1BE; i <= 0x01FF; i++)
    {
        printfHex(((uint8_t *)&mbr)[i]);
        printf(" ");
    }
    printf("\n");
    */
    if (mbr.magicnumber != 0xAA55)
    {
        printf("illegal MBR");
        return;
    }

    for (int i = 0; i < 4; i++)
    {
        if (mbr.parimaryPartition[i].partition_id == 0x00)
        {
            continue;
        }
        printf(" Partition ");
        printfHex(i & 0xFF);
        if (mbr.parimaryPartition[i].bootable == 0x80)
        {
            printf(" bootable. Type");
        }
        else
        {
            printf(" not blltable. Type");
        }

        printfHex(mbr.parimaryPartition[i].partition_id);

        ReadBiosBlock(hd, mbr.parimaryPartition[i].start_lba);
    }
}
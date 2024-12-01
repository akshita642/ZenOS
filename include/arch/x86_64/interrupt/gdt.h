#pragma once

#include "stdint.h"

#define GDT_SIZE 5

/*

GDT Segment Descriptor (8)

----------------------------------------------------
|63-----56|55--52|51----48|47----------40|39-----32|
----------------------------------------------------
|Base     |Flags |Limit   |Access Byte   |Base     |
|31     24|3    0|19    16|7            0|23     16|
----------------------------------------------------
|31---------------------16|15---------------------0|
----------------------------------------------------
|Base                     |Limit                   |
|15                      0|15                     0|
----------------------------------------------------

*/

struct GDT_Entry8 {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t access;
    uint8_t limit_high_and_flags;
    uint8_t base_high;
} __attribute__((packed));

/*
Access Byte

--------------------
|7|6  7|4|3|2 |1 |0|
--------------------
|P|DPL |S|E|DC|RW|A|
--------------------

P: Present bit. Must be set (1) for any valid segment
DPL: Descriptor privilege level field
S: Descriptor type bit. If clear (0) the descriptor defines a system segment (eg. a Task State Segment). If set (1) it defines a code or data segment
E: Executable bit
DC: Direction bit/Conforming bit
RW: Read/Write
A: Accessed bit

Flags

-----------------
|3|2 |1|0       |
-----------------
|G|DB|L|Reserved|
-----------------

G: Granularity flag. if clear, limit is 1 byte blocks. if set, the limit is 4 KiB blocks
DB: size flag. if clear, the descriptior defines 16 bit protected mode segment. if set it defined a 32 bit protected mode segment.
L: Long mode code flag


Special case for system segment (TSS or LDT)

Access byte

---------------
|7|6 5|4|3   1|
---------------
|P|DPL|S|Type |
---------------

Type: type of system segment

Types available in 32-bit protected mode:

0x1: 16-bit TSS (Available)
0x2: LDT
0x3: 16-bit TSS (Busy)
0x9: 32-bit TSS (Available)
0xB: 32-bit TSS (Busy)
Types available in Long Mode:

0x2: LDT
0x9: 64-bit TSS (Available)
0xB: 64-bit TSS (Busy)

*/

#define GDT_ENTRY_ACCESS_PRESENT    0x80
#define GDT_ENTRY_ACCESS_DPL0       0x00
#define GDT_ENTRY_ACCESS_DPL1       0x20
#define GDT_ENTRY_ACCESS_DPL2       0x40
#define GDT_ENTRY_ACCESS_DPL3       0x60
#define GDT_ENTRY_ACCESS_NOT_SYSTEM 0x10
#define GDT_ENTRY_ACCESS_EXECUTABLE 0x08
#define GDT_ENTRY_ACCESS_DIRECTION  0x04
#define GDT_ENTRY_ACCESS_RW         0x02
#define GDT_ENTRY_ACCESS_ACCESSED   0x01

#define GDT_TYPE_CODE   (GDT_ENTRY_ACCESS_EXECUTABLE | GDT_ENTRY_ACCESS_RW)
#define GDT_TYPE_DATA   (GDT_ENTRY_ACCESS_RW)
#define GDT_TYPE_TSS    (GDT_ENTRY_ACCESS_EXECUTABLE | GDT_ENTRY_ACCESS_ACCESSED)

#define GDT_ENTRY_ACCESS_KERNEL_CODE   (GDT_TYPE_CODE | GDT_ENTRY_ACCESS_NOT_SYSTEM | \
                                    GDT_ENTRY_ACCESS_DPL0 | GDT_ENTRY_ACCESS_PRESENT)
#define GDT_ENTRY_ACCESS_KERNEL_DATA   (GDT_TYPE_DATA | GDT_ENTRY_ACCESS_NOT_SYSTEM | \
                                    GDT_ENTRY_ACCESS_DPL0 | GDT_ENTRY_ACCESS_PRESENT)
#define GDT_ENTRY_ACCESS_TSS    (GDT_TYPE_TSS | GDT_ENTRY_ACCESS_DPL0 | GDT_ENTRY_ACCESS_PRESENT)

#define GDT_ENTRY_FLAGS_GRAN        0x80
#define GDT_ENTRY_FLAGS_DB          0x40
#define GDT_ENTRY_FLAGS_LONG_MODE   0x20

#define GDT_ENTRY_FLAGS_KERNEL_CODE (GDT_ENTRY_FLAGS_GRAN | GDT_ENTRY_FLAGS_LONG_MODE)
#define GDT_ENTRY_FLAGS_KERNEL_DATA (GDT_ENTRY_FLAGS_GRAN | GDT_ENTRY_FLAGS_DB)
#define GDT_ENTRY_FLAGS_TSS 0x0

/*

GDT Segment Descriptor (16)

----------------------------------------------------
|127---------------------------------------------96|
----------------------------------------------------
|Reserved                                          |
----------------------------------------------------
|95----------------------------------------------64|
----------------------------------------------------
|Base                                              |
|63                                              32|
----------------------------------------------------
|63-----56|55--52|51----48|47----------40|39-----32|
----------------------------------------------------
|Base     |Flags |Limit   |Access Byte   |Base     |
|31     24|3    0|19    16|7            0|23     16|
----------------------------------------------------
|31---------------------16|15---------------------0|
----------------------------------------------------
|Base                     |Limit                   |
|15                      0|15                     0|
----------------------------------------------------

*/

struct GDT_Entry16 {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t access;
    uint8_t limit_high_and_flags;
    uint8_t base_mid_upper;
    uint32_t base_high;
    uint32_t reserved;
} __attribute__((packed));

struct TSS_Segment {
    uint32_t reserved1;
    uint64_t resp[3];
    uint64_t reserved2;
    uint64_t ist[7];
    uint64_t reserved3;
    uint64_t reserved4;
    uint16_t iomap_base_address;
} __attribute__((packed));

void init_gdt_with_tss();
void set_gdt_entry8(struct GDT_Entry8* entry, uint32_t base_address, uint16_t size, uint8_t flags, uint8_t access);
void set_gdt_entry16(struct GDT_Entry16* entry, uint64_t base_address, uint32_t size, uint8_t access, uint8_t flags);
void init_tss_segment(struct TSS_Segment* tss_segment);

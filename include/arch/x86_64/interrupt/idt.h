#pragma once

#include "stdint.h"

/*

----------------------------------------------------------
|127---------------------------------------------------96|
----------------------------------------------------------
|Reserved                                                |
----------------------------------------------------------
|95----------------------------------------------------64|
----------------------------------------------------------
|63                       48|47|46 |44|43    40|39     32|
----------------------------------------------------------
|Offset                     |P |DPL|0 |Gate type|Reserved|
|31                       16|  |1 0|  |3       0|        |
----------------------------------------------------------
|31-----------------------16|15-------------------------0|
----------------------------------------------------------
|Segment Selector           |Offset                      |
|15                        0|15                         0|
----------------------------------------------------------

*/

struct IDT_Entry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t type_attr;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t reserved;
} __attribute__((packed));

/*

Offset: Address of the entry point of the ISR
Selector: Segment Selector
IST: offet into the IST
Gate Type: A 4-bit value which defines the type of gate this Interrupt Descriptor represents. 
            In long mode there are two valid type values:
            
            0b1110 or 0xE: 64-bit Interrupt Gate
            0b1111 or 0xF: 64-bit Trap Gate

DPL: Privilege Level
Present: Present bit

*/

#define IDT_TYPE_INTERRUPT  0x0E
#define IDT_TYPE_TRAP       0x0F
#define IDT_ENTRY_DPL0      0x00
#define IDT_ENTRY_DPL1      0x20
#define IDT_ENTRY_DPL2      0x40
#define IDT_ENTRY_DPL3      0x60
#define IDT_ENTRY_PRESENT   0x80

#define IDT_ENTRY_KERNEL    (IDT_ENTRY_DPL0 | IDT_ENTRY_PRESENT)

struct IDT_Ptr {
    uint16_t size;
    uint64_t offset;
} __attribute__((packed));

void init_idt();
void set_idt_entry(struct IDT_Entry* entry, uint64_t handler, uint16_t selector, uint8_t ist, uint8_t flags, uint8_t gate_type);

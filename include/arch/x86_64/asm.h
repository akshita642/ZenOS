#pragma once

#include "stdint.h"

extern void cli();
extern void sti();
extern void lgdt();
extern void ltr();
extern void lidt(uint64_t idtp);

#pragma once

#include "idt.h"

void page_fault_handler(struct InterruptStackFrame *frame);

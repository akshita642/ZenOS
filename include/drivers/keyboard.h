#pragma once

__attribute__((interrupt))
void irq_keyboard_handler(struct InterruptStackFrame* frame);

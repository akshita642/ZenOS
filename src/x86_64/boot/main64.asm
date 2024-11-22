global long_mode_start

extern kernel_main
extern gdt64.Data
extern mboot_saved_info.mboot_ebx

section .text
bits 64
long_mode_start:
    mov ax, gdt64.Data
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov edi, dword [mboot_saved_info.mboot_ebx]

    call kernel_main

    hlt

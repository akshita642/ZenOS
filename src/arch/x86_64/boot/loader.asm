global loader

extern kmain

bits 64
long_mode_start:
    mov ax, gdt64.Data
    mov ds, rax
    mov es, rax
    mov fs, rax
    mov gs, rax
    mov ss, ax

    call kmain

.halt:
    hlt
    jmp .halt

extern lgdt
lgdt:
    lgdt [gdt64.Pointer]
    ret

extern ltr
ltr:
    mov di, gdt64.Tss
    ltr di
    ret

extern lidt
lidt:
    lidt [rdi]
    ret

extern cli
cli:
    cli
    ret

extern sti
sti:
    sti
    ret

section .text
bits 32
loader:
    mov esp, kernel_stack + KERNEL_STACK_SIZE

    call setup_page_tables

    lgdt [gdt64.Pointer]
    jmp gdt64.Code:long_mode_start

    hlt

setup_page_tables:
    mov eax, page_table_l3
    or eax, 0b11 ; present, writable
    mov [page_table_l4], eax

    mov eax, page_table_l2
    or eax, 0b11 ; present, writable
    mov [page_table_l3], eax

    mov ecx, 0 ; counter

.loop:
    mov eax, 0x200000
    mul ecx
    or eax, 0b10000011 ; present, writable, huge page
    mov [page_table_l2 + ecx * 8], eax

    inc ecx
    cmp ecx, 512
    jne .loop

.enable_paging:
    ; pass page table location to cpu
    mov eax, page_table_l4
    mov cr3, eax

    ; enable Physical Address Extension (PAE)
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; enable long mode
    mov ecx, 0xC0000080
    rdmsr ; read model-specific register
    or eax, 1 << 8
    wrmsr ; write model-specific register

    ; enable paging
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    ret

section .bss
align 4096
page_table_l4:
    resb 4096
page_table_l3:
    resb 4096
page_table_l2:
    resb 4096

section .bss
extern tss_segment
tss_segment:
    resd 1 ; reserved 
    resq 3 ; resp
    resq 2 ; reserved
    resq 7 ; ist
    resq 2 ; reserved
    resw 1 ; I/O map base address

PRESENT        equ 1 << 7
NOT_SYS        equ 1 << 4
EXEC           equ 1 << 3
DC             equ 1 << 2
RW             equ 1 << 1
ACCESSED       equ 1 << 0

; Flags bits
GRAN_4K       equ 1 << 7
SZ_32         equ 1 << 6
LONG_MODE     equ 1 << 5

section .rodata
align 8
extern gdt64
gdt64:
    .Null: equ $ - gdt64
        dq 0
    .Code: equ $ - gdt64
        dd 0xFFFF                                   ; Limit & Base (low, bits 0-15)
        db 0                                        ; Base (mid, bits 16-23)
        db PRESENT | NOT_SYS | EXEC | RW            ; Access
        db GRAN_4K | LONG_MODE | 0xF                ; Flags & Limit (high, bits 16-19)
        db 0                                        ; Base (high, bits 24-31)
    .Data: equ $ - gdt64
        dd 0xFFFF                                   ; Limit & Base (low, bits 0-15)
        db 0                                        ; Base (mid, bits 16-23)
        db PRESENT | NOT_SYS | RW                   ; Access
        db GRAN_4K | SZ_32 | 0xF                    ; Flags & Limit (high, bits 16-19)
        db 0                                        ; Base (high, bits 24-31)
    .Tss: equ $ - gdt64
        dq 0
        dq 0
    .Pointer:
        dw $ - gdt64 - 1
        dq gdt64

KERNEL_STACK_SIZE equ 4096

align 8
section .bss
kernel_stack:
    resb KERNEL_STACK_SIZE

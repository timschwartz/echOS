.global gdt_flush

gdt_flush:
    # Load new GDT from rdi
    movq %rdi, %rax
    lgdt (%rax)

    # Update segment registers
    movq $0x10, %rax   # Kernel data segment selector (0x10)
    movq %rax, %ds
    movq %rax, %es
    movq %rax, %ss

    # Update the code segment
    movq $0x08, %rax   # Kernel code segment selector (0x08)
    pushq %rax         # Push code segment selector
    leaq retf_label(%rip), %rax
    pushq %rax         # Push return address
    lretq              # Long jump to the new code segment

retf_label:
    ret

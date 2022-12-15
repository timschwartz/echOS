.code64
.global gdt_flush

flush:
  ret

gdt_flush:
  lgdt (%rdi)
  mov $0x10, %ax
  mov %ax, %ds
  mov %ax, %es
  mov %ax, %ss
  jmp flush

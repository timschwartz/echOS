.global gdt_flush
.code64

flush:
  ret

gdt_flush:
  jmp flush

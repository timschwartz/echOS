.global _start

.extern __libc_main

_start:
	call __libc_main

wait:
	hlt
	jmp wait

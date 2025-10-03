/* Does not clear BSS */

.text
.global _start

_start:
	li x2, 4 * (1 << SRAM_ADDR_WIDTH)
	call main

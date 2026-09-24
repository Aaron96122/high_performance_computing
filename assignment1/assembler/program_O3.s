	.build_version macos, 26, 0	sdk_version 26, 5
	.section	__TEXT,__text,regular,pure_instructions
	.globl	_main                           ; -- Begin function main
	.p2align	2
_main:                                  ; @main
	.cfi_startproc
; %bb.0:
	sub	sp, sp, #64
	stp	x29, x30, [sp, #48]             ; 16-byte Folded Spill
	add	x29, sp, #48
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	sub	x0, x29, #16
	mov	w1, #1                          ; =0x1
	bl	_timespec_get
	mov	x8, #39680                      ; =0x9b00
	movk	x8, #46564, lsl #16
	movk	x8, #23385, lsl #32
	movk	x8, #1776, lsl #48
	str	x8, [sp]
Lloh0:
	adrp	x0, l_.str@PAGE
Lloh1:
	add	x0, x0, l_.str@PAGEOFF
	bl	_printf
	add	x0, sp, #16
	mov	w1, #1                          ; =0x1
	bl	_timespec_get
	ldr	x0, [sp, #16]
	ldur	x1, [x29, #-16]
	bl	_difftime
	ldr	x8, [sp, #24]
	ldur	x9, [x29, #-8]
	sub	x8, x8, x9
	scvtf	d1, x8
	mov	x8, #70368744177664             ; =0x400000000000
	movk	x8, #16527, lsl #48
	fmov	d2, x8
	fdiv	d1, d1, d2
	mov	x8, #145685290680320            ; =0x848000000000
	movk	x8, #16686, lsl #48
	fmov	d2, x8
	fmadd	d0, d0, d2, d1
	fmov	d1, #5.00000000
	fdiv	d0, d0, d1
	str	d0, [sp]
Lloh2:
	adrp	x0, l_.str.1@PAGE
Lloh3:
	add	x0, x0, l_.str.1@PAGEOFF
	bl	_printf
	mov	w0, #0                          ; =0x0
	ldp	x29, x30, [sp, #48]             ; 16-byte Folded Reload
	add	sp, sp, #64
	ret
	.loh AdrpAdd	Lloh2, Lloh3
	.loh AdrpAdd	Lloh0, Lloh1
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__cstring,cstring_literals
l_.str:                                 ; @.str
	.asciz	"The result is %ld\n"

l_.str.1:                               ; @.str.1
	.asciz	"benchmark time for one iteration: %f\n"

.subsections_via_symbols

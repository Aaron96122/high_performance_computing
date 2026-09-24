	.build_version macos, 26, 0	sdk_version 26, 5
	.section	__TEXT,__text,regular,pure_instructions
	.globl	_main                           ; -- Begin function main
	.p2align	2
_main:                                  ; @main
	.cfi_startproc
; %bb.0:
	sub	sp, sp, #112
	stp	d9, d8, [sp, #48]               ; 16-byte Folded Spill
	stp	x22, x21, [sp, #64]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #80]             ; 16-byte Folded Spill
	stp	x29, x30, [sp, #96]             ; 16-byte Folded Spill
	add	x29, sp, #96
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	.cfi_offset w21, -40
	.cfi_offset w22, -48
	.cfi_offset b8, -56
	.cfi_offset b9, -64
	mov	x20, x1
	mov	x19, x0
	add	x0, sp, #32
	mov	w1, #1                          ; =0x1
	bl	_timespec_get
	mov	w8, #5                          ; =0x5
	mov	w9, #51712                      ; =0xca00
	movk	w9, #15258, lsl #16
                                        ; fake_use: $w9
LBB0_1:                                 ; =>This Inner Loop Header: Depth=1
	subs	w8, w8, #1
	b.ne	LBB0_1
; %bb.2:
	mov	w21, #5                         ; =0x5
                                        ; fake_use: $w21
	mov	x22, #39680                     ; =0x9b00
	movk	x22, #46564, lsl #16
	movk	x22, #23385, lsl #32
	movk	x22, #1776, lsl #48
	str	x22, [sp]
Lloh0:
	adrp	x0, l_.str@PAGE
Lloh1:
	add	x0, x0, l_.str@PAGEOFF
	bl	_printf
	add	x0, sp, #16
	mov	w1, #1                          ; =0x1
	bl	_timespec_get
	ldr	x0, [sp, #16]
	ldr	x1, [sp, #32]
	bl	_difftime
	ldr	x8, [sp, #24]
	ldr	x9, [sp, #40]
	sub	x8, x8, x9
	scvtf	d1, x8
	mov	x8, #70368744177664             ; =0x400000000000
	movk	x8, #16527, lsl #48
	fmov	d2, x8
	fdiv	d1, d1, d2
	mov	x8, #145685290680320            ; =0x848000000000
	movk	x8, #16686, lsl #48
	fmov	d2, x8
	fmadd	d8, d0, d2, d1
	fmov	d0, #5.00000000
	fdiv	d0, d8, d0
	str	d0, [sp]
Lloh2:
	adrp	x0, l_.str.1@PAGE
Lloh3:
	add	x0, x0, l_.str.1@PAGEOFF
	bl	_printf
                                        ; fake_use: $d8
                                        ; fake_use: $x22
	ldp	x8, x9, [sp, #16]
                                        ; fake_use: $x8
                                        ; fake_use: $x9
	ldp	x8, x9, [sp, #32]
                                        ; fake_use: $x8
                                        ; fake_use: $x9
                                        ; fake_use: $w21
	mov	w8, #51712                      ; =0xca00
	movk	w8, #15258, lsl #16
                                        ; fake_use: $x8
                                        ; fake_use: $x20
                                        ; fake_use: $w19
	mov	w0, #0                          ; =0x0
	ldp	x29, x30, [sp, #96]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #80]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #64]             ; 16-byte Folded Reload
	ldp	d9, d8, [sp, #48]               ; 16-byte Folded Reload
	add	sp, sp, #112
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

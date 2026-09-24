	.build_version macos, 26, 0	sdk_version 26, 5
	.section	__TEXT,__text,regular,pure_instructions
	.globl	_main                           ; -- Begin function main
	.p2align	2
_main:                                  ; @main
	.cfi_startproc
; %bb.0:
	sub	sp, sp, #112
	stp	x29, x30, [sp, #96]             ; 16-byte Folded Spill
	add	x29, sp, #96
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	stur	wzr, [x29, #-4]
	stur	w0, [x29, #-8]
	stur	x1, [x29, #-16]
	mov	x8, #51712                      ; =0xca00
	movk	x8, #15258, lsl #16
	stur	x8, [x29, #-24]
	mov	w8, #5                          ; =0x5
	stur	w8, [x29, #-28]
	str	xzr, [sp, #24]
	add	x0, sp, #48
	mov	w1, #1                          ; =0x1
	bl	_timespec_get
	str	wzr, [sp, #12]
	b	LBB0_1
LBB0_1:                                 ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB0_3 Depth 2
	ldr	w8, [sp, #12]
	subs	w8, w8, #5
	b.ge	LBB0_8
	b	LBB0_2
LBB0_2:                                 ;   in Loop: Header=BB0_1 Depth=1
	str	xzr, [sp, #24]
	str	wzr, [sp, #8]
	b	LBB0_3
LBB0_3:                                 ;   Parent Loop BB0_1 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ldrsw	x8, [sp, #8]
	mov	x9, #51712                      ; =0xca00
	movk	x9, #15258, lsl #16
	subs	x8, x8, x9
	b.ge	LBB0_6
	b	LBB0_4
LBB0_4:                                 ;   in Loop: Header=BB0_3 Depth=2
	ldrsw	x9, [sp, #8]
	ldr	x8, [sp, #24]
	add	x8, x8, x9
	str	x8, [sp, #24]
	b	LBB0_5
LBB0_5:                                 ;   in Loop: Header=BB0_3 Depth=2
	ldr	w8, [sp, #8]
	add	w8, w8, #1
	str	w8, [sp, #8]
	b	LBB0_3
LBB0_6:                                 ;   in Loop: Header=BB0_1 Depth=1
	b	LBB0_7
LBB0_7:                                 ;   in Loop: Header=BB0_1 Depth=1
	ldr	w8, [sp, #12]
	add	w8, w8, #1
	str	w8, [sp, #12]
	b	LBB0_1
LBB0_8:
	ldr	x8, [sp, #24]
	mov	x9, sp
	str	x8, [x9]
	adrp	x0, l_.str@PAGE
	add	x0, x0, l_.str@PAGEOFF
	bl	_printf
	add	x0, sp, #32
	mov	w1, #1                          ; =0x1
	bl	_timespec_get
	ldr	x0, [sp, #32]
	ldr	x1, [sp, #48]
	bl	_difftime
	ldr	x8, [sp, #40]
	ldr	x9, [sp, #56]
	subs	x8, x8, x9
	scvtf	d1, x8
	mov	x8, #70368744177664             ; =0x400000000000
	movk	x8, #16527, lsl #48
	fmov	d2, x8
	fdiv	d2, d1, d2
	mov	x8, #145685290680320            ; =0x848000000000
	movk	x8, #16686, lsl #48
	fmov	d1, x8
	fmadd	d0, d0, d1, d2
	str	d0, [sp, #16]
	ldr	d0, [sp, #16]
	fmov	d1, #5.00000000
	fdiv	d0, d0, d1
	mov	x8, sp
	str	d0, [x8]
	adrp	x0, l_.str.1@PAGE
	add	x0, x0, l_.str.1@PAGEOFF
	bl	_printf
	mov	w0, #0                          ; =0x0
	ldp	x29, x30, [sp, #96]             ; 16-byte Folded Reload
	add	sp, sp, #112
	ret
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__cstring,cstring_literals
l_.str:                                 ; @.str
	.asciz	"The result is %ld\n"

l_.str.1:                               ; @.str.1
	.asciz	"benchmark time for one iteration: %f\n"

.subsections_via_symbols

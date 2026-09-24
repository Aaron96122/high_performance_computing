	.file	"naive.c"
# GNU C23 (GCC) version 16.1.1 20260515 (Red Hat 16.1.1-2) (x86_64-redhat-linux)
#	compiled by GNU C version 16.1.1 20260515 (Red Hat 16.1.1-2), GMP version 6.3.0, MPFR version 4.2.2, MPC version 1.4.1, isl version none
# GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
# options passed: -mtune=generic -march=x86-64 -mtls-dialect=gnu2 -O1
	.text
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"The result is %ld\n"
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 8
.LC3:
	.string	"benchmark time for one iteration: %f\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB22:
	.cfi_startproc
	subq	$40, %rsp	#,
	.cfi_def_cfa_offset 48
# naive.c:14: 	timespec_get(&bench_start_time, TIME_UTC);
	leaq	16(%rsp), %rdi	#, tmp114
	movl	$1, %esi	#,
	call	timespec_get	#
	movl	$5, %edx	#, ivtmp_26
.L2:
# naive.c:7: {
	movl	$1000000000, %eax	#, ivtmp_1
	.p2align 3
.L3:
# naive.c:18: 		for (int ix=0; ix<N; ix++)
	subl	$1, %eax	#, ivtmp_1
	jne	.L3	#,
# naive.c:15: 	for (int bx=0; bx<bench_iter; bx++)
	subl	$1, %edx	#, ivtmp_26
	jne	.L2	#,
# naive.c:23: 	printf("The result is %ld\n", num);
	movabsq	$499999999500000000, %rsi	#,
	movl	$.LC0, %edi	#,
	movl	$0, %eax	#,
	call	printf	#
# naive.c:24: 	timespec_get(&bench_stop_time, TIME_UTC);
	movq	%rsp, %rdi	#, tmp116
	movl	$1, %esi	#,
	call	timespec_get	#
# naive.c:26: 	       difftime(bench_stop_time.tv_sec, bench_start_time.tv_sec) * 1000000 + (bench_stop_time.tv_nsec - bench_start_time.tv_nsec) / 1000;
	movq	16(%rsp), %rsi	# bench_start_time.tv_sec,
	movq	(%rsp), %rdi	# bench_stop_time.tv_sec,
	call	difftime	#
# naive.c:26: 	       difftime(bench_stop_time.tv_sec, bench_start_time.tv_sec) * 1000000 + (bench_stop_time.tv_nsec - bench_start_time.tv_nsec) / 1000;
	movq	8(%rsp), %rcx	# bench_stop_time.tv_nsec, bench_stop_time.tv_nsec
	subq	24(%rsp), %rcx	# bench_start_time.tv_nsec, _8
# naive.c:26: 	       difftime(bench_stop_time.tv_sec, bench_start_time.tv_sec) * 1000000 + (bench_stop_time.tv_nsec - bench_start_time.tv_nsec) / 1000;
	movabsq	$2361183241434822607, %rdx	#, tmp123
	movq	%rcx, %rax	# _8, tmp134
	imulq	%rdx	# tmp123
	sarq	$7, %rdx	#, tmp124
	sarq	$63, %rcx	#, tmp125
	subq	%rcx, %rdx	# tmp125, _9
# naive.c:26: 	       difftime(bench_stop_time.tv_sec, bench_start_time.tv_sec) * 1000000 + (bench_stop_time.tv_nsec - bench_start_time.tv_nsec) / 1000;
	pxor	%xmm1, %xmm1	# _10
	cvtsi2sdq	%rdx, %xmm1	# _9, _10
# naive.c:26: 	       difftime(bench_stop_time.tv_sec, bench_start_time.tv_sec) * 1000000 + (bench_stop_time.tv_nsec - bench_start_time.tv_nsec) / 1000;
	mulsd	.LC1(%rip), %xmm0	#, _5
# naive.c:25: 	bench_diff_time =
	addsd	%xmm1, %xmm0	# _10, bench_diff_time_21
# naive.c:27: 	printf("benchmark time for one iteration: %f\n", bench_diff_time/bench_iter);
	divsd	.LC2(%rip), %xmm0	#, _11
	movl	$.LC3, %edi	#,
	movl	$1, %eax	#,
	call	printf	#
# naive.c:31: }
	movl	$0, %eax	#,
	addq	$40, %rsp	#,
	.cfi_def_cfa_offset 8
	ret	
	.cfi_endproc
.LFE22:
	.size	main, .-main
	.section	.rodata.cst8,"aM",@progbits,8
	.align 8
.LC1:
	.long	0
	.long	1093567616
	.align 8
.LC2:
	.long	0
	.long	1075052544
	.ident	"GCC: (GNU) 16.1.1 20260515 (Red Hat 16.1.1-2)"
	.section	.note.GNU-stack,"",@progbits

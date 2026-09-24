	.file	"naive.c"
# GNU C23 (GCC) version 16.1.1 20260515 (Red Hat 16.1.1-2) (x86_64-redhat-linux)
#	compiled by GNU C version 16.1.1 20260515 (Red Hat 16.1.1-2), GMP version 6.3.0, MPFR version 4.2.2, MPC version 1.4.1, isl version none
# GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
# options passed: -mtune=generic -march=x86-64 -mtls-dialect=gnu2 -O0
	.text
	.section	.rodata
.LC0:
	.string	"The result is %ld\n"
	.align 8
.LC2:
	.string	"benchmark time for one iteration: %f\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB6:
	.cfi_startproc
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	subq	$96, %rsp	#,
	movl	%edi, -84(%rbp)	# argc, argc
	movq	%rsi, -96(%rbp)	# argv, argv
# naive.c:8: 	const long N=1000000000;
	movq	$1000000000, -24(%rbp)	#, N
# naive.c:9: 	const int bench_iter=5;
	movl	$5, -28(%rbp)	#, bench_iter
# naive.c:12: 	long num=0;
	movq	$0, -8(%rbp)	#, num
# naive.c:14: 	timespec_get(&bench_start_time, TIME_UTC);
	leaq	-64(%rbp), %rax	#, tmp113
	movl	$1, %esi	#,
	movq	%rax, %rdi	# tmp113,
	call	timespec_get	#
# naive.c:15: 	for (int bx=0; bx<bench_iter; bx++)
	movl	$0, -12(%rbp)	#, bx
# naive.c:15: 	for (int bx=0; bx<bench_iter; bx++)
	jmp	.L2	#
.L5:
# naive.c:17: 		num = 0;
	movq	$0, -8(%rbp)	#, num
# naive.c:18: 		for (int ix=0; ix<N; ix++)
	movl	$0, -16(%rbp)	#, ix
# naive.c:18: 		for (int ix=0; ix<N; ix++)
	jmp	.L3	#
.L4:
# naive.c:20: 			num += ix;
	movl	-16(%rbp), %eax	# ix, tmp114
	cltq
	addq	%rax, -8(%rbp)	# _1, num
# naive.c:18: 		for (int ix=0; ix<N; ix++)
	addl	$1, -16(%rbp)	#, ix
.L3:
# naive.c:18: 		for (int ix=0; ix<N; ix++)
	movl	-16(%rbp), %eax	# ix, tmp115
	cltq
	cmpq	%rax, -24(%rbp)	# _2, N
	jg	.L4	#,
# naive.c:15: 	for (int bx=0; bx<bench_iter; bx++)
	addl	$1, -12(%rbp)	#, bx
.L2:
# naive.c:15: 	for (int bx=0; bx<bench_iter; bx++)
	movl	-12(%rbp), %eax	# bx, tmp116
	cmpl	-28(%rbp), %eax	# bench_iter, tmp116
	jl	.L5	#,
# naive.c:23: 	printf("The result is %ld\n", num);
	movq	-8(%rbp), %rax	# num, tmp117
	movq	%rax, %rsi	# tmp117,
	movl	$.LC0, %edi	#,
	movl	$0, %eax	#,
	call	printf	#
# naive.c:24: 	timespec_get(&bench_stop_time, TIME_UTC);
	leaq	-80(%rbp), %rax	#, tmp118
	movl	$1, %esi	#,
	movq	%rax, %rdi	# tmp118,
	call	timespec_get	#
# naive.c:26: 	       difftime(bench_stop_time.tv_sec, bench_start_time.tv_sec) * 1000000 + (bench_stop_time.tv_nsec - bench_start_time.tv_nsec) / 1000;
	movq	-64(%rbp), %rdx	# bench_start_time.tv_sec, _3
# naive.c:26: 	       difftime(bench_stop_time.tv_sec, bench_start_time.tv_sec) * 1000000 + (bench_stop_time.tv_nsec - bench_start_time.tv_nsec) / 1000;
	movq	-80(%rbp), %rax	# bench_stop_time.tv_sec, _4
# naive.c:26: 	       difftime(bench_stop_time.tv_sec, bench_start_time.tv_sec) * 1000000 + (bench_stop_time.tv_nsec - bench_start_time.tv_nsec) / 1000;
	movq	%rdx, %rsi	# _3,
	movq	%rax, %rdi	# _4,
	call	difftime	#
# naive.c:26: 	       difftime(bench_stop_time.tv_sec, bench_start_time.tv_sec) * 1000000 + (bench_stop_time.tv_nsec - bench_start_time.tv_nsec) / 1000;
	movsd	.LC1(%rip), %xmm1	#, tmp119
	mulsd	%xmm0, %xmm1	# _5, _6
# naive.c:26: 	       difftime(bench_stop_time.tv_sec, bench_start_time.tv_sec) * 1000000 + (bench_stop_time.tv_nsec - bench_start_time.tv_nsec) / 1000;
	movq	-72(%rbp), %rdx	# bench_stop_time.tv_nsec, _7
# naive.c:26: 	       difftime(bench_stop_time.tv_sec, bench_start_time.tv_sec) * 1000000 + (bench_stop_time.tv_nsec - bench_start_time.tv_nsec) / 1000;
	movq	-56(%rbp), %rax	# bench_start_time.tv_nsec, _8
# naive.c:26: 	       difftime(bench_stop_time.tv_sec, bench_start_time.tv_sec) * 1000000 + (bench_stop_time.tv_nsec - bench_start_time.tv_nsec) / 1000;
	movq	%rdx, %rcx	# _7, _7
	subq	%rax, %rcx	# _8, _7
# naive.c:26: 	       difftime(bench_stop_time.tv_sec, bench_start_time.tv_sec) * 1000000 + (bench_stop_time.tv_nsec - bench_start_time.tv_nsec) / 1000;
	movabsq	$2361183241434822607, %rdx	#, tmp121
	movq	%rcx, %rax	# _9, tmp127
	imulq	%rdx	# tmp121
	sarq	$7, %rdx	#, tmp122
	movq	%rcx, %rax	# _9, _9
	sarq	$63, %rax	#, _9
	subq	%rax, %rdx	# tmp123, _10
# naive.c:26: 	       difftime(bench_stop_time.tv_sec, bench_start_time.tv_sec) * 1000000 + (bench_stop_time.tv_nsec - bench_start_time.tv_nsec) / 1000;
	pxor	%xmm0, %xmm0	# _11
	cvtsi2sdq	%rdx, %xmm0	# _10, _11
# naive.c:25: 	bench_diff_time =
	addsd	%xmm1, %xmm0	# _6, bench_diff_time_27
	movsd	%xmm0, -40(%rbp)	# bench_diff_time_27, bench_diff_time
# naive.c:27: 	printf("benchmark time for one iteration: %f\n", bench_diff_time/bench_iter);
	pxor	%xmm1, %xmm1	# _12
	cvtsi2sdl	-28(%rbp), %xmm1	# bench_iter, _12
	movsd	-40(%rbp), %xmm0	# bench_diff_time, tmp125
	divsd	%xmm1, %xmm0	# _12, tmp125
	movq	%xmm0, %rax	# tmp125, _13
	movq	%rax, %xmm0	# _13,
	movl	$.LC2, %edi	#,
	movl	$1, %eax	#,
	call	printf	#
# naive.c:28: 	return 0;	
	movl	$0, %eax	#, _29
# naive.c:31: }
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE6:
	.size	main, .-main
	.section	.rodata
	.align 8
.LC1:
	.long	0
	.long	1093567616
	.ident	"GCC: (GNU) 16.1.1 20260515 (Red Hat 16.1.1-2)"
	.section	.note.GNU-stack,"",@progbits

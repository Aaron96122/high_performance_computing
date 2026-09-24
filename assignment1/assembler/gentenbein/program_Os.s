	.file	"naive.c"
# GNU C23 (GCC) version 16.1.1 20260515 (Red Hat 16.1.1-2) (x86_64-redhat-linux)
#	compiled by GNU C version 16.1.1 20260515 (Red Hat 16.1.1-2), GMP version 6.3.0, MPFR version 4.2.2, MPC version 1.4.1, isl version none
# GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
# options passed: -mtune=generic -march=x86-64 -mtls-dialect=gnu2 -Os
	.text
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"The result is %ld\n"
.LC3:
	.string	"benchmark time for one iteration: %f\n"
	.section	.text.startup,"ax",@progbits
	.globl	main
	.type	main, @function
main:
.LFB6:
	.cfi_startproc
	subq	$40, %rsp	#,
	.cfi_def_cfa_offset 48
# naive.c:14: 	timespec_get(&bench_start_time, TIME_UTC);
	movl	$1, %esi	#,
	movq	%rsp, %rdi	#, tmp112
	call	timespec_get	#
# naive.c:23: 	printf("The result is %ld\n", num);
	movl	$.LC0, %edi	#,
	xorl	%eax, %eax	#
	movabsq	$499999999500000000, %rsi	#,
	call	printf	#
# naive.c:24: 	timespec_get(&bench_stop_time, TIME_UTC);
	leaq	16(%rsp), %rdi	#, tmp114
	movl	$1, %esi	#,
	call	timespec_get	#
# naive.c:26: 	       difftime(bench_stop_time.tv_sec, bench_start_time.tv_sec) * 1000000 + (bench_stop_time.tv_nsec - bench_start_time.tv_nsec) / 1000;
	movq	(%rsp), %rsi	# bench_start_time.tv_sec,
	movq	16(%rsp), %rdi	# bench_stop_time.tv_sec,
	call	difftime	#
# naive.c:26: 	       difftime(bench_stop_time.tv_sec, bench_start_time.tv_sec) * 1000000 + (bench_stop_time.tv_nsec - bench_start_time.tv_nsec) / 1000;
	movq	24(%rsp), %rax	# bench_stop_time.tv_nsec, bench_stop_time.tv_nsec
	subq	8(%rsp), %rax	# bench_start_time.tv_nsec, _8
# naive.c:26: 	       difftime(bench_stop_time.tv_sec, bench_start_time.tv_sec) * 1000000 + (bench_stop_time.tv_nsec - bench_start_time.tv_nsec) / 1000;
	movl	$1000, %ecx	#, tmp122
# naive.c:26: 	       difftime(bench_stop_time.tv_sec, bench_start_time.tv_sec) * 1000000 + (bench_stop_time.tv_nsec - bench_start_time.tv_nsec) / 1000;
	mulsd	.LC1(%rip), %xmm0	#, _5
# naive.c:26: 	       difftime(bench_stop_time.tv_sec, bench_start_time.tv_sec) * 1000000 + (bench_stop_time.tv_nsec - bench_start_time.tv_nsec) / 1000;
	cqto
# naive.c:27: 	printf("benchmark time for one iteration: %f\n", bench_diff_time/bench_iter);
	movl	$.LC3, %edi	#,
# naive.c:26: 	       difftime(bench_stop_time.tv_sec, bench_start_time.tv_sec) * 1000000 + (bench_stop_time.tv_nsec - bench_start_time.tv_nsec) / 1000;
	idivq	%rcx	# tmp122
# naive.c:26: 	       difftime(bench_stop_time.tv_sec, bench_start_time.tv_sec) * 1000000 + (bench_stop_time.tv_nsec - bench_start_time.tv_nsec) / 1000;
	cvtsi2sdq	%rax, %xmm1	# _9, _10
# naive.c:27: 	printf("benchmark time for one iteration: %f\n", bench_diff_time/bench_iter);
	movb	$1, %al	#,
# naive.c:25: 	bench_diff_time =
	addsd	%xmm1, %xmm0	# _10, bench_diff_time_21
# naive.c:27: 	printf("benchmark time for one iteration: %f\n", bench_diff_time/bench_iter);
	divsd	.LC2(%rip), %xmm0	#, _11
	call	printf	#
# naive.c:31: }
	xorl	%eax, %eax	#
	addq	$40, %rsp	#,
	.cfi_def_cfa_offset 8
	ret	
	.cfi_endproc
.LFE6:
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

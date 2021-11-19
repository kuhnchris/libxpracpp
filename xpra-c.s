	.file	"xpra-c.cpp"
	.text
	.section	.rodata
.LC0:
	.string	"starting with zlib: %s\n"
.LC1:
	.string	"socket went fucky."
.LC2:
	.string	"127.0.0.1"
.LC3:
	.string	"connect failed"
.LC4:
	.string	"Compression failed: %s\n"
.LC5:
	.string	"sent payload ( %d )...\n"
.LC6:
	.string	"received: ( %lu ) %s"
	.text
	.globl	main
	.type	main, @function
main:
.LFB6:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$2112, %rsp
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	call	zlibVersion@PLT
	movq	%rax, %rsi
	leaq	.LC0(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$0, %edx
	movl	$1, %esi
	movl	$2, %edi
	call	socket@PLT
	movl	%eax, -2112(%rbp)
	cmpl	$0, -2112(%rbp)
	jns	.L2
	leaq	.LC1(%rip), %rax
	movq	%rax, %rdi
	call	puts@PLT
	movl	$-1, %eax
	jmp	.L6
.L2:
	leaq	.LC2(%rip), %rax
	movq	%rax, %rdi
	call	inet_addr@PLT
	movl	%eax, -2092(%rbp)
	movl	$10000, %edi
	call	htons@PLT
	movw	%ax, -2094(%rbp)
	movw	$2, -2096(%rbp)
	leaq	-2096(%rbp), %rcx
	movl	-2112(%rbp), %eax
	movl	$16, %edx
	movq	%rcx, %rsi
	movl	%eax, %edi
	call	connect@PLT
	shrl	$31, %eax
	testb	%al, %al
	je	.L4
	leaq	.LC3(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$-2, %eax
	jmp	.L6
.L4:
	movq	$0, -2064(%rbp)
	movq	$0, -2056(%rbp)
	leaq	-2048(%rbp), %rdx
	movl	$0, %eax
	movl	$126, %ecx
	movq	%rdx, %rdi
	rep stosq
	movb	$80, -2064(%rbp)
	movb	$1, -2063(%rbp)
	movb	$1, -2062(%rbp)
	movb	$0, -2061(%rbp)
	movb	$0, -2060(%rbp)
	movb	$0, -2059(%rbp)
	movb	$0, -2058(%rbp)
	movb	$8, -2057(%rbp)
	movq	$0, -1040(%rbp)
	movq	$0, -1032(%rbp)
	leaq	-1024(%rbp), %rdx
	movl	$0, %eax
	movl	$126, %ecx
	movq	%rdx, %rdi
	rep stosq
	movq	$1024, -2104(%rbp)
	movl	$1819043176, -2071(%rbp)
	movw	$111, -2067(%rbp)
	movb	$0, -2065(%rbp)
	leaq	-2071(%rbp), %rdx
	leaq	-2104(%rbp), %rsi
	leaq	-1040(%rbp), %rax
	movl	$0, %r8d
	movl	$5, %ecx
	movq	%rax, %rdi
	call	compress2@PLT
	movl	%eax, -2108(%rbp)
	cmpl	$0, -2108(%rbp)
	je	.L5
	movl	-2108(%rbp), %eax
	movl	%eax, %edi
	call	zError@PLT
	movq	%rax, %rsi
	leaq	.LC4(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$-4, %eax
	jmp	.L6
.L5:
	movq	-2104(%rbp), %rax
	movb	%al, -2057(%rbp)
	movq	-2104(%rbp), %rdx
	leaq	-1040(%rbp), %rax
	leaq	-2064(%rbp), %rcx
	addq	$8, %rcx
	movq	%rax, %rsi
	movq	%rcx, %rdi
	call	memcpy@PLT
	movzbl	-2057(%rbp), %eax
	movsbq	%al, %rdx
	leaq	-2064(%rbp), %rsi
	movl	-2112(%rbp), %eax
	movl	$0, %ecx
	movl	%eax, %edi
	call	send@PLT
	movzbl	-2057(%rbp), %eax
	movsbl	%al, %eax
	movl	%eax, %esi
	leaq	.LC5(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	leaq	-2064(%rbp), %rsi
	movl	-2112(%rbp), %eax
	movl	$0, %ecx
	movl	$1024, %edx
	movl	%eax, %edi
	call	recv@PLT
	leaq	-2064(%rbp), %rax
	movq	%rax, %rdi
	call	strlen@PLT
	movq	%rax, %rcx
	leaq	-2064(%rbp), %rax
	movq	%rax, %rdx
	movq	%rcx, %rsi
	leaq	.LC6(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$0, %eax
.L6:
	movq	-8(%rbp), %rdx
	subq	%fs:40, %rdx
	je	.L7
	call	__stack_chk_fail@PLT
.L7:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	main, .-main
	.ident	"GCC: (GNU) 11.1.0"
	.section	.note.GNU-stack,"",@progbits

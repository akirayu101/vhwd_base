

.globl asm_swap_context

	.text  


asm_swap_context:
	mov %rdi,%rcx;
	mov %rsi,%rdx;
	mov %rbp,-0x08(%rsp);
	mov %rbx,-0x10(%rsp);
	mov %rsi,-0x18(%rsp);
	mov %rdi,-0x20(%rsp);
	mov %r12,-0x28(%rsp);
	mov %r13,-0x30(%rsp);
	mov %r14,-0x38(%rsp);
	mov %r15,-0x40(%rsp);

	mov %rsp,(%rdx);
	mov (%rcx),%rsp;

	mov -0x08(%rsp),%rbp;
	mov -0x10(%rsp),%rbx;
	mov -0x18(%rsp),%rsi;
	mov -0x20(%rsp),%rdi;
	mov -0x28(%rsp),%r12;
	mov -0x30(%rsp),%r13;
	mov -0x38(%rsp),%r14;
	mov -0x40(%rsp),%r15;

	mov (%rsp),%rdx;
	cmpq $0,%rdx;
	jne .L1;
	mov %rcx,%rdi;
	call 0x8(%rsp);
	.L1:
	ret



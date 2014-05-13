
	option casemap:none
	public asm_swap_context

	.code  


asm_swap_context proc
	;mov rcx,qword ptr[callee];
	;mov rdx,qword ptr[caller];

	mov qword ptr[rsp-08h],rbp;
	mov qword ptr[rsp-10h],rbx;
	mov qword ptr[rsp-18h],rsi;
	mov qword ptr[rsp-20h],rdi;

	mov qword ptr[rsp-28h],r12;
	mov qword ptr[rsp-30h],r13;
	mov qword ptr[rsp-38h],r14;
	mov qword ptr[rsp-40h],r15;

	mov r8,gs:[0];
	mov qword ptr[rsp-48h],r8;
	mov r8,gs:[8];
	mov qword ptr[rsp-50h],r8;
	mov r8,gs:[16];
	mov qword ptr[rsp-58h],r8;
		
	mov qword ptr[edx],rsp;
	mov rsp,qword ptr[ecx];

	mov r8,qword ptr[rsp-58h];
	mov gs:[16],r8;
	mov r8,qword ptr[rsp-50h];
	mov gs:[8],r8;
	mov r8,qword ptr[rsp-48h];
	mov gs:[0],r8;

	mov r15,qword ptr[rsp-40h];
	mov r14,qword ptr[rsp-38h];
	mov r13,qword ptr[rsp-30h];
	mov r12,qword ptr[rsp-28h];

	mov rdi,qword ptr[rsp-20h];
	mov rsi,qword ptr[rsp-18h];
	mov rbx,qword ptr[rsp-10h];
	mov rbp,qword ptr[rsp-08h];

	mov rdx,qword ptr[rsp];
	cmp rdx,0;
	jne lb_exit;
	;push rcx;
	call qword ptr[rsp+8h];
	lb_exit:
	ret
asm_swap_context endp

 
	end

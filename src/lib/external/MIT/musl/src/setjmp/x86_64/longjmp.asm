
; @@@

_TEXT	segment

		public	longjmp

		align	16

longjmp	proc

; int longjmp(jmp_buf, int);
; BUG: 浮動小数点数レジスタを復旧していない

; longjmp の返り値が 0 の場合、1 に変更
	mov rax, rdx
	test rax, rax
	jnz skip
	inc rax
skip:

; jmp_buf から rbx, rbp, r12, r14, r15 レジスタを復旧
	mov rbx, qword ptr [rcx]
	mov rsi, qword ptr [rcx + 8 * 1]
	mov rdi, qword ptr [rcx + 8 * 2]
	mov rbp, qword ptr [rcx + 8 * 3]
	mov r12, qword ptr [rcx + 8 * 4]
	mov r13, qword ptr [rcx + 8 * 5]
	mov r14, qword ptr [rcx + 8 * 6]
	mov r15, qword ptr [rcx + 8 * 7]

; rsp レジスタを復旧
	mov rdx, qword ptr [rcx + 8 * 8]
	mov rsp, rdx

; return address へ jmp
	mov rdx, qword ptr [rcx + 8 * 9]
	jmp rdx

longjmp	endp

_TEXT	ends
		end

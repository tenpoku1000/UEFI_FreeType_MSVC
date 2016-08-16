
; @@@

_TEXT   segment

        public   setjmp

        align    16

setjmp  proc

; int setjmp(jmp_buf);

; jmp_buf に rbx, rbp, r12, r14, r15 レジスタを保存
    mov qword ptr [rcx], rbx
    mov qword ptr [rcx + 8 * 1], rsi
    mov qword ptr [rcx + 8 * 2], rdi
    mov qword ptr [rcx + 8 * 3], rbp
    mov qword ptr [rcx + 8 * 4], r12
    mov qword ptr [rcx + 8 * 5], r13
    mov qword ptr [rcx + 8 * 6], r14
    mov qword ptr [rcx + 8 * 7], r15

; jmp_buf に rsp レジスタを保存(return address 除外)
    lea rdx, [rsp + 8]
    mov qword ptr [rcx + 8 * 8], rdx

; jmp_buf に return address の保存
    mov rdx, [rsp]
    mov qword ptr [rcx + 8 * 9], rdx

; return 0;
    xor rax, rax
    ret

setjmp  endp

_TEXT   ends
        end

; @@@

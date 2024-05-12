section .text
  global _start

_start:
  mov rax, 0             ; syscall for read
  mov rdi, 0             ; providing file descriptor as 0(stdin)
  mov rsi, tape          ; load tape address
  add rsi, [pointer]     ; go to pointer address
  mov rdx, 1             ; length of the string (in this case, 1)
  syscall                ; provoke

  mov rax, 1             ; syscall for write
  mov rdi, 1             ; providing file descriptor as 1(stdout)
  mov rsi, tape          ; load tape address
  add rsi, [pointer]     ; go to pointer address
  mov rdx, 1             ; length of the string (in this case, 1)
  syscall

  inc qword [pointer]    ; increase the pointer

  mov rax, 60            ; syscall for exit
  mov rdi, 0             ; exit code 0
  syscall                ; invode exit syscall

section .bss
tape: resq 30000
pointer: resq 1          ; pointer to current cell


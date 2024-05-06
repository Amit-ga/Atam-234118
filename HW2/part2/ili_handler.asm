.globl my_ili_handler
.extern what_to_do, old_ili_handler

.text
.align 4, 0x90
my_ili_handler:
#fault pushes theinstruction that cuased the fault onto the kernel stack
  mov (%rsp), %rbx #rbx contains the bad opcode
  mov (%rbx), %rbx
  xor %r9, %r9  #%r9 is 0 if command is one byte abd 1 if command is 2 bytes
  movb %bl, %al
  cmpb $0x0F, %al
  movb %bl, %dil
  je two_bytes
  jmp next
two_bytes:
  mov $1,%r9
  movb %bh, %al
  movb %al, %dil
next:
  push %r9
  call what_to_do
  pop %r9
#rax contains the result of what to do
  cmp $0, %rax
  jne back_to_user_code
  je handler
back_to_user_code:
  mov %rax, %rdi
  cmp $0, %r9
  je add_one
  jne add_two
add_one:
  addq $1, (%rsp)
  jmp end
add_two:
  addq $2, (%rsp)
  jmp end
handler:
  jmp *old_ili_handler
end:
  iretq

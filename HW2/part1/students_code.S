.global count_above

.section .data
num: .fill 21,1,0
char: .zero 1
string_terminator: .ascii "\n"

.section .text
count_above:
#prolog
pushq %rbp
movq %rsp,%rbp
#%dil - char saparator (1 byte)
#esi- num (4 bytes)
#backup
xor %r8, %r8
xor %r9, %r9
movb %dil, %r8b
movzx %r8b, %r8
movq %rsi, %r9

#save mammory access
leaq char(%rip), %r10 #char address is in %r10
# movb (%r10), %r13b #char value is in %r13
leaq string_terminator(%rip), %r11 #\n address is in %r11
movb (%r11), %r14b #\n is in %r14
leaq num(%rip), %r12 #num address is in %r12

push %r8
push %r9
push %r10
push %r11
#get file path
call get_path #%rax contains the filename
pop %r11
pop %r10
pop %r9
pop %r8

movq %rax, %rdi #%rdi contains the filename
movq $2, %rax #%rax contains the service number for opening a file
movq $0, %rsi #opening flags
push %r11
syscall
pop %r11
#rax now contains the descriptor of the file

movq %rax, %rdi #rdi now contains the descriptor of the file
xor %r15, %r15
xor %rbx, %rbx
xor %rcx, %rcx

read_char:
  mov $0, %rax
  movq %r10, %rsi
  mov $1, %rdx
  push %rcx
  push %r11
  syscall
  pop %r11
  pop %rcx
  cmpb %r8b, (%r10) #check if arrived at saparator
  je full_num
  cmpb $1, %al#check if arrived at EOF
  jne full_num_end
  xor %r13, %r13
  movb (%r10), %r13b #char value is in %r13
  movb %r13b, (%r12,%rcx) ##
  inc %rcx
  jmp read_char
full_num_end:
 mov $1, %r15
full_num:
    # movb %r11b, num(,%rcx,1)  ##
    movb %r14b, (%r12,%rcx)  
    # leaq (num), %rdi #save string (char array)address   ##
    xor %rcx, %rcx
    push %rdi
    movq %r12, %rdi
      push %rcx
      push %r8
      push %r9
      push %r10
      push %r11
      call atam_atol
      #rax contains the decimal value of num
      pop %r11
      pop %r10
      pop %r9
      pop %r8
      pop %rcx
      pop %rdi
      cmpq %r9, %rax
      jg greater
next:
    cmpq $1, %r15
    je epilog_count_above
    jmp read_char
greater:
    inc %rbx
    jmp next
epilog_count_above: 
    #restore
    #movsx %dil, %r8
    #movsx %esi, %r9
    mov %rbx, %rax
    leave
    ret

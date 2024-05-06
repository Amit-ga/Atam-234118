.global _start
.section .text
_start:
	xor %r15, %r15 #neg flag
	movl num, %r8d
	cmp $0, %r8d #dst<src
	je end_HW1
	jl Neg_HW1
pos_HW1:
        mov $destination, %r10
        mov $source, %rbx
        cmp %rbx, %r10
        je  check_done_HW1
        ja back_HW1
        jb front_HW1
check_done_HW1:
    cmp $0, %r15
    je end_HW1      
    cmp %rbx, %r10
    jb back_HW1
front_HW1:
        mov  $0, %eax
front_loop_HW1:
        movb source(%eax), %cl
        movb %cl, destination(%eax)
        inc %eax
        cmp %r8d, %eax
        jne front_loop_HW1
        cmp $1, %r15
        je swap_HW1
        jmp end_HW1

back_HW1:
        mov %r8d, %eax        # start from i=length
back_loop_HW1:
        movb  source-1(%eax), %cl
        movb  %cl, destination-1(%eax)
        dec   %eax
        jnz   back_loop_HW1
        cmp $1, %r15
        je swap_HW1
        jmp end_HW1
Neg_HW1:
	neg %r8d
	mov $1, %r15
	jmp pos_HW1
swap_HW1:
	#mov $destination, %r10
reverse_dest_HW1:
    #lea -1(%r10,%r8,1), %r11 #%r11 = address of last entry
    cmp $1, %r8d
    jbe end_HW1 #yes, it's done aleady
    mov $0, %eax
    dec %r8
Next_HW1:
    movb destination(%eax), %cl #%cl = value at start
    movb destination(%r8d), %dl #%dl = value at end
    movb %cl, destination(%r8d)
    movb %dl, destination(%eax)
    inc %eax
    dec %r8d
    cmp %eax,%r8d #Have we reached the middle?
    ja  Next_HW1 #no, keep going
end_HW1:

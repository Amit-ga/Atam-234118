.global _start

.section .text
_start:
	mov  $array1, %rax
	mov  $array2, %rbx
	mov  $mergedArray, %rcx
loop_HW1:
        movl (%rax), %edi
	movl (%rbx), %esi
	testl %edi, %edi #if rax=0 check if %rbx is also 0
	jne Next_HW1
	cmp $0, %esi #if rax=0 and %rbx=0 end the program
	je end_HW1
Next_HW1:
	cmpl %esi, %edi
	jb Second_Array_bigger_HW1 #%rax<%rbx
	ja First_Array_bigger_HW1 #%rax>%rbx
	je Equal_HW1 #%rax=%rbx
Second_Array_bigger_HW1:
	movl %esi, (%rcx) #store the value of esi into the 32 bits at location rcx 
INC1_HW1:	
	add $4, %rbx
	cmp %esi, (%rbx)
	je INC1_HW1
        add $4, %rcx
	jmp loop_HW1
First_Array_bigger_HW1:
	movl  %edi, (%rcx)
INC2_HW1:
	add $4, %rax
	cmp %edi, (%rax)
	je INC2_HW1
        add $4, %rcx
	jmp loop_HW1
Equal_HW1:
	movl %edi, (%rcx)
INC3_HW1:
        add $4, %rax
        cmp %edi, (%rax)
        je INC3_HW1
INC4_HW1:
	add $4, %rbx
        cmp %edi, (%rbx)
        je INC4_HW1
INC5_HW1:
        add $4, %rcx
        cmp %edi, (%rax)
        je INC5_HW1
        
        jmp loop_HW1
end_HW1:
	movl  $0, %ecx

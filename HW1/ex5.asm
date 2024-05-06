.global _start

.section .text
_start:
	movq $0, %r8 #address of first node to swap
	movq $0, %r9 #address second node to swap 
        movq $0, %r10 #address of prev of first node to swap
        movq $0, %r11 #address of prev of second node to swap        
	movq head, %rsi
        cmp $0, %rsi
        je end_HW1
	movq $0, %rax 
	movsxl val, %rbx #val - 32 bit
        cmp (%rsi), %rbx #first node is val 
        je First_HW1
        jne loop_HW1
First_HW1:
        inc %rax
        movq $head-8, %r10
        movq %rsi, %r8
loop_HW1:
        cmp $0, %rax
        je save_first_node_prev_HW1
        cmp $1,%rax
        je save_second_node_prev_HW1
        
back_from_prev_save_HW1:
	movq 8(%rsi), %rsi
        cmp $0, %rsi
	je Count_HW1
	cmp (%rsi), %rbx
	je Equal_HW1
        jne loop_HW1

save_first_node_prev_HW1:
         movq %rsi, %r10
         jmp back_from_prev_save_HW1
save_second_node_prev_HW1:
         movq %rsi, %r11
         jmp back_from_prev_save_HW1
         
Equal_HW1:
	inc %rax
        cmp $2, %rax
        ja end_HW1
	cmp $1, %rax
	je Save_first_node_HW1
	movq %rsi, %r9
        jmp loop_HW1
Save_first_node_HW1:
	movq %rsi, %r8
        jmp loop_HW1
        
Count_HW1:
	cmp $2, %rax
	jne end_HW1

Swap_HW1:
        #%r8 address of node1
        #%r9 address of node2
        #%r10 address of prev1
        #%r11 #address of prev2
        #swaps %r8 and %r9:
#check if node1 and node2 are adjacent
         #node1.next = 2?
          cmp 8(%r8), %r9 
          je Adjacent_HW1 
          jne NOT_Adjacent_HW1        
 Adjacent_HW1:
        #cmp %r8, %r9
       # jb Node2_first
        # we need: 
        #prev1.next -> 2
        movq %r9, 8(%r10) 
        # node2.next -> 1
        movq 8(%r9), %rax #backup next2
        movq %r8, 8(%r9)
        ### 
        #SAME AS NODE1.NEXT
        ## prev2.next -> 1
        #movq %r8, 8(%r11) 
        ###
        #node1.next -> node2.next
        movq %rax, 8(%r8)
        jmp end_HW1
 NOT_Adjacent_HW1:
         #we need: 
        #prev1.next -> 2
        movq %r9, 8(%r10) 
        # node2.next -> node1.next
        movq 8(%r9), %rax #backup next2
        movq 8(%r8), %rbx #save next1
        movq %rbx, 8(%r9)
        # prev2.next -> 1
        movq %r8, 8(%r11)
        #node1.next -> node2.next
        movq %rax, 8(%r8)

end_HW1:

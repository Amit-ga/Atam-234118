.global _start

.section .text
_start:
    movq $root, %rax
    cmp $0, (%rax)
    je empty_tree_HW1
    movq (%rax), %rax
    movq $new_node, %rbx
    movq (%rbx), %r9

traverse_HW1:
    mov (%rax), %r8
    cmpq %r9, %r8
    je end_HW1 #if root ==  new_node
    movq %rax, %rcx #backup
    jl right_HW1 #root<r9
    jmp left_HW1
left_HW1:
    movq 8(%rax), %rax
    cmp $0, %rax #reached a leaf
    jne traverse_HW1
    movq $new_node, 8(%rcx)
    jmp end_HW1
right_HW1:
    movq 16(%rax), %rax
    cmp $0, %rax #reached a leaf
    jne traverse_HW1
    movq $new_node, 16(%rcx) 
    jmp end_HW1
empty_tree_HW1:
    movq $new_node, root
    jmp end_HW1 
end_HW1:

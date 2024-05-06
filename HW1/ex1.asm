.global _start

.section .text
_start:
	movq num, %rsi
	and $3,%rsi
	cmp $0,%rsi
	je cond_dividable_HW1
	jmp cond_non_dividable_HW1
cond_dividable_HW1:
	movb $1,Bool
	jmp end_HW1
cond_non_dividable_HW1:
	movb $0,Bool
end_HW1:

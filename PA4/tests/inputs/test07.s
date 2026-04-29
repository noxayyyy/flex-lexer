.data
sum: .word 0
i: .word 0
sum: .word 0
i: .word 0

.text
.globl main
main:
li sum, 0
li i, 0
L0:
slt t0, i, 10
beqz t0, L1
mul t1, i, 4
add t2, sum, t1
lw sum, t2
add t3, i, 1
lw i, t3
j L0
L1:
li $v0, 1
lw $a0, sum
syscall

li $v0, 10
syscall

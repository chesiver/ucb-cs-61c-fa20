.globl factorial

.data
n: .word 12

.text
main:
    la t0, n
    lw a0, 0(t0)
    jal ra, factorial

    addi a1, a0, 0
    addi a0, x0, 1
    ecall # Print Result

    addi a1, x0, '\n'
    addi a0, x0, 11
    ecall # Print newline

    addi a0, x0, 10
    ecall # Exit

factorial:
    # YOUR CODE HERE
    add t1, x0, a0
    addi a0, x0, 1
loop:
    mul a0, a0, t1
    addi t1, t1, -1
    beq t1, x0, exit
    jal x0, loop
exit:
    jr ra
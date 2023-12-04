.globl dot

.text
# =======================================================
# FUNCTION: Dot product of 2 int vectors
# Arguments:
#   a0 (int*) is the pointer to the start of v0
#   a1 (int*) is the pointer to the start of v1
#   a2 (int)  is the length of the vectors
#   a3 (int)  is the stride of v0
#   a4 (int)  is the stride of v1
# Returns:
#   a0 (int)  is the dot product of v0 and v1
# Exceptions:
# - If the length of the vector is less than 1,
#   this function terminates the program with error code 75.
# - If the stride of either vector is less than 1,
#   this function terminates the program with error code 76.
# =======================================================
length_error:
    li a1, 75
    jal exit2

stride_error:
    li a1, 76
    jal exit2

dot:
    li t0, 1
    blt a2, t0, length_error
    blt a3, t0, stride_error
    blt a4, t0, stride_error

    # cnt
    li t0, 0
    # index 1
    li t1, 0
    # index 2
    li t2, 0
    # res
    li t3, 0
    # Prologue


loop_start:
    bge t0, a2, loop_end

    slli t4, t1, 2
    add t4, t4, a0
    lw t4, 0(t4)
    slli t5, t2, 2
    add t5, t5, a1
    lw t5, 0(t5)

    mul t4, t4, t5

    add t3, t3, t4

    addi t0, t0, 1
    add t1, t1, a3
    add t2, t2, a4

    j loop_start

loop_end:
    mv a0, t3
    # Epilogue
    ret

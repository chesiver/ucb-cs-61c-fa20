.globl matmul

.text
# =======================================================
# FUNCTION: Matrix Multiplication of 2 integer matrices
# 	d = matmul(m0, m1)
# Arguments:
# 	a0 (int*)  is the pointer to the start of m0 
#	a1 (int)   is the # of rows (height) of m0
#	a2 (int)   is the # of columns (width) of m0
#	a3 (int*)  is the pointer to the start of m1
# 	a4 (int)   is the # of rows (height) of m1
#	a5 (int)   is the # of columns (width) of m1
#	a6 (int*)  is the pointer to the the start of d
# Returns:
#	None (void), sets d = matmul(m0, m1)
# Exceptions:
#   Make sure to check in top to bottom order!
#   - If the dimensions of m0 do not make sense,
#     this function terminates the program with exit code 72.
#   - If the dimensions of m1 do not make sense,
#     this function terminates the program with exit code 73.
#   - If the dimensions of m0 and m1 don't match,
#     this function terminates the program with exit code 74.
# =======================================================
dimensions_error_m0:
    li a1, 72
    jal exit2

dimensions_error_m1:
    li a1, 73
    jal exit2

dimensions_error_not_match:
    li a1, 74
    jal exit2

matmul:
    # Error checks
    li t0, 1
    blt a1, t0, dimensions_error_m0
    blt a2, t0, dimensions_error_m0
    blt a4, t0, dimensions_error_m1
    blt a5, t0, dimensions_error_m1
    bne a2, a4, dimensions_error_not_match

    # Prologue
    addi sp, sp, -4
    sw ra, 0(sp)

    li t0, 0

    # total_cnt
    li t2, 0

outer_loop_start:
    bge t0, a1, outer_loop_end
    
    li t1, 0

inner_loop_start:
    bge t1, a5, inner_loop_end

    addi sp, sp, -40
    sw a0, 0(sp)
    sw a1, 4(sp)
    sw a2, 8(sp)
    sw a3, 12(sp)
    sw a4, 16(sp)
    sw a5, 20(sp)
    sw a6, 24(sp)
    sw t0, 28(sp)
    sw t1, 32(sp)
    sw t2, 36(sp)

    mul t3, t0, a2
    slli t3, t3, 2
    add a0, a0, t3

    mv t4, t1
    slli t4, t4, 2
    add a1, a3, t4

    # a2 v.length == m0.column_num
    mv a2, a2

    li a3, 1

    mv a4, a5

    jal dot
    mv t5, a0

    lw a0, 0(sp)
    lw a1, 4(sp)
    lw a2, 8(sp)
    lw a3, 12(sp)
    lw a4, 16(sp)
    lw a5, 20(sp)
    lw a6, 24(sp)
    lw t0, 28(sp)
    lw t1, 32(sp)
    lw t2, 36(sp)

    addi sp, sp, 40

    slli t4, t2, 2
    add t4, t4, a6
    sw t5, 0(t4)

    addi t2, t2, 1

    addi t1, t1, 1
    j inner_loop_start


inner_loop_end:
    addi t0, t0, 1
    j outer_loop_start

outer_loop_end:
    # Epilogue
    lw ra, 0(sp)
    addi sp, sp, 4

    ret

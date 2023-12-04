.globl argmax

.text
# =================================================================
# FUNCTION: Given a int vector, return the index of the largest
#	element. If there are multiple, return the one
#	with the smallest index.
# Arguments:
# 	a0 (int*) is the pointer to the start of the vector
#	a1 (int)  is the # of elements in the vector
# Returns:
#	a0 (int)  is the first index of the largest element
# Exceptions:
# - If the length of the vector is less than 1,
#   this function terminates the program with error code 77.
# =================================================================

length_error:
    li a1, 78
    jal exit2
    
argmax:
    # Prologue
    li t0, 1
    blt a1, t0, length_error
    
loop_start:
    li t0, 0
    lw t1, 0(a0)
    li t2, 1
    j loop_continue

no_replace:
    addi t2, t2, 1
    j loop_continue
    
loop_continue:
    bge t2, a1, loop_end

    slli t3, t2, 2
    add t3, t3, a0
    lw t3, 0(t3)

    bge t1, t3, no_replace

    # replace
    mv t0, t2
    mv t1, t3
    addi t2, t2, 1
    j loop_continue

loop_end:
    mv a0, t0
    # Epilogue
    ret

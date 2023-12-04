.globl read_matrix

.data
failure_message: .asciiz "Test failed for some reason.\n"
success_message: .asciiz "Sanity checks passed! Make sure there are no CC violations.\n"

.text
# ==============================================================================
# FUNCTION: Allocates memory and reads in a binary file as a matrix of integers
#
# FILE FORMAT:
#   The first 8 bytes are two 4 byte ints representing the # of rows and columns
#   in the matrix. Every 4 bytes afterwards is an element of the matrix in
#   row-major order.
# Arguments:
#   a0 (char*) is the pointer to string representing the filename
#   a1 (int*)  is a pointer to an integer, we will set it to the number of rows
#   a2 (int*)  is a pointer to an integer, we will set it to the number of columns
# Returns:
#   a0 (int*)  is the pointer to the matrix in memory
# Exceptions:
# - If malloc returns an error,
#   this function terminates the program with error code 88.
# - If you receive an fopen error or eof, 
#   this function terminates the program with error code 90.
# - If you receive an fread error or eof,
#   this function terminates the program with error code 91.
# - If you receive an fclose error or eof,
#   this function terminates the program with error code 92.
# ==============================================================================
malloc_error:
    li a1, 88
    jal exit2

file_open_error:
    li a1, 90
    jal exit2

file_read_error:
    li a1, 91
    jal exit2

file_close_error:
    li a1, 92
    jal exit2

read_matrix:

    # Prologue
	addi sp, sp, -28
    sw s0, 0(sp)
    sw s1, 4(sp)
    sw s2, 8(sp)
    sw s3, 12(sp)
    sw s4, 16(sp)
    sw s5, 20(sp)
    sw ra, 24(sp)

    # keep parameters
    mv s0, a0
    mv s1, a1
    mv s2, a2

    # fopen
    mv a1, s0
    li a2, 0
    jal fopen
    # keep file discriptor
    mv s3, a0
    li t0, -1
    beq s3, t0, file_open_error
    
    # fread rows
    mv a1, s3
    mv a2, s1
    li a3, 4
    jal fread
    li t0, 4
    blt a0, t0, file_read_error

    # fread cols
    mv a1, s3
    mv a2, s2
    li a3, 4
    jal fread
    li t0, 4
    blt a0, t0, file_read_error


    # malloc for matrix
    # calculate total size
    lw t0, 0(s1)
    lw t1, 0(s2)
    mul s4, t0, t1
    slli a0, s4, 2

    jal malloc
    beq a0, x0, malloc_error
    # keep buffer
    mv s5, a0


    # read matrix
    mv a1, s3
    ## buffer
    mv a2, s5
    ## bytes number = size * 4
    slli a3, s4, 2

    jal fread
    slli t0, s4, 2
    blt a0, t0, file_read_error

    # close
    mv a1, s3
    jal fclose
    bne a0, x0, file_close_error

    mv a0, s5
    mv a1, s1
    mv a2, s2

    # Epilogue
    lw s0, 0(sp)
    lw s1, 4(sp)
    lw s2, 8(sp)
    lw s3, 12(sp)
    lw s4, 16(sp)
    lw s5, 20(sp)
    lw ra, 24(sp)
    addi sp, sp, 28

    ret

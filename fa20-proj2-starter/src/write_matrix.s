.globl write_matrix

.data
tmp_array: .word 1 2

.text
# ==============================================================================
# FUNCTION: Writes a matrix of integers into a binary file
# FILE FORMAT:
#   The first 8 bytes of the file will be two 4 byte ints representing the
#   numbers of rows and columns respectively. Every 4 bytes thereafter is an
#   element of the matrix in row-major order.
# Arguments:
#   a0 (char*) is the pointer to string representing the filename
#   a1 (int*)  is the pointer to the start of the matrix in memory
#   a2 (int)   is the number of rows in the matrix
#   a3 (int)   is the number of columns in the matrix
# Returns:
#   None
# Exceptions:
# - If you receive an fopen error or eof,
#   this function terminates the program with error code 93.
# - If you receive an fwrite error or eof,
#   this function terminates the program with error code 94.
# - If you receive an fclose error or eof,
#   this function terminates the program with error code 95.
# ==============================================================================

file_open_error:
    li a1, 93
    jal exit2

file_write_error:
    li a1, 94
    jal exit2

file_close_error:
    li a1, 95
    jal exit2

write_matrix:
    # Prologue
    addi sp, sp, -28
    sw ra, 0(sp)
    sw s0, 4(sp)
    sw s1, 8(sp)
    sw s2, 12(sp)
    sw s3, 16(sp)
    sw s4, 20(sp)
    sw s5, 24(sp)

    # save parameters
    mv s0, a0
    mv s1, a1
    mv s2, a2
    mv s3, a3

    # fopen
    mv a1, s0
    li a2, 1
    jal fopen
    li t0, -1
    beq a0, t0, file_open_error
    # keep file discriptor
    mv s4, a0

    # fwrite for rows/cols 
    la t0, tmp_array
    sw s2, 0(t0)
    sw s3, 4(t0)
    mv a1, s4
    mv a2, t0
    li a3, 2
    li a4, 4
    jal fwrite
    li t0, 2
    blt a0, t0, file_write_error

    # fwrite for matrix
    mul s5, s2, s3
    mv a1, s4
    mv a2, s1
    mv a3, s5
    li a4, 4
    jal fwrite
    blt a0, s5, file_write_error

    # close
    mv a1, s4
    jal fclose
    bne a0, x0, file_close_error


    # Epilogue
    lw ra, 0(sp)
    lw s0, 4(sp)
    lw s1, 8(sp)
    lw s2, 12(sp)
    lw s3, 16(sp)
    lw s4, 20(sp)
    lw s5, 24(sp)
    addi sp, sp, 28

    ret

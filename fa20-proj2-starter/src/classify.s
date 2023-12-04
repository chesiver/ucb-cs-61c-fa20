.globl classify

.data
dimention_m0: .word 0 0
dimention_m1: .word 0 0
dimention_input: .word 0 0

print_prefix: .asciiz "Test Print: "
line_break: .asciiz "\n"

.text

debug:
    addi sp, sp, -8
    sw, ra, 0(sp)
    sw, s0, 4(sp)
    mv s0, a0
    la a1, print_prefix
    jal print_str
    mv a1, s0
    jal print_int
    la a1, line_break
    jal print_str
    lw, ra, 0(sp)
    lw, s0, 4(sp)
    addi sp, sp, 8
    jr ra


classify:
    # =====================================
    # COMMAND LINE ARGUMENTS
    # =====================================
    # Args:
    #   a0 (int)    argc
    #   a1 (char**) argv
    #   a2 (int)    print_classification, if this is zero, 
    #               you should print the classification. Otherwise,
    #               this function should not print ANYTHING.
    # Returns:
    #   a0 (int)    Classification
    # Exceptions:
    # - If there are an incorrect number of command line args,
    #   this function terminates the program with exit code 89.
    # - If malloc fails, this function terminats the program with exit code 88.
    #
    # Usage:
    #   main.s <M0_PATH> <M1_PATH> <INPUT_PATH> <OUTPUT_PATH>
    addi sp, sp, -44
    sw ra, 0(sp)
    sw s0, 4(sp)
    sw s1, 8(sp)
    sw s2, 12(sp)
    sw s3, 16(sp) # m0
    sw s4, 20(sp) # m1
    sw s5, 24(sp) # input
    sw s6, 28(sp) # m0 * input
    sw s7, 32(sp) # m1 * ReLU(m0 * input)
    sw s8, 36(sp) # m1 rows
    sw s9, 40(sp) # input cols

    # store parameters
    mv s0, a0
    mv s1, a1
    mv s2, a2

	# =====================================
    # LOAD MATRICES
    # =====================================

    # Load pretrained m0

    la t0, dimention_m0
    lw a0, 4(s1)
    addi a1, t0, 0
    addi a2, t0, 4
    jal read_matrix
    mv s3, a0

    # Load pretrained m1

    la t0, dimention_m1
    lw a0, 8(s1)
    addi a1, t0, 0
    addi a2, t0, 4
    jal read_matrix
    mv s4, a0

    # Load input matrix

    la t0, dimention_input
    lw a0, 12(s1)
    addi a1, t0, 0
    addi a2, t0, 4
    jal read_matrix
    mv s5, a0

    # =====================================
    # RUN LAYERS
    # =====================================

    # 1. LINEAR LAYER:    m0 * input

    # allocate
    la t0, dimention_m0
    lw t0, 0(t0)
    la t1, dimention_input
    lw t1, 4(t1)
    mul t0, t0, t1
    addi sp, sp, -4
    sw t0, 0(sp)
    slli t0, t0, 2
    mv a0, t0
    jal malloc
    mv s6, a0

    mv a0, s3
    la t0, dimention_m0
    lw a1, 0(t0)
    lw a2, 4(t0)
    mv a3, s5
    la t0, dimention_input
    lw a4, 0(t0)
    lw a5, 4(t0)
    mv a6, s6
    jal matmul

    lw t0, 0(sp)
    addi sp, sp, 4

    # 2. NONLINEAR LAYER: ReLU(m0 * input)

    mv a0, s6
    mv a1, t0
    jal relu

    # 3. LINEAR LAYER:    m1 * ReLU(m0 * input)

    # Allocate

    la t0, dimention_m1
    lw t0, 0(t0)
    la t1, dimention_input
    lw t1, 4(t1)
    mul t0, t0, t1
    slli t0, t0, 2
    mv a0, t0
    jal malloc
    mv s7, a0

    # Parameters of matmul

    la t0, dimention_m1
    lw s8, 0(t0)
    lw t1, 4(t0)
    la t2, dimention_m0
    lw t3, 0(t2)
    la t4, dimention_input
    lw s9, 4(t4)

    mv a0, s4
    mv a1, s8
    mv a2, t1
    mv a3, s6
    mv a4, t3
    mv a5, s9
    mv a6, s7
    jal matmul

    # =====================================
    # WRITE OUTPUT
    # =====================================
    # Write output matrix

    lw a0, 16(s1)
    mv a1, s7
    mv a2, s8
    mv a3, s9
    jal write_matrix

    # =====================================
    # CALCULATE CLASSIFICATION/LABEL
    # =====================================

    # Call argmax

    mul t0, s8, s9
    mv a0, s7
    mv a1, t0
    jal argmax

    # Print classification

    mv a1, a0
    jal print_int

    # Print newline afterwards for clarity

    la a1, line_break
    jal print_str

    # Epilogue

    lw ra, 0(sp)
    lw s0, 4(sp)
    lw s1, 8(sp)
    lw s2, 12(sp)
    lw s3, 16(sp)
    lw s4, 20(sp)
    lw s5, 24(sp)
    lw s6, 28(sp)
    lw s7, 32(sp)
    lw s8, 36(sp)
    lw s9, 40(sp)
    addi sp, sp, 44

    ret

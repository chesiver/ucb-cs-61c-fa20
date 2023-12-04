python3 -m unittest unittests.TestWriteMatrix -v

java -jar tools/venus.jar src/main.s -ms -1 inputs/simple0/bin/m0.bin inputs/simple0/bin/m1.bin inputs/simple0/bin/inputs/input0.bin  outputs/test_basic_main/student_basic_output.bin

java -jar tools/venus.jar src/main.s -ms -1 inputs/simple1/bin/m0.bin inputs/simple1/bin/m1.bin inputs/simple1/bin/inputs/input0.bin  outputs/test_basic_main/student_basic_output.bin

java -jar tools/venus.jar src/main.s -ms -1 inputs/mnist/bin/m0.bin inputs/mnist/bin/m1.bin inputs/mnist/bin/inputs/mnist_input0.bin  outputs/test_basic_main/student_basic_output.bin

java -jar tools/venus.jar src/main.s -ms -1 inputs/mnist/bin/m0.bin inputs/mnist/bin/m1.bin inputs/mnist/bin/inputs/mnist_input1.bin  outputs/test_basic_main/student_basic_output.bin

java -jar tools/venus.jar src/main.s -ms -1 inputs/mnist/bin/m0.bin inputs/mnist/bin/m1.bin inputs/mnist/bin/inputs/mnist_input2.bin  outputs/test_basic_main/student_basic_output.bin

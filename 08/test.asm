//initialize the stack pointer to 256
@256
D=A
@SP
M=D
//initialize the LCL segment to 300
@300
D=A
@LCL
M=D
//initialize the ARG segment to 400
@400
D=A
@ARG
M=D
//initialize the THIS segment to 500
@500
D=A
@THIS
M=D
//initialize the THAT segment to 600
@600
D=A
@THAT
M=D
//test.vm
//push constant 15
@15
D=A
@SP
A=M
M=D
@SP
M=M+1
//push constant 21
@21
D=A
@SP
A=M
M=D
@SP
M=M+1
//pop pointer 0
@SP
M=M-1
A=M
D=M
@THIS
M=D
//pop pointer 1
@SP
M=M-1
A=M
D=M
@THAT
M=D
//push constant 30
@30
D=A
@SP
A=M
M=D
@SP
M=M+1
//push pointer 0
@THIS
D=M
@SP
A=M
M=D
@SP
M=M+1

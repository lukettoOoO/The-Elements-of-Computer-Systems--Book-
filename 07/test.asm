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
//push constant 131
@131
D=A
@SP
A=M
M=D
@SP
M=M+1
//push constant 19
@19
D=A
@SP
A=M
M=D
@SP
M=M+1
//pop that 3
@3
D=A
@THAT
D=D+M
@R13
M=D
@SP
M=M-1
A=M
D=M
@R13
A=M
M=D
//push constant 5
@5
D=A
@SP
A=M
M=D
@SP
M=M+1
//push that 3
@3
D=A
@THAT
D=D+M
A=D
D=M
@SP
A=M
M=D
@SP
M=M+1
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
//call Sys.init
@Sys.init
0;JMP
//
//label LUCA
(LUCA)
//goto FUNC1
@FUNC1
0;JMP
//if-goto MY_LABEL
@SP
M=M-1
A=M
D=M
@MY_LABEL
D;JNE

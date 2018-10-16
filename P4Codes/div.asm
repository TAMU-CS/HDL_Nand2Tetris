// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/div.asm

// Divides R0 by R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)

// to get dividend
// a = bq + r
@2 //i = 1, and represents b
M=0

//RAM[0] represents the remainder R0

(LOOP)
	@1		//D= R1 (a)
	D=M
	@0		//D=R0-R1 (q-a)
	D=D-M
	@END
	D;JGT	//if D < q goto END

	@1
	D=M		//D=R1
	@0
	M=M-D	//R0=R0-R1
	@2
	M=M+1	//i++
	@LOOP
	0;JMP	//go to LOOP
(END)
	@LOOP
	0; JMP	//Infinite loop
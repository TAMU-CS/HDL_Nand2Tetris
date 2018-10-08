// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Mult.asm

// Do multiplication of R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)

// counter is = to the size of the number
@2
M=0		//keep track of the product
@i1
M=0		//iterator for loop1, and will represent the count
@pow2
M=1		//keep track of powers of two

(LOOP1)
	@0		//R0
	D=M
	@pow2	
	D=M-D	//check if power of 2 is greater than R0
	@LOOP2
	D; JGT	//if 2^i1 > R0

	@pow2
	D=M
	@pow2
	M=D+M
	@i1
	M=M+1
	@LOOP1
	0;JMP	//continue looping

(LOOP2)
	@i1
	D=M		//decrement the 1st iterator
	@END
	D; JLT	//if M < 0 then go to end

	@i1
	D=M
	@temp1
	M=1<D
	@temp1
	D=M
	@1
	D=D&M
	@SHIFT
	D; JGT
	(RESUME)
	@i1
	M=M-1
	@LOOP2
	0; JMP

	(SHIFT)		//total += R0 << counter
		@0
		D=M
		@i1
		D=D<M
		@2
		M=D+M
		@RESUME
		0; JMP

(END)
	@LOOP1
	0; JMP
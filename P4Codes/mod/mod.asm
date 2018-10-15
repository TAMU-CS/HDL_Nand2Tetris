//This asm computes the modulo of two numbers
//Assuming R0 stores the number a and R1 stores the number b (b can not be 0)
//so in normal programming language, the goal is to compute RAM[R0]%RAM[R1]
//The result will be put to RAM[R2]
//Assuming RAM[R1] is positive integer and RAM[R0] is non-negative integer
//write your code here.

//to iterate through for loop and subtract b from a
(START)
	@R0
	D = M
	@R3		//copy R0 to temp
	M = D
(LOOP)
	@R1
	D = M
	@R3
	MD = M - D	//R0 = R0 - R1
	@LOOP
	D; JGE		//if R0 > 0 then continue
	@R1
	D = D + M
	@R2
	M = D
(END)
	@START		//continue inf loop
	0; JMP
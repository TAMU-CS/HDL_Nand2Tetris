// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input. 
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel. When no key is pressed, the
// program clears the screen, i.e. writes "white" in every pixel.

	@8192   //(512 * 32)/16 : number of registers = pixel count / register size
	D=A
	@count
	M=D     // count = 8192
(OLOOP)
	@i		//set the iterator
	M = 0
	@KBD	//keyboard reading
	D = M
	@WHITE
	D; JEQ	//jump to white if the beyboard reading is 0
(BLACK)
	@i
	D = M
	@SCREEN
	A = A + D	//keep track of pixel
	M = -1		//set pixel to black
	@i
	M = M + 1
	D = M
	@count
	D = M - D
	@OLOOP
	D; JEQ
	@BLACK
	0; JMP
(WHITE)
	@i
	D = M
	@SCREEN
	A = A + D	//keep track of pixel
	M = 0		//set pixel to black
	@i
	M = M + 1
	D = M
	@count
	D = M - D
	@OLOOP
	D; JEQ
	@WHITE
	0; JMP
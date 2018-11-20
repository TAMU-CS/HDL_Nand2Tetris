/*
	Basic Hack Assembler
	--takes assembly program and converts to HACK machine code
*/

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
using namespace std;

//symbol table setup
void PreDefineSymbols(unordered_map<string, int>& Symbol);
void SetupSymbols(unordered_map<string, int>& Symbol, stringstream& temp);
//setup instruction translation
void PreDefineInstructions(unordered_map<string, string>&comp,
	unordered_map<string, string>&dest, unordered_map<string, string>&jump);
//bit translator funcitons
string ToHack(string& line, unordered_map<string, string>&comp,
	unordered_map<string, string>&dest, unordered_map<string, string>&jump);
//changes number to 16 bit binary string
string numTo16Bin(int num);
//parser function
void parser(ifstream& prog, ofstream& hack, unordered_map<string, int>& Symbol, unordered_map<string, string>&comp,
	unordered_map<string, string>&dest, unordered_map<string, string>&jump);

int main(int argc, char *argv[]){
	//setup the instruction tables
	unordered_map<string, string>comp;
	unordered_map<string, string>dest; 
	unordered_map<string, string>jump;
	PreDefineInstructions(comp, dest, jump);

	cout << "Basic Hack Assembler\n";
	cout << "created by: Kevin Tang\n";

	//vector<string> fnames = { "Add.asm", "Max.asm", "Rect.asm", "Pong.asm" };
	//process all the arguments
	for (int i = 0; i < argc; i++) {
		//get file name to conver to hack code
		string fname(argv[i]);

		//determine output name with proper extension
		string oname = fname.substr(0, fname.size() - 3) + "hack";

		//setup output and input files
		ofstream hackoutput(oname.c_str());
		ifstream program(fname.c_str());

		//setup the parser
		unordered_map<string, int> Symbols;
		parser(program, hackoutput, Symbols, comp, dest, jump);

		//close streams
		program.close();
		hackoutput.close();
	}

	cout << "\n\nHACK COMPLETED\n\n";

	return 0;
}

//setup functions
void PreDefineInstructions(unordered_map<string, string>&comp, 
	unordered_map<string, string>&dest, unordered_map<string, string>&jump) {
	
	//setting up comp
	//a = 0
	comp["0"] = "0101010";
	comp["1"] = "0111111";
	comp["-1"] = "0111010";
	comp["D"] = "0001100";
	comp["A"] = "0110000";
	comp["!D"] = "0001101";
	comp["!A"] = "0110001";
	comp["-D"] = "0001111";
	comp["-A"] = "0110011";
	comp["D+1"] = "0011111";
	comp["A+1"] = "0110111";
	comp["D-1"] = "0001110";
	comp["A-1"] = "0110010";
	comp["D+A"] = "0000010";
	comp["D-A"] = "0010011";
	comp["A-D"] = "0000111";
	comp["D&A"] = "0000000";
	comp["D|A"] = "0010101";
	//a = 1
	comp["M"] = "1110000";
	comp["!M"] = "1110001";
	comp["-M"] = "1110011";
	comp["M+1"] = "1110111";
	comp["M-1"] = "1110010";
	comp["D+M"] = "1000010";
	comp["D-M"] = "1010011";
	comp["M-D"] = "1000111";
	comp["D&M"] = "1000000";
	comp["D|M"] = "1010101";

	//setup dest
	dest["null"] = "000";
	dest["M"] = "001";
	dest["D"] = "010";
	dest["MD"] = "011";
	dest["A"] = "100";
	dest["AM"] = "101";
	dest["AD"] = "110";
	dest["AMD"] = "111";

	//setup jump
	jump["null"] = "000";
	jump["JGT"] = "001";
	jump["JEQ"] = "010";
	jump["JGE"] = "011";
	jump["JLT"] = "100";
	jump["JNE"] = "101";
	jump["JLE"] = "110";
	jump["JMP"] = "111";

	return;
}

void PreDefineSymbols(unordered_map<string, int>& Symbol) {
	
	//add all of the R0..15 values to symbol
	for (int i = 0; i < 16; i++) {
		Symbol["R" + to_string(i)] = i;
	}

	//add screen
	Symbol["SCREEN"] = 16384;
	Symbol["KBD"] = 24576;
	Symbol["SP"] = 0;
	Symbol["LCL"] = 1;
	Symbol["ARG"] = 2;
	Symbol["THIS"] = 3;
	Symbol["THAT"] = 4;

}

void SetupSymbols(unordered_map<string, int>& Symbol, stringstream& temp, int linecount) {
	PreDefineSymbols(Symbol);

	string line;

	//loop through the temp stream and get labels and add them into the symbols table
	for (int x = 0; x < linecount; x++) {
		getline(temp, line);

		//check if first char is parentheses
		if (line[0] == '(') {
			//get the entire word
			int i = 1;
			while (line[i] != ')') {
				i++;
			}

			Symbol[line.substr(1, i - 1)] = x;

			//adjust x as the line count
			x--;
			linecount--;
		}
		else { //remove the label from the process
			temp << line << endl;
		}
	}

	//add variable names
	int varCount = 0; //keep track of variable count
	for (int x = 0; x < linecount; x++) {
		getline(temp, line);

		//check if varible name @
		if (line[0] == '@') {
			//get the first character and see if its not a number
			if (!isdigit(line[1])) {

				//check if the symbol exists in symbol table
				string varName = line.substr(1, line.size() - 1);
				if (Symbol.find(varName) == Symbol.end()) {
					Symbol[varName] = 16 + varCount;
					varCount++;
				}

				//convert the var name to an address
				int varAddress = Symbol[varName];
				temp << "@" << varAddress << endl;
			}
			else {
				temp << line << endl;
			}
		}
		else {
			temp << line << endl;
		}
	}

	return;
}

//conversion functions
string ToHack(string& line, unordered_map<string, string>&comp,
	unordered_map<string, string>&dest, unordered_map<string, string>&jump) {
	
	string bin = "111"; //start with default 111 value
	string tdest = "null";
	string tcomp = "0";
	string tjump = "null";

	//check for = or ;
	for(int i = 1; i < line.size(); i++) {
		if (line[i] == '=') { //a destination exists
			//found destination
			tdest = line.substr(0, i);

			//get comp
			int j;
			bool compFound = false;
			for (j = i+2; j < line.size(); j++) {
				if (line[j] == ';') {
					tcomp = line.substr(i + 1, j - (i + 1));
					compFound = true;
					break;
				}
			}

			if (!compFound) { //no semicolon found, get comp
				tcomp = line.substr(i + 1, line.size() - (i + 1));
			}

			break;
		}
		else if (line[i] == ';') { //comp found, check for jump
			tcomp = line.substr(0, i);

			//get jump
			if (i != line.size() - 1) {
				tjump = line.substr(i + 1, line.size() - (i + 1));
			}
			break;
		}
	}


	bin = bin + comp[tcomp] + dest[tdest] + jump[tjump];
	return bin;
}

string numTo16Bin(int num) {
	string temp = "0000000000000000"; //16 bit starting num

	int cnt = 15;
	while (num != 0) {
		if (num % 2 == 0) { //even
			num = num / 2;
		}
		else { //odd
			num = (num - 1) / 2;
			temp[cnt] = '1';
		}

		cnt--; //move to next highest digit
	}

	return temp;
}

void parser(ifstream& prog, ofstream& hack, unordered_map<string, int>& Symbol, unordered_map<string, string>&comp,
	unordered_map<string, string>&dest, unordered_map<string, string>&jump) {
	//clean out new lines and comments
	stringstream temp;	//workspace

	//loop through the program to remove spaces and comments and print in stringstream
	int linecount = 0;//keep track of the line count
	string line;
	string line_nospace;
	while (getline(prog, line)) {

		//remove all white spaces
		bool emptyline = true;
		for (int i = 0; i < line.size(); i++) {
			if (!isspace(line[i])) {
				//check if a comment
				if (line[i] == '/') {
					break;//stop proccessing this line
				}
				else {
					emptyline = false;
					temp << line[i];
				}
			}
		}

		if (!emptyline) {
			linecount++;
			temp << endl;
		}
	}

	SetupSymbols(Symbol, temp, linecount);

	//handle instructions
	while (getline(temp, line)){
		if (line[0] == '@') {
			hack << numTo16Bin(stoi(line.substr(1, line.size() - 1))) << endl;
		}
		else {
			//two cases: 1. address, 2. dest = comp; jump
			hack << ToHack(line, comp, dest, jump) << endl;
		}
	}

	return;
}
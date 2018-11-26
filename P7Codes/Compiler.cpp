/*
	Compiler
	Converts VM to HACK
*/

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

//line processor
string processPush(string addr, int index);
string processAr(string proc);
string processBool(string boolAr, string flag);
void processLine(ofstream& hack, string line);

//parser function
void parser(ifstream& prog, ofstream& hack);

int main(int argc, char *argv[]){
	//setup the instruction tables
	cout << "Basic VM to Hack Compiler\n";
	cout << "created by: Kevin Tang\n\n\n";

	//process all the arguments
	for (int i = 1; i < argc; i++) {
		//get file name to conver to hack code
		string fname(argv[i]);

		//determine output name with proper extension
		string oname = fname.substr(0, fname.size() - 2) + "asm";
		cout << "Processing: " << fname << " to " << oname << endl;

		//setup output and input files
		ofstream hackoutput(oname.c_str());
		ifstream program(fname.c_str());

		//setup the parser
		parser(program, hackoutput);

		//close streams
		program.close();
		hackoutput.close();
	}

	cout << "\n\nCompiled Successfully\n\n";

	return 0;
}

string processPush(string addr, int index){
	string ind = to_string(index);
	
	return "@" + addr + "\nD=M\n@" + ind + "\nA=D+A\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
}

string processPop(string addr, int index){
	string ind = to_string(index);

	return "@" + addr + "\nD=M\n@" + ind + "\nD=D+A\n@R13\nM=D\n@SP\nAM=M-1\nD=M\n@R13\nA=M\nM=D\n";	
}

string processAr(string proc){ //does arithmetic processes
	return "@SP\nM=M-1\nA=M\nD=M\nA=A-1\n" + proc;
}

string processBool(string boolAr, string flag){ //does boolean processes
        return "@SP\nAM=M-1\nD=M\nA=A-1\nD=M-D\n@FALSE" + flag + "\nD;" + boolAr + 
				"\n@SP\nA=M-1\nM=-1\n@CONTINUE" + flag + "\n0;JMP\n(FALSE" + flag + 
				")\n@SP\nA=M-1\nM=0\n(CONTINUE" + flag + ")\n";

}

//main line processor
void processLine(ofstream& hack, string line, int &jumpFlag){
	string sjumpFlag = to_string(jumpFlag);
	if(line.substr(0, 4) == "push"){
		if(line.substr(4,8) == "constant"){//push constant x
			int x = stoi(line.substr(12, line.size()));
			
			hack << "@" << x << endl;
			hack << "D=A\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
		}else if(line.substr(4,8) == "argument"){
			int x = stoi(line.substr(12, line.size()));
			
			hack << processPush("ARG", x);
		}else if(line.substr(4, 5) == "local"){
			int x = stoi(line.substr(9, line.size()));
			
			hack << processPush("LCL", x);			
		}else if(line.substr(4, 4) == "this"){
			int x = stoi(line.substr(8, line.size()));
			
			hack << processPush("THIS", x);			
		}else if(line.substr(4, 4) == "that"){
			int x = stoi(line.substr(8, line.size()));
			
			hack << processPush("THAT", x);			
		}else if(line.substr(4, 4) == "temp"){
			int x = stoi(line.substr(8, line.size())) + 5;
			
			hack << processPush("R5", x);			
		}else if(line.substr(4, 6) == "static"){
			int x = stoi(line.substr(10, line.size()));
			
			hack << "@STATIC" << x << "\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";			
		}else if(line.substr(4, 7) == "pointer"){
			int x = stoi(line.substr(11, line.size()));
			if(x == 0){
				hack << "@THIS\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
			}else if(x == 1){
				hack << "@THAT\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
				}	
		}
	}else if(line.substr(0, 3) == "pop"){
		if(line.substr(3, 5) == "local"){
			int x = stoi(line.substr(8, line.size()));
			hack << processPop("LCL",x);
		}else if(line.substr(3, 8) == "argument"){
			int x = stoi(line.substr(11, line.size()));
			hack << processPop("ARG", x);
		}else if(line.substr(3, 4) == "this"){
			int x = stoi(line.substr(7, line.size()));
			hack << processPop("THIS", x);
		}else if(line.substr(3, 4) == "that"){
			int x = stoi(line.substr(7, line.size()));
			hack << processPop("THAT", x);
		}else if(line.substr(3, 4) == "temp"){
			int x = stoi(line.substr(7, line.size())) + 5;
			hack << processPop("R5", x);
		}else if(line.substr(3, 7) == "pointer"){
			int x = stoi(line.substr(10, line.size()));
			if(x == 0){ //this
				hack << "@THIS\nD=A\n@R13\nM=D\n@SP\nAM=M-1\nD=M\n@R13\nA=M\nM=D\n";	
			}else if(x == 1){//that
				hack << "@THAT\nD=A\n@R13\nM=D\n@SP\nAM=M-1\nD=M\n@R13\nA=M\nM=D\n";									
			}
		}else if(line.substr(3, 6) == "static"){
			int x = stoi(line.substr(9, line.size()));
			hack << "@STATIC" << x << "\nD=A\n@R13\nM=D\n@SP\nAM=M-1\nD=M\n@R13\nA=M\nM=D\n";
		}
	}else if(line.substr(0,3) == "add"){ //OPERATIONS
		hack << processAr("M=M+D\n");
	}else if(line.substr(0,3) == "sub"){
		hack << processAr("M=M-D\n");
	}else if(line.substr(0,3) == "neg"){
		hack << "D=0\n@SP\nA=M-1\nM=D-M\n";
	}else if(line.substr(0,2) == "eq"){
		hack << processBool("JNE", sjumpFlag);
		jumpFlag++;
	}else if(line.substr(0,2) == "gt"){
		hack << processBool("JLE", sjumpFlag);
		jumpFlag++;
	}else if(line.substr(0,2) == "lt"){
		hack << processBool("JGE", sjumpFlag);
		jumpFlag++;		
	}else if(line.substr(0,3) == "and"){
		hack << processAr("M=M&D\n");
	}else if(line.substr(0,2) == "or"){
		hack << processAr("M=M|D\n");
	}else if(line.substr(0,3) == "not"){
		hack << "@SP\nA=M-1\nM=!M\n";
	}
}

//setup functions
void parser(ifstream& prog, ofstream& hack) {
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
					break;//stop processing this line
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
	
	int jumpcount = 0; //keep track of the jumps
	for(int i = 0; i < linecount; i++){
		getline(temp, line);
		
		//process the line
		processLine(hack, line, jumpcount);
	}
	
	

	return;
}


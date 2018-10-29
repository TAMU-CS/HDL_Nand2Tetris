/*
	Basic Hack Assembler
	--takes assembly program and converts to HACK machine code
*/

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
using namespace std;

//parser function
void parser(ifstream& prog, ofstream& hack){
	//clean out new lines and comments
	cout << "test\n";

	//loop through the program
	string line;
	string line_nospace;
	while(getline(prog, line)){

		//process line, get rid of spaces
		for(int i = 0; i < line.size(); i++){
			cout << line[i];
		}

		cout << endl;
	}


	return;
}

int main(){
	cout << "Basic Hack Assembler\n";
	cout << "Input File Name to conver: ";

	//get file name to conver to hack code
	string fname;
	cin >> fname;

	//determine output name with proper extension
	string oname = fname.substr(0, fname.size() - 3) + "hack";

	//setup output and input files
	ofstream hackoutput(oname.c_str());
	ifstream program(fname.c_str());

	parser(program, hackoutput);

	//close streams
	program.close();
	hackoutput.close();

	return 0;
}

/*
Brian Cabantug
Grace Derderian
Program #3
CPSC 323-85
Oct 30, 2017
Compilers used: Visual Studio 2013 (Visual C++ 12.0)
XCode (g++ Version 5.4)
Tested using both IDE and command line
*/

#include <iostream>
#include <fstream>
#include "parser.h"

using namespace std;

//main that takes in a file argument
int main(int argc, char* argv[]) {
	//if argument list is missing file name
	if (argc < 2) {
		//output error message
		cout << "Missing file name..." << endl;
		return 1;
	}
	//opens teh file
	ifstream fin(argv[1]);
	//if file can not open, output error message
	if (!fin) {
		cout << "file not found..." << endl;
		return 1;
	}

	parser(fin); //parser function which takes in the file
				 //closes file after program completes
	fin.close();

	return 0;
}
//testing function within IDE's
//int main() {
//	ifstream fin("six.txt");
//
//	if (!fin) {
//		cout << "file not found...";
//		return 1;
//	}
//
//	parser(fin);
//	fin.close();
//
//}
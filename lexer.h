/*
REUSING FOR PROJECT 2/3

Brian Cabantug
Grace Derderian

Program #2
CPSC 323-85
Sept 29 2017

Compilers used: Visual Studio 2013 (Visual C++ 12.0)
XCode (g++ Version 5.4)

Tested using both IDE and command line
*/

/*
Brian Cabantug
CPSC 323-85 - Fall 2017
Program #1 - testing the lexer function
Code tested using Visual C++ 2013 (Visual C++ 12.0)
*/
#ifndef LEXER_H //header file protectors
#define LEXER_H

//iostream, fstream in testlex.cpp
#include<iostream>
#include<fstream>
#include<cctype> //used for isalpha, isdigit comparison
#include<vector> //used for operator list

using namespace std;

//LexTok struct being passed to the program
struct LexTok {
	string lexeme;
	string token;
};


LexTok lexer(ifstream &file) {//takes file reference from function call

	string keywords[] = { "program", "begin", "end", "function", "read", "write", "if", "elsif", "else", "while", "do", "until", "return" };
	vector<string> keyword(keywords, keywords + sizeof(keywords) / sizeof(string));
	//vector of keywords for comparing lexStr for keywords 

	//variables to hold the string
	string lexStr = "";
	char ch;

	//lexTok variable that will be returned
	LexTok current;

	if (file >> ch) { //gets the first character from the buffer
		lexStr += ch; //add it to the string
		if (isalpha(ch)) { //checks the char to see if it is an identifier, keyword, or type
			ch = file.peek(); //peeks to the next char and takes it

			while (isalpha(ch) || isdigit(ch) || ch == '_') { //goes through the whole string until it hits white space
				ch = file.get();
				lexStr += ch;
				ch = file.peek();
			}
			current.lexeme = lexStr; //assigns the string as the lexeme

									 //checks for keyword, type, or identifier
			if (current.lexeme.compare("int") == 0 || current.lexeme.compare("real") == 0 || current.lexeme.compare("string") == 0) { // check for type
				current.token = "Type";
			}

			else {//check for keyword
				for (int i = 0; i < keyword.size(); i++) { //for loop to go through the vector of keywords
					if (current.lexeme.compare(keyword[i]) == 0) { //compares the current lexeme to check if it is a keyword
						current.token = "Keyword"; //if it is, token is keyword and breaks from the loop
						break; //break the loop if found
					}
					//if not keyword or type, assigns it as identifier
					else {
						current.token = "Identifier";
					}
				}
			}
		}

		else if (isdigit(ch)) {//check for intconst, realconst, or intconst.
			ch = file.peek(); //peeks at the next char
			while (isdigit(ch)) {	//continues to save any following digits to lexStr 
				ch = file.get();
				lexStr += ch;
				ch = file.peek();
			}

			current.lexeme = lexStr;
			current.token = "IntConst"; //assumes first instance as an intconst

			if (ch == '.') { //checks for real const

				ch = file.get(); //checks by looking past the period
				ch = file.peek();

				if (isdigit(ch)) { //checks if the character after the decimal is a digit, thus checking if it is a realConst
					lexStr += '.';	//if it is, saves it to the lexStr
					ch = file.get();
					lexStr += ch;
					ch = file.peek();

					while (isdigit(ch)) {//recieves all the digits after the period
						ch = file.get();
						lexStr += ch;
						ch = file.peek();
					}
					current.lexeme = lexStr;
					current.token = "RealConst"; //sets the current lexeme as realConst

				}

				else {		//if not a realConst, returns the period (moves the buffer back) and sets lexeme as IntConst
					file.putback(ch);
					current.lexeme = lexStr;
					current.token = "IntConst";
				}
			}
		}

		else if (ch == '"') { //checking for strConstants or eof by first finding the opening quote
			ch = file.peek(); //peeks ahead
			while (ch != '"' && !file.eof()) { //while the next char is not the closing quotes and is not at the eof, get all the characters for the string constant
				ch = file.get();
				lexStr += ch;
				ch = file.peek();
			}

			if (!file.eof()) { //assigns the end quote and lexeme as the strConstant if not at the end of the file
				ch = file.get();
				lexStr += ch;
				current.lexeme = lexStr;
				current.token = "StrConst";
			}
			else { //if eof, then return lexeme token as error for token and the rest of the string that has no close quotes
				current.lexeme = lexStr;
				current.token = "error";
			}
		}

		else if (ch == '=' || ch == '>' || ch == '<') { //check for relational operators
			if (ch == '=') { //check for equals
				current.lexeme = ch;
				current.token = "RelOp";
			}
			else if (ch == '>') { //check for greater than
				ch = file.peek();
				if (ch == '=') { //to check for greater than equal to
					ch = file.get();
					lexStr += ch;
					current.lexeme = lexStr;
					current.token = "RelOp";
				}
				else { //default >
					current.lexeme = lexStr;
					current.token = "RelOp";
				}
			}
			else if (ch == '<') { //check for less than

				ch = file.peek();
				if (ch == '=') { //to check for greater than equal to
					ch = file.get();
					lexStr += ch;
					current.lexeme = lexStr;
					current.token = "RelOp";
				}
				else if (ch == '>') { //check for not equal
					ch = file.get();
					lexStr += ch;
					current.lexeme = lexStr;
					current.token = "RelOp";
				}
				else { //default <
					current.lexeme = lexStr;
					current.token = "RelOp";
				}

			}
		}
		//check for normal operators list
		else if (ch == '.' || ch == ',' || ch == ';' || ch == ':' || ch == '(' || ch == ')' || ch == '+' || ch == '-' || ch == '*' || ch == '/') {
			ch = file.peek();//peeks ahead
			if (lexStr.compare(":") == 0 && ch == '=') { //if  :=
				ch = file.get(); //saves it to lexStr
				lexStr += ch;
			}

			current.lexeme = lexStr; //sets the operator
			current.token = "Operator";
		}

		//if not anything above, then it is an error
		else {
			current.lexeme = ch;
			current.token = "error";
		}
		//returns the current lexeme token back to the function call
		return current;
	}
	//covers eof when starting after the last lexeme available
	else {
		current.lexeme = "";
		current.token = "eof";

		return current; //returns eof lexTok
	}
}
#endif

/* REUSING FOR PROJECT 4 / MODIFYING
Brian Cabantug
Grace Derderian
Program #4
CPSC 323-85
Dec 1 2017
Compilers used: Visual Studio 2013 (Visual C++ 12.0)
XCode (g++ Version 5.4)
Tested using both IDE and command line
*/
#ifndef PARSER_H //header file protectors
#define PARSER_H

//lexer header file and string library included
#include "lexer.h"
#include <string>
#include <vector>


//vector to hold the commands after
vector <string> assemblyCommands;
//array that represents the temp registers
string tRegister[10] = {};
//vector that holds the declared variables 
vector<string> list;
//vector that 
//vector <string> ifOrder;
//vector/stack that holds the locations of the elsifs/else for the branch conds
vector<int> location;

//while counters
int whileCount = 0;
//if counter
int ifCount = 0;
//elsifcounter
int elsifCount = 0;


//Function Prototypes

void Program(ifstream&, LexTok&); //
void DeclList(ifstream&, LexTok&); //
vector<string> Decl(ifstream&, LexTok&); //
string Type(ifstream&, LexTok&);//

vector<string> VarList(ifstream&, LexTok&);
void StmtList(ifstream&, LexTok&); //
void Stmt(ifstream&, LexTok&); //
void Assign(ifstream&, LexTok&); //
void Read(ifstream&, LexTok&); //
void Write(ifstream&, LexTok&); //
void If(ifstream&, LexTok&); //
void While(ifstream&, LexTok&); //

string Cond(ifstream&, LexTok&); //
string RelOp(ifstream&, LexTok&); // 
string Expr(ifstream&, LexTok&); //
string Term(ifstream&, LexTok&); //
string Factor(ifstream&, LexTok&); //


//parser function that starts the top down recursion for grammar rules that takes in the file
void parser(ifstream &file) {
	//calls the first lexeme/token combo in the file
	LexTok curToken = lexer(file);

	//calls the Program function, checking if "program" is present
	Program(file, curToken);

}

//Display the error message
void writeError(string s, LexTok& check, ifstream &file) {
	cout << "Error: unexpected string: " << check.lexeme << ", expected " << s << endl;
}

//lexeme check
bool accept(string s, LexTok& check, ifstream &file) {

	//checks the string with the lexeme
	if (s.compare(check.lexeme) == 0) {
		check = lexer(file);
		return true;
	}
	else {
		return false;
	}
}

void expect(string s, LexTok& check, ifstream &file) {
	if (!accept(s, check, file)) {
		writeError(s, check, file);
	}
}

//Grace and Brian
void Program(ifstream& file, LexTok& token) {
	//Consume token if present
	expect("program", token, file);

	//Check for DeclList
	if (token.token.compare("Type") == 0) {
		//Call DeclList
		DeclList(file, token);
	}

	//Consume token if present
	expect("begin", token, file);
	//begins the main assembly commands 
	assemblyCommands.push_back(".text");
	assemblyCommands.push_back("main:");

	//Check for StmtList
	if ((!(token.lexeme.compare("end"))) == 0) {
		//Call StmtList
		StmtList(file, token);
	}

	//Consume token if present
	expect("end", token, file);

	//Consume token if present
	expect(".", token, file);

	//the assembly commands to end the program are added at the end
	assemblyCommands.push_back("li $v0, 10");
	assemblyCommands.push_back("syscall");

	//output commands at end of programs
	bool txt = false;
	for (vector<string>::iterator it = assemblyCommands.begin(); it != assemblyCommands.end(); it++) {
		if (it->compare(".text") == 0) {
			txt = true;
		}

		if (txt == false) {
			cout << *it << endl;
		}
		else {
			if (it->back() != ':' && it->compare(".text") != 0) {
				cout << "\t" << *it << endl;

			}
			else {
				cout << *it << endl;
			}

		}

	}

}

void DeclList(ifstream& file, LexTok& token) {
	//vector to hold the list of declarations
	vector<string> declL;

	//starts off the declarations of variables
	assemblyCommands.push_back(".data");

	//Check for initial declaration and loops back if there are more
	do {
		//Call Decl and takes the list of declarations given to add to the main vector of commands
		declL = Decl(file, token);


		//loop to go through the vector 
		for (vector<string>::iterator it = declL.begin(); it != declL.end(); it++) {

			//adds them to the main assembly Command vector
			assemblyCommands.push_back(*it);
		}

	} while (token.token.compare("Type") == 0);

}

//Changing data type to return the data statements to save variable list
vector<string> Decl(ifstream& file, LexTok& token) {

	//declaration list vector
	vector<string> decl;
	//vector of variabls
	vector<string> vars;
	//string that takes in the type
	string t = "";
	//boolean for checking if declaration errors are present
	bool error = false;

	string varName = "";



	//Call Type function, assigns the type to t
	t = Type(file, token);

	//Call ValList function, passes the vector to vars
	vars = VarList(file, token);

	//Consume token if present
	expect(";", token, file);

	//loop to set all the declarations

	for (vector<string>::iterator it = vars.begin(); it != vars.end(); it++) {
		string d = "";
		//gets the var
		d = d + *it;
		//check if the var being declared has already been declared
		if (list.empty() == true) { //if the list that checks if the vars is empty
			list.push_back(d); //add the var to the list



		}
		else { //otherwise, iterate through the vector to check if there is a duplicate present
			for (vector<string>::iterator il = list.begin(); il != list.end(); il++) {
				if (il->compare(d) == 0) { //if a duplicate is found
					error = true; //set error to true
					break; //break out of the iterator
				}
			}
		}
		//if error is found, output error message
		if (error == true) {
			cout << "error:" << d << " is declared more than once" << endl;
		}
		else { //otherwise, add the var to the list for checking
			list.push_back(d);

			varName = d;
		}

		//sets the rest of the declaration of the variable
		d = d + ":    ";
		d = d + t;
		d = d + "0";

		//saves it in the decl vector to pass back
		decl.push_back(d);

		//assign var to register (project 4 part 1)
		for (int i = 0; i < 10; i++) { //iterates through the register list
			if (tRegister[i].compare("") == 0) { //first register that is empty
				tRegister[i] = varName; //assigns the var to the register
				break; //breaks out of the loop
			}
		}

	}

	//returns decl vector
	return decl;
}


//Grace
string Type(ifstream& file, LexTok& token) {
	//check for type
	string t = "";

	//if the type is int, then return
	if (token.lexeme.compare("int") == 0)
	{
		//word is only type available so it is automatically set
		t = ".word	";

		//consume the token first
		token = lexer(file);
	}
	//returns the asssembly for type
	return t;
}

//Grace
vector<string> VarList(ifstream& file, LexTok& token) {
	//vector to hold the identifier list
	vector<string> ident;

	if (token.token.compare("Identifier") == 0)
	{
		//adds the identifier to the temp vector
		ident.push_back(token.lexeme);

		//moves to the next token
		token = lexer(file);
	}

	//if comma is found, then
	while (token.lexeme.compare(",") == 0) {
		//consume the token
		token = lexer(file);

		//check if next is another identifier
		if (token.token.compare("Identifier") == 0)
		{
			//push the next identifier to the vector
			ident.push_back(token.lexeme);
			//get next token
			token = lexer(file);
		}
	}//loop again if there is a comma following the identifier

	//return the vector
	return ident;
}

//Grace
void StmtList(ifstream& file, LexTok& token) {

	//call Stmt function
	Stmt(file, token);

	//continues to call stmt function if lexemes are Ident, read, write, if, while, do, return
	while (token.token.compare("Identifier") == 0 || token.lexeme.compare("read") == 0 || token.lexeme.compare("write") == 0 || token.lexeme.compare("if") == 0 || token.lexeme.compare("while") == 0) {
		Stmt(file, token);
	}

}

//Grace
void Stmt(ifstream& file, LexTok& token) {
	//if token is identifier
	if (token.token.compare("Identifier") == 0)
	{	//calls Assign function and output rule

		//checks if identifier was declared
		//bool for checking set to false
		bool err = false;
		//iterator to check through list
		for (vector<string>::iterator it = list.begin(); it != list.end(); it++) {
			if (token.lexeme.compare(*it) != 0) { //if the identifier was not found in the list
				err = true; //return that it is true
			}
			else { //otherwise, if it is in the list, set to false
				err = false;
				break;// breaks out of loop
			}
		}//outputs error if found
		if (err == true) {
			cout << "error: using " << token.lexeme << " without declaring first" << endl;
		}

		Assign(file, token);
	}
	//if token is read
	else if (token.lexeme.compare("read") == 0)
	{	//calls Read function and output rule
		Read(file, token);
	}
	//if token is write
	else if (token.lexeme.compare("write") == 0)
	{
		//calls Write function and output rule
		Write(file, token);
	}
	//if token is if
	else if (token.lexeme.compare("if") == 0)
	{	//calls If function and output rule
		//saves the count of if that is called
		ifCount++;
		If(file, token);
	}
	//if token is while
	else if (token.lexeme.compare("while") == 0)
	{	//calls While function and output rule
		While(file, token);
	}
}

//Grace
void Assign(ifstream& file, LexTok& token) {
	//assign vector that holds the 
	//vector<string> assign;

	//variables to set the identifier and register to push to the assembly commands
	string ident = "";
	string reg = "";



	//if token is Identifier, consume token
	if (token.token.compare("Identifier") == 0)
	{
		//gets the identifier
		//ident = token.lexeme;

		//check the identifier and assign the register instead
		for (int i = 0; i < 10; i++) {
			//if the identifier is found in the register
			if (token.lexeme.compare(tRegister[i]) == 0) {
				//assign the register
				ident = "$t" + to_string(i);

				//break out of the loop
				break;

			}
		}

		token = lexer(file);
	}


	expect(":=", token, file);

	//call Expr function, and gets the register
	reg = Expr(file, token);
	//completes the command and passes it to the assemblyCommands vector
	//assemblyCommands.push_back("sw " + reg + ", " + ident);
	assemblyCommands.push_back("move " + ident + ", " + reg + "");


	//clear register after storing value back into the variable
	int rNum = reg[2] - '0';
	tRegister[rNum] = "";

	expect(";", token, file);

	for (int i = 0; i < 10; i++){
		//boolean to check if register used in conditional is part of the declared list
		bool declared = false;
		//iterates throuhg the list of declared variables to see if it keeps 
		for (vector<string>::iterator it = list.begin(); it != list.end(); it++) {
			if ((*it).compare(tRegister[i]) == 0) {
				declared = true; //if found bool is true and breaks
				break;
			}
		} //if not in the declared list, clears the register instead
		if (declared == false){
			
			tRegister[i] = "";
		}

	}
}

//BRIAN
void Read(ifstream& file, LexTok& token) {
	//vetor to hold the identifiers to read
	vector<string> idents;


	expect("read", token, file);

	expect("(", token, file);
	//call VarList func, saves the varList
	idents = VarList(file, token);
	//temp var to check which variable causes the error
	string errorVar = "";

	//first check if the identifier is present from the declaration list
	//bool var to check if it is in the declared list
	bool err = false;
	for (vector<string>::iterator ito = idents.begin(); ito != idents.end(); ito++) {
		for (vector<string>::iterator it = list.begin(); it != list.end(); it++) { //double iterator to check through both the var list and the declared list
			if (ito->compare(*it) != 0) { //if not found, the var not found is saved
				err = true;
				errorVar = *ito;
			}
			else { //otherwise, then it is false and breaks out of the inner iterator
				err = false;
				break;
			}
		}
	}
	//if the error is found, the output
	if (err == true) {
		cout << "error: using " << errorVar << " without declaring first" << endl;
	}

	//for every entry in the varList, pushes these commands
	for (vector<string>::iterator it = idents.begin(); it != idents.end(); it++) {
		assemblyCommands.push_back("li $v0, 5");
		assemblyCommands.push_back("syscall");
		assemblyCommands.push_back("sw $v0, " + *it);
	}

	expect(")", token, file);

	expect(";", token, file);
}

//Brian
void Write(ifstream& file, LexTok& token) {
	//the vector that holds the list of expressions
	vector<string> exprList;
	//counter that counts how many expressions there are evertyome one is push to exprList
	int i = 0;

	//after consuming write lexeme
	expect("write", token, file);

	expect("(", token, file);

	//keeps on finding an expression as long as the token matches with the comma
	exprList.push_back(Expr(file, token));
	//adds the commands for each expression
	assemblyCommands.push_back("li $v0, 1");
	assemblyCommands.push_back("move $a0, " + exprList[i]);
	assemblyCommands.push_back("syscall");

	//clear register used
	int regNum = exprList[i][2] - '0';
	tRegister[regNum] = "";

	while (token.lexeme.compare(",") == 0) {
		//consumes comma token
		i++; //if anouther expression is detected, add one to the counter
		token = lexer(file);
		exprList.push_back(Expr(file, token));

		assemblyCommands.push_back("li $v0, 1");
		assemblyCommands.push_back("move $a0, " + exprList[i]);
		assemblyCommands.push_back("syscall");
		//clear register used
		int regNum = exprList[i][2] - '0';
		tRegister[regNum] = "";
	}

	expect(")", token, file);

	expect(";", token, file);

}

//BRIAN 
void If(ifstream& file, LexTok& token) {
	//string to hold the conditional
	string cond = "";

	//keeps the current count of how many if's have been encountered locally
	int countOfIf = ifCount;

	//consumes if
	expect("if", token, file);

	expect("(", token, file);

	//calls Cond function, saves the branch
	cond = Cond(file, token);

	expect(")", token, file);

	//pushes the first endIf to fulfill the first condition
	//pushes the incomplete conditional back to the assemblyCommands
	assemblyCommands.push_back(cond);
	//saves the location of said conditional in the locations vector
	location.push_back(assemblyCommands.size() - 1);

	//ifOrder.push_back("");

	expect("begin", token, file);

	//cals StmtList function
	StmtList(file, token);

	expect("end", token, file);

	//if there is an elsif/else present after the if, add a branch to the end of said If 
	if (token.lexeme.compare("elsif") == 0 || token.lexeme.compare("else") == 0) {
		assemblyCommands.push_back("b endIf" + to_string(countOfIf));
	}

	//elseif statements if there are any
	if (token.lexeme.compare("elsif") == 0) {

		do {
			elsifCount++;
			//consume initial elsif
			expect("elsif", token, file);

			//starts the elsif
			assemblyCommands.push_back("elseif" + to_string(elsifCount) + ":");

			//add the label to the associated branch (like stack)
			assemblyCommands[location.back()] += "elseif" + to_string(elsifCount);
			location.pop_back();


			expect("(", token, file);
			//call Cond function
			cond = Cond(file, token);

			expect(")", token, file);
			//saves the next location the next label will be saves
			assemblyCommands.push_back(cond);
			location.push_back(assemblyCommands.size() - 1);

			expect("begin", token, file);
			//call StmtList function
			StmtList(file, token);

			expect("end", token, file);
			//if elsif or else are present, add branch to end of If
			if (token.lexeme.compare("elsif") == 0 || token.lexeme.compare("else") == 0) {
				assemblyCommands.push_back("b endIf" + to_string(countOfIf));
			}


		} while (token.lexeme.compare("elsif") == 0); //continues the loop if elseif token is present

	}

	//checking for else statement
	//if else if present
	if (token.lexeme.compare("else") == 0) {
		//starts else commands
		assemblyCommands.push_back("else" + to_string(countOfIf) + ":");
		//consume else token
		expect("else", token, file);
		//add the label to the associated branch
		assemblyCommands[location.back()] += "else" + to_string(countOfIf);
		location.pop_back();

		expect("begin", token, file);
		//call StmtList
		StmtList(file, token);
		expect("end", token, file);

		//assemblyCommands.push_back("b endIf" + to_string(countOfIf));

	}
	else { //if there is no else after the elsif(s), set the last branch with no label to this location
		assemblyCommands[location.back()] += "endIf" + to_string(countOfIf);
		location.pop_back();

	}


	//branch to end the complete ifStatement
	assemblyCommands.push_back("endif" + to_string(countOfIf) + ":");
}

void While(ifstream& file, LexTok& token) {
	//consume while token/keyword
	whileCount++;

	//variables that hold how many whiles/iteration of while is active
	string whil = "while" + to_string(whileCount);
	string endWhil = "endWhl" + to_string(whileCount);

	//saves the bracnh to jump if conditional is not met
	string cond = "";

	if (token.lexeme.compare("while") == 0) {
		//starts the while loop
		assemblyCommands.push_back(whil + ":");

		token = lexer(file);
	}

	expect("(", token, file);
	//calls Cond function, saves statement
	cond = Cond(file, token);
	//adds the endWhile jump to the condition statement
	cond = cond + endWhil;
	//add condition statement to the commands list
	assemblyCommands.push_back(cond);


	expect(")", token, file);

	expect("begin", token, file);

	//checks for stmtList if there
	if (token.token.compare("Identifier") == 0 || token.lexeme.compare("read") == 0 || token.lexeme.compare("write") == 0 || token.lexeme.compare("if") == 0 || token.lexeme.compare("while") == 0) {
		//call StmtList function
		StmtList(file, token);
	}

	expect("end", token, file);
	//pushes the commands that ends the while loop
	assemblyCommands.push_back("b " + whil);
	assemblyCommands.push_back(endWhil + ":");

}

//Grace
string Cond(ifstream& file, LexTok& token) {
	//string to return
	string cond = "";
	//registers being compared
	string r1 = "";
	string r2 = "";
	//variable for what tyoe of branch basesd on conditional
	string relo = "";

	//Call Expr function. saves first register
	r1 = Expr(file, token);

	//Call RelOp function, saves the branch
	relo = RelOp(file, token);

	//Call Expr function, saves second register
	r2 = Expr(file, token);

	//complete initial statement of branch
	cond = relo + " " + r1 + ", " + r2 + ", ";

	//clears the registers used in the conditional to free up for the following function
	
	//boolean to check if register used in conditional is part of the declared list
	bool declared = false;
	//iterates throuhg the list of declared variables to see if it keeps 
	for (vector<string>::iterator it = list.begin(); it != list.end(); it++) {
		if ((*it).compare(tRegister[r1[2]-'0']) == 0) {
			declared = true; //if found bool is true and breaks
			break;
		}
	} //if not in the declared list, clears the register instead
	if (declared == false){
		int mo = r1[2] - '0';
		tRegister[mo] = "";
	}
	
	declared = false;

	for (vector<string>::iterator it = list.begin(); it != list.end(); it++) {
		if ((*it).compare(tRegister[r2[2] - '0']) == 0) {
			declared = true; //if found bool is true and breaks
			break;
		}
	} //if not in the declared list, clears the register instead
	if (declared == false){
		int mo = r2[2] - '0';
		tRegister[mo] = "";
	}
	/////

	//return the statement
	return cond;
}

//Grace
string RelOp(ifstream& file, LexTok& token) {
	//string to hold which branch to use
	string relator = "";

	//Check if it is a RelOp
	if (token.token.compare("RelOp") == 0)
	{
		//equal
		if (token.lexeme.compare("=") == 0) {
			//return opposite branch for the comparison
			relator = "bne";
		}
		//not equal
		else if (token.lexeme.compare("<>") == 0) {
			relator = "beq";
		}
		//less than
		else if (token.lexeme.compare("<") == 0) {
			relator = "bge";
		}
		//greater than
		else if (token.lexeme.compare(">") == 0) {
			relator = "ble";
		}
		//less or equal
		else if (token.lexeme.compare("<=") == 0) {
			relator = "bgt";
		}
		//greater or equal
		else {
			relator = "blt";
		}


		//consume lexeme
		token = lexer(file);
	}
	//return branch
	return relator;

}

//Grace
string Expr(ifstream& file, LexTok& token) {
	//registers 1 and 2
	string r1 = "";
	string r2 = "";

	//the next empty register to store the value into
	string r3 = "";



	//Call Term function, set rregister 1
	r1 = Term(file, token);

	//If lexeme is + or -
	while (token.lexeme.compare("+") == 0 || token.lexeme.compare("-") == 0) {
		if (token.lexeme.compare("+") == 0) {
			//consume token
			token = lexer(file);

			//call the factor if it is factor
			r2 = Term(file, token);

			//assigns the 3rd register as the next empty register to move the calculated value to
			for (int i = 0; i < 10; i++) {

				if (tRegister[i].compare("") == 0) {
					//assign it as the register to use for assignment
					r3 = "$t" + to_string(i);
					tRegister[i] = r1 + " + " + r2;
					break;
				}
			}

			//gets add command
			assemblyCommands.push_back("add " + r3 + ", " + r1 + ", " + r2);
			//clears the register that is unused after calc
			/*int mo = r3[2] - '0';
			tRegister[mo] = "";*/

		}
		else if (token.lexeme.compare("-") == 0) {
			//consume token
			token = lexer(file);

			//call the factor if it is factor
			r2 = Term(file, token);

			//assigns the 3rd register as the next empty register to move the calculated value to
			for (int i = 0; i < 10; i++) {

				if (tRegister[i].compare("") == 0) {
					//assign it as the register to use for assignment
					r3 = "$t" + to_string(i);
					tRegister[i] = r1 + " + " + r2;
					break;
				}
			}


			//gets sub command
			assemblyCommands.push_back("sub " + r3 + ", " + r1 + ", " + r2);
			//clears the register that is unused after calc
			/*int mo = r3[2] - '0';
			tRegister[mo] = "";*/
		}

		//returns the value of r3 after calculation to store into the variable
		return r3;

	}

	//return the register with the value
	return r1;
}

//Grace
string Term(ifstream& file, LexTok& token) {
	string r1 = "";
	string r2 = "";

	string r3 = "";

	//Call Factor function
	r1 = Factor(file, token);

	//Check if lexeme is * or /
	while (token.lexeme.compare("*") == 0 || token.lexeme.compare("/") == 0) {
		//multiply
		if (token.lexeme.compare("*") == 0) {


			//consume token
			token = lexer(file);

			//call the factor if it is factor
			r2 = Factor(file, token);

			//assigns the 3rd register as the next empty register to move the calculated value to
			for (int i = 0; i < 10; i++) {

				if (tRegister[i].compare("") == 0) {
					//assign it as the register to use for assignment
					r3 = "$t" + to_string(i);
					tRegister[i] = r1 + " + " + r2;
					break;
				}
			}


			//set commands
			assemblyCommands.push_back("mult " + r1 + ", " + r2);


			assemblyCommands.push_back("mflo " + r3);
			//clear unused register after use
			int mo = r2[2] - '0';

			tRegister[mo] = "";
		}
		//divide
		else if (token.lexeme.compare("/") == 0) {

			//consume token
			token = lexer(file);

			//call the factor if it is factor
			r2 = Factor(file, token);

			//assigns the 3rd register as the next empty register to move the calculated value to
			for (int i = 0; i < 10; i++) {

				if (tRegister[i].compare("") == 0) {
					//assign it as the register to use for assignment
					r3 = "$t" + to_string(i);
					tRegister[i] = r1 + " + " + r2;
					break;
				}
			}

			//set commands
			assemblyCommands.push_back("div " + r1 + ", " + r2);
			assemblyCommands.push_back("mflo " + r3);
			//clear unused register after use
			int mo = r2[2] - '0';
			tRegister[mo] = "";
		}

	}

	//return register holding the value
	return r1;


}


//Grace
string Factor(ifstream& file, LexTok& token) {

	string reg = "$t";
	string in = "";

	//Check if identifier, , realConst, strConst
	if (token.token.compare("Identifier") == 0)
	{
		in = token.lexeme;

		//check if variable is declared
		bool err = false;
		//iterates to check if the variable was declared before being used
		for (vector<string>::iterator it = list.begin(); it != list.end(); it++) {
			if (token.lexeme.compare(*it) != 0) { //if it is not found, then return that error is true
				err = true;
			}
			else { //otherwise, return false for error and break out
				err = false;
				break;
			}
		}
		if (err == true) { //if not found in declared list, then the error is output
			cout << "error: using " << token.lexeme << " without declaring first" << endl;
		}


		//consume token
		token = lexer(file);

		for (int i = 0; i < 10; i++) {
			//if the temp register is empty/not used yet
			if (tRegister[i].compare("") == 0) {
				//assign it as the register to use for assignment
				reg = reg + to_string(i);
				tRegister[i] = in;
				break;
			}
			else if (tRegister[i].compare(in) == 0){
				reg = reg + to_string(i);
				//tRegister[i] = in;
				break;
			}
		}

		//push back the load
		//assemblyCommands.push_back("lw " + reg + ", " + in);


	}
	//check if intConst
	else if (token.token.compare("IntConst") == 0)
	{
		in = token.lexeme;

		//Consume token
		token = lexer(file);

		for (int i = 0; i < 10; i++) {
			//if the temp register is empty/not used yet
			if (tRegister[i].compare("") == 0) {
				//assign it as the register to use for assignment
				reg = reg + to_string(i);
				tRegister[i] = in;
				break;
			}

		}
		//load immediate
		assemblyCommands.push_back("li " + reg + ", " + in);

	}

	else if (token.lexeme.compare("(") == 0)
	{

		expect("(", token, file);
		//Call Expr function
		reg = Expr(file, token);

		expect(")", token, file);
	}
	//returns the register the value was stored in
	return reg;

}

#endif

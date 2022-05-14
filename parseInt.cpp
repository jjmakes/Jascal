#include <vector>
#include <string>
#include "parseInt.h"
#include "val.h"

using namespace std;
map<string, bool> defVar;
map<string, Token> SymTable;
map<string, Value> TempsResults; //Container of temporary locations of Value objects for results of expressions, variables values and constants 
queue <Value> * ValQue; //declare a pointer variable to a queue of Value objects

namespace Parser {
	bool pushed_back = false;
	LexItem	pushed_token;

	static LexItem GetNextToken(istream& in, int& line) {
		if( pushed_back ) {
			pushed_back = false;
			return pushed_token;
		}
		return getNextToken(in, line);
	}

	static void PushBackToken(LexItem & t) {
		if( pushed_back ) {
			abort();
		}
		pushed_back = true;
		pushed_token = t;	
	}

}

static int error_count = 0;

int ErrCount()
{
    return error_count;
}

void ParseError(int line, string msg)
{
	++error_count;
	cout << line << ": " << msg << endl;
}

void printQue(){
	for (size_t i = 0; i < ValQue->size(); i++)
	{
		cout << ValQue->front() << endl;
		ValQue->push(ValQue->front());
		ValQue->pop();
	}
}

void printTemps() {
	for (auto it = TempsResults.cbegin(); it != TempsResults.cend(); ++it) {
		cout << it->first << " : " << it->second << endl;
	}
	return;
}

void printTokens(istream& in, int& line) {
	LexItem t = Parser::GetNextToken(in, line);
	cout << "Token: " << t.GetToken() << " Lexeme: " << t.GetLexeme() << endl;
	while (t.GetToken() != END) {

		cout << "Token: " << t << endl << "Lexeme: " << t.GetLexeme() << endl << endl;
	}
	return;
}

void printVars() {
	for (auto it = defVar.cbegin(); it != defVar.cend(); ++it) {
		cout << it->first << " " << it->second << endl;
	}
	return;
}

void printSyms() {
	for (auto it = SymTable.cbegin(); it != SymTable.cend(); ++it) {
		cout << it->first << " " << it->second << endl;
	}
	return;
}

//IdList:= IDENT {,IDENT}
bool IdentList(istream& in, int& line, vector<string> &IdList) {
	bool status = false;
	string identstr;
	
	LexItem tok = Parser::GetNextToken(in, line);
	if(tok == IDENT)
	{
		identstr = tok.GetLexeme();
		IdList.push_back(identstr);
		if (!(defVar.find(identstr)->second))
		{
			defVar[identstr] = true;
		}	
		else
		{
			ParseError(line, "Variable Redefinition");
			return false;
		}
		
	}
	else
	{
		Parser::PushBackToken(tok);
		return true;
	}
	
	tok = Parser::GetNextToken(in, line);
	
	if (tok == COMMA) {
		status = IdentList(in, line, IdList);
	}
	else if(tok == COLON)
	{
		Parser::PushBackToken(tok);
		return true;
	}
	else {
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	return status;
	
}//End of IdentList


//LogicExpr = Expr (== | <) Expr
bool LogicExpr(istream& in, int& line, Value & retVal)
{
	Value val1, val2;	
	bool t1 = Expr(in, line, val1);
	LexItem tok;
	
	if( !t1 ) {
		return false;
	}
	
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	if ( tok == GTHAN  || tok == EQUAL  || tok == LTHAN)
	{
		t1 = Expr(in, line, val2);
		if( !t1 ) 
		{
			ParseError(line, "Missing expression after relational operator");
			return false;
		}
		
		switch (tok.GetToken()) {
			case GTHAN: {
				Value val((val1 > val2).GetBool());
				retVal = val;
				break; }
			case LTHAN: {
				Value val((val1 < val2).GetBool());
				retVal = val;
				break; }
			case EQUAL: {
				Value val((val1 == val2).GetBool());
				retVal = val;
				break; }
            default:
                break;
		}
		if(retVal.IsErr()){
			ParseError(line, "Run-Time Error-Illegal Mixed Type Operands for a Logic Expression");
			return false;
		}
		return true;
	}

	Parser::PushBackToken(tok);
	return true;
}

//Factor := ident | iconst | rconst | sconst | (Expr)
bool Factor(istream& in, int& line, int sign, Value & retVal) {
	LexItem tok = Parser::GetNextToken(in, line);
	
	if( tok == IDENT ) {
		string lexeme = tok.GetLexeme();
		if (!(defVar.find(lexeme)->second)) 
		{
			ParseError(line, "Using Undefined Variable");
			return false;	
		}

		if ((TempsResults.find(lexeme) == TempsResults.end())) {
			ParseError(line, "Undefined Variable");
			return false;
		}
		else {
			if (sign != 0) {
				Value sVal(sign);
				retVal = sVal * TempsResults[tok.GetLexeme()];
			} else {
			retVal = TempsResults[tok.GetLexeme()];
			}
		}
		
		return true;
	}
	else if( tok == ICONST ) {
		retVal.SetType(VINT);
		retVal.SetInt(stoi(tok.GetLexeme()));
		if (sign != 0) {
			retVal = retVal * sign;
		}
		return true;
	}
	else if( tok == SCONST ) {
		if (sign != 0) {
			ParseError(line, "Illegal Operand Type for Sign Operator");
			return false;
		}
		retVal.SetType(VSTRING);
		retVal.SetString(tok.GetLexeme());
		return true;
	}
	else if( tok == RCONST ) {
		retVal.SetType(VREAL);
		retVal = retVal * sign;
		retVal.SetReal(stof(tok.GetLexeme()));
		if (sign != 0) {
			retVal = retVal * sign;
		}
		return true;
	}
	else if( tok == LPAREN ) {
		bool ex = Expr(in, line, retVal);
		if( !ex ) {
			ParseError(line, "Missing expression after (");
			return false;
		}
		if( Parser::GetNextToken(in, line) == RPAREN )
			return ex;
		else 
		{
			Parser::PushBackToken(tok);
			ParseError(line, "Missing ) after expression");
			return false;
		}
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}

	return false;
}


//SFactor = Sign Factor | Factor
bool SFactor(istream& in, int& line, Value & retVal)
{
	LexItem t = Parser::GetNextToken(in, line);
	bool status;
	int sign = 0;
	if(t == MINUS )
	{
		sign = -1;
	}
	else if(t == PLUS)
	{
		sign = 1;
	}
	else
		Parser::PushBackToken(t);
	status = Factor(in, line, sign, retVal);
	return status;
}


//Term:= SFactor {(*|/) SFactor}
bool Term(istream& in, int& line, Value & retVal) {
	
	Value val1, val2;	
	bool t1 = SFactor(in, line, val1);
	LexItem tok;
	
	if( !t1 ) {
		return false;
	}
	retVal = val1;

	tok	= Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
	}
	while ( tok == MULT || tok == DIV  )
	{
		t1 = SFactor(in, line, val2);
		
		if( !t1 ) {
			ParseError(line, "Missing operand after operator");
			return false;
		}


		if(tok == MULT){
			retVal = retVal * val2;
			if(retVal.IsErr()){
				ParseError(line, "Illegal multiplication operation.");
				return false;
			}
		}
	
		else if(tok == DIV){
			if ((val2 == 0).GetBool()) {
				ParseError(line, "Run-Time Error-Illegal Division by Zero");
				return false;
			}
			retVal = retVal / val2;
			if(retVal.IsErr()){
				ParseError(line, "Illegal division operation.");
				return false;
			}
		}



		
		
		tok	= Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}
	}
	Parser::PushBackToken(tok);
	return true;
}


//Expr:= Term {(+|-) Term}
bool Expr(istream& in, int& line, Value & retVal) {
	Value val1, val2;
	bool t1 = Term(in, line, val1);
	LexItem tok;
	if( !t1 ) {
		return false;
	}
	retVal = val1;

	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	//Evaluate: evaluate the expression for addition or subtraction
	while ( tok == PLUS || tok == MINUS ) 
	{
		t1 = Term(in, line, val2);
		if( !t1 ) 
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}
		
		if(tok == PLUS)
		{
			retVal = retVal + val2;
			if(retVal.IsErr())
			{
				ParseError(line, "Illegal addition operation.");
				return false;
			}
		}
		else if(tok == MINUS)
		{
			retVal = retVal - val2;
			if(retVal.IsErr())
			{
				ParseError(line, "Illegal subtraction operation.");
				return false;
			}
		}
			
		tok = Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}		
		
		
	}
	Parser::PushBackToken(tok);
	return true;
}//end of Expr






bool Prog(istream& in, int& line)
{
	bool f1, f2;
	LexItem tok = Parser::GetNextToken(in, line);
		
	if (tok.GetToken() == PROGRAM) {
		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == IDENT) {
			
			tok = Parser::GetNextToken(in, line);
			if (tok.GetToken() == SEMICOL) {
				f1 = DeclBlock(in, line); 
			
				if(f1) {
					f2 = ProgBody(in, line);
					if(!f2)
					{
						return false;
					}
					return true; //Successful Parsing is completed
				}
				else
				{
					ParseError(line, "Incorrect Declaration Section.");
					return false;
				}
			}
			else
			{
				ParseError(line-1, "Missing Semicolon.");
				return false;
			}
		}
		else
		{
			ParseError(line, "Missing Program Name.");
			return false;
		}
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else if(tok.GetToken() == DONE && tok.GetLinenum() <= 1){
		ParseError(line, "Empty File");
		return true;
	}
	ParseError(line, "Missing PROGRAM.");
	return false;
}

bool ProgBody(istream& in, int& line){
	bool status;
		
	LexItem tok = Parser::GetNextToken(in, line);
	
	if (tok.GetToken() == BEGIN) {
		
		status = Stmt(in, line);
		
		while(status)
		{
			tok = Parser::GetNextToken(in, line);
			if(tok != SEMICOL)
			{
				line--;
				ParseError(line, "Missing semicolon in Statement.");
				return false;
			}
			
			status = Stmt(in, line);
		}
			
		tok = Parser::GetNextToken(in, line);
		if(tok == END )
		{
			return true;
		}
		else 
		{
			ParseError(line, "Syntactic error in Program Body.");
			return false;
		}
	}
	else
	{
		ParseError(line, "Non-recognizable Program Body.");
		return false;
	}	
}//End of ProgBody function


bool DeclBlock(istream& in, int& line) {
	bool status = false;
	LexItem tok;
	LexItem t = Parser::GetNextToken(in, line);
	if(t == VAR)
	{
		status = DeclStmt(in, line);
		
		while(status)
		{
			tok = Parser::GetNextToken(in, line);
			if(tok != SEMICOL)
			{
				line--;
				ParseError(line, "Missing semicolon in Declaration Statement.");
				return false;
			}
			status = DeclStmt(in, line);
		}
		
		tok = Parser::GetNextToken(in, line);
		if(tok == BEGIN )
		{
			Parser::PushBackToken(tok);
			return true;
		}
		else 
		{
			ParseError(line, "Syntactic error in Declaration Block.");
			return false;
		}
	}
	else
	{
		ParseError(line, "Non-recognizable Declaration Block.");
		return false;
	}
	
}//end of DeclBlock function

bool DeclStmt(istream& in, int& line)
{
	LexItem t;
	vector<string> IdList;	
	bool status = IdentList(in, line, IdList);

	if (!status)
	{
		ParseError(line, "Incorrect variable in Declaration Statement.");
		return status;
	}
	
	t = Parser::GetNextToken(in, line);
	if(t == COLON)
	{
		t = Parser::GetNextToken(in, line);
		if(t == INTEGER || t == REAL || t == STRING)
		{
			for (string i: IdList)
				SymTable[i] = t.GetToken();
			return true;
		}
		else
		{
			ParseError(line, "Incorrect Declaration Type.");
			return false;
		}
	}
	else
	{
		Parser::PushBackToken(t);
		return false;
	}
	
}//End of DeclStmt

//WriteStmt:= wi, ExpreList 
bool WriteLnStmt(istream& in, int& line) {
	LexItem t;
	ValQue = new queue<Value>;
	t = Parser::GetNextToken(in, line);
	if( t != LPAREN ) {
		
		ParseError(line, "Missing Left Parenthesis");
		return false;
	}
	
	bool ex = ExprList(in, line);
	
	if( !ex ) {
		ParseError(line, "Missing expression after WriteLn");
		return false;
	}
	
	t = Parser::GetNextToken(in, line);
	if(t != RPAREN ) {
		
		ParseError(line, "Missing Right Parenthesis");
		return false;
	}	
	
	while (!(*ValQue).empty())
	{
		Value nextVal = (*ValQue).front();
		cout << nextVal;
		ValQue->pop();
	}
	cout << endl;

	return ex;
}//End of WriteLnStmt
	

//Stmt = AssigStmt | IfStmt | WriteStmt | ForStmt 
bool Stmt(istream& in, int& line) {
	bool status;
	
	LexItem t = Parser::GetNextToken(in, line);
	
	switch( t.GetToken() ) {

	case WRITELN:
		status = WriteLnStmt(in, line);
		
		break;

	case IF:
		status = IfStmt(in, line);
		break;

	case IDENT:
		Parser::PushBackToken(t);
        status = AssignStmt(in, line);
		
		break;
		
	default:
		Parser::PushBackToken(t);
		return false;
	}

	return status;
}//End of Stmt


//IfStmt:= if (Expr) then Stm} [Else Stmt]
bool IfStmt(istream& in, int& line) {
	bool ex=false, status ; 
	LexItem t;
	
	t = Parser::GetNextToken(in, line);
	if( t != LPAREN ) {
		
		ParseError(line, "Missing Left Parenthesis");
		return false;
	}
	
	Value v; 
			 
	ex = LogicExpr(in, line, v);
	if( !ex ) {
		ParseError(line, "Missing if statement Logic Expression");
		return false;
	}
	
	t = Parser::GetNextToken(in, line);
	if(t != RPAREN ) {
		
		ParseError(line, "Missing Right Parenthesis");
		return false;
	}

	t = Parser::GetNextToken(in, line);
	if(t != THEN)
	{
		ParseError(line, "If-Stmt Syntax Error");
		return false;
	}

	// Cloning the maps to rewrite after stmt.
	map<string, bool> savDefVar(defVar);
	map<string, Token> savSymTable(SymTable);
	map<string, Value> savTempsResults(TempsResults); 

	status = Stmt(in, line);
	
	if (!v.GetBool()) {
		defVar = savDefVar;
		SymTable = savSymTable;
		TempsResults = savTempsResults;
	}
	
	
	
	if(!status)
	{
		ParseError(line, "Missing Statement for If-Stmt Then-Part");
		return false;
	}
	t = Parser::GetNextToken(in, line);
	if( t == ELSE ) {
		
		// Cloning the maps to rewrite after stmt.
		map<string, bool> savDefVar(defVar);
		map<string, Token> savSymTable(SymTable);
		map<string, Value> savTempsResults(TempsResults); 
	
		status = Stmt(in, line);
	
		if (v.GetBool()) {
			defVar = savDefVar;
			SymTable = savSymTable;
			TempsResults = savTempsResults;
		}

		if(!status)
		{
			ParseError(line, "Missing Statement for If-Stmt Else-Part");
			return false;
		}
		return true;
	}
		
	Parser::PushBackToken(t);
	return true;
}//End of IfStmt function

//AssignStmt:= Var = Expr
bool AssignStmt(istream& in, int& line) {
	
	bool varstatus = false, status = false;
	LexItem t = Parser::GetNextToken(in, line); 
	string identstr = t.GetLexeme();

	varstatus = Var(in, line, t);

	
	if (varstatus){
		t = Parser::GetNextToken(in, line);
		if (t == ASSOP){

			Value val;

			status = Expr(in, line, val);

			switch (SymTable.find(identstr)->second) {
				case INTEGER:
					if (!val.IsInt() && !val.IsReal()) {
						//cout << "1" << endl;
						ParseError(line, "Illegal Assignment Operation");
						return false;
					}
					break;
				case REAL:
					if (!val.IsInt() && !val.IsReal()) {
						ParseError(line, "Illegal Assignment Operation");
						return false;
					}
					break;
				case STRING:
					if (!val.IsString()) {
						ParseError(line, "Illegal Assignment Operation");
						return false;
					}
					break;
                default:
                    break;
			}

			if (SymTable.find(identstr)->second == REAL && val.IsInt()) {
				val.SetReal(val.GetInt());
				val.SetType(VREAL);
			}
			else if (SymTable.find(identstr)->second == INTEGER && val.IsReal()) {
				val.SetInt((int)val.GetReal());
				val.SetType(VINT);
			}
			TempsResults[identstr] = val; 

			if(!status) {
				ParseError(line, "Missing Expression in Assignment Statment");
				return status;
			}
			
		}
		else if(t.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << t.GetLexeme() << ")" << endl;
			return false;
		}
		else {
			ParseError(line, "Missing Assignment Operator");
			return false;
		}
	}
	else {
		ParseError(line, "Missing Left-Hand Side Variable in Assignment statement");
		return false;
	}
	return status;	
}


//ExprList:= Expr {,Expr}
bool ExprList(istream& in, int& line) {
	bool status = false;
	Value val1;	
	status = Expr(in, line, val1);
	ValQue->push(val1);	
	if(!status){
		ParseError(line, "Missing Expression");
		return false;
	}
	
	LexItem tok = Parser::GetNextToken(in, line);
	
	if (tok == COMMA) {
		
		status = ExprList(in, line);
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else{
		Parser::PushBackToken(tok);
	return true;
	}
	return status;
}


//Var:= ident
bool Var(istream& in, int& line, LexItem & idTok)
{
	string identstr = idTok.GetLexeme();;
	
	if (idTok == IDENT){
		if (!(defVar.find(identstr)->second))
		{
			ParseError(line, "Undeclared Variable");
			return false;
		}	
		return true;
	}
	else if(idTok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << idTok.GetLexeme() << ")" << endl;
		return false;
	}
	return false;
}//End of Var

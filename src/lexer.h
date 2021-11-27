/*
	Group - 5
	Vikram Waradpande - 2015B4A70454P
	Rinkesh Jain - 2015B4A70590P
	Yajat Dawar - 2015B4A70620P
	Anmol Naugaria - 2015B4A70835P
*/


#include  <stdio.h>
#include "lexerDef.h"

// //Map from an enum of type nonterminal to its string
// char* nonTerminalString[] = {"program","mainFunction","otherFunctions","function","input_par","output_par","parameter_list","dataType","primitiveDatatype",
// "constructedDatatype","remaining_list","stmts","typeDefinitions","typeDefinition","fieldDefinitions","fieldDefinition","moreFields",
// "declarations","declaration","global_or_not","otherStmts","stmt","assignmentStmt","singleOrRecId","new_24","funCallStmt","outputParameters",
// "inputParameters","iterativeStmt","conditionalStmt","elsePart","ioStmt","allVar","arithmeticExpression","expPrime","term","termPrime",
// "factor","highPrecedenceOperators","lowPrecedenceOperators","all","temp","booleanExpression","var","logicalOp","relationalOp","returnStmt",
// "optionalReturn","idList","more_ids","allVarDash"};


// //Map from an enum of type terminal to its string
// char* TerminalString[] = {"TK_ASSIGNOP","TK_COMMENT","TK_FIELDID","TK_ID","TK_NUM","TK_RNUM","TK_FUNID","TK_RECORDID","TK_WITH",
// "TK_PARAMETERS","TK_END","TK_WHILE","TK_TYPE","TK_MAIN","TK_GLOBAL","TK_PARAMETER","TK_LIST","TK_SQL","TK_SQR","TK_INPUT",
// "TK_OUTPUT","TK_INT","TK_REAL","TK_COMMA","TK_SEM","TK_COLON","TK_DOT","TK_ENDWHILE","TK_OP","TK_CL","TK_IF","TK_THEN",
// "TK_ENDIF","TK_READ","TK_WRITE","TK_RETURN","TK_PLUS","TK_MINUS","TK_MUL","TK_DIV","TK_CALL","TK_RECORD","TK_ENDRECORD",
// "TK_ELSE","TK_AND","TK_OR","TK_NOT","TK_LT","TK_LE","TK_EQ","TK_GT","TK_GE","TK_NE","eps","$","TK_ERROR"};


//Hash table for keywords
LinkedList* KeywordTable;
KeywordNode getNode(LinkedList l, char* input);
void pushLL(LinkedList head, KeywordNode node);
TokenInfo getNextToken(FILE* fp);
void getStream();
void populateTransitionTable();
long hash(char* c);
Terminal getTokenEnum(char* lexeme);
int getTransitionIndex(char c);
void populateTransitionTable();
void populateTokenList();
void initializeLexer();
int getIndexToRetractState();

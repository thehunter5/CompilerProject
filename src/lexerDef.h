/*
	Group - 5
	Vikram Waradpande - 2015B4A70454P
	Rinkesh Jain - 2015B4A70590P
	Yajat Dawar - 2015B4A70620P
	Anmol Naugaria - 2015B4A70835P
*/

#ifndef _LEXERDEFINITIONS_
#define _LEXERDEFINITIONS_

#define numStates 58
#define numInputs 31
#define bufferLength 4096
#define keywordHashModulo 111
#define keywordHashMul 31
#define maxTokenLength 15
#define numKeywords 24




typedef enum nonTerminal {program,mainFunction,otherFunctions,function,input_par,output_par,parameter_list,dataType,primitiveDatatype,
constructedDatatype,remaining_list,stmts,typeDefinitions,typeDefinition,fieldDefinitions,fieldDefinition,moreFields,
declarations,declaration,global_or_not,otherStmts,stmt,assignmentStmt,singleOrRecId,new_24,funCallStmt,outputParameters,
inputParameters,iterativeStmt,conditionalStmt,elsePart,ioStmt,allVar,arithmeticExpression,expPrime,term,termPrime,
factor,highPrecedenceOperators,lowPrecedenceOperators,all,temp,booleanExpression,var,logicalOp,relationalOp,returnStmt,
optionalReturn,idList,more_ids,allVarDash} NonTerminal;

//enum for terminals
// Union Name should be Term

typedef enum terminal {TK_ASSIGNOP,TK_COMMENT,TK_FIELDID,TK_ID,TK_NUM,TK_RNUM,TK_FUNID,TK_RECORDID,TK_WITH,
TK_PARAMETERS,TK_END,TK_WHILE,TK_TYPE,TK_MAIN,TK_GLOBAL,TK_PARAMETER,TK_LIST,TK_SQL,TK_SQR,TK_INPUT,
TK_OUTPUT,TK_INT,TK_REAL,TK_COMMA,TK_SEM,TK_COLON,TK_DOT,TK_ENDWHILE,TK_OP,TK_CL,TK_IF,TK_THEN,
TK_ENDIF,TK_READ,TK_WRITE,TK_RETURN,TK_PLUS,TK_MINUS,TK_MUL,TK_DIV,TK_CALL,TK_RECORD,TK_ENDRECORD,
TK_ELSE,TK_AND,TK_OR,TK_NOT,TK_LT,TK_LE,TK_EQ,TK_GT,TK_GE,TK_NE,eps,$,TK_ERROR} Terminal;

typedef enum termOrNonTerm {nonterminal,terminal} TermOrNonTerm;


struct keyNode{
	char* keyword;
	Terminal token;
	struct keyNode* next;
};

//typedef struct keyNode keywordNode;
typedef struct keyNode* KeywordNode;

struct linkedList{
	KeywordNode head;
	//int size;
};

typedef struct linkedList* LinkedList;



typedef struct tInfo{
	char* lexeme;
	Terminal token;
	int lineNumber;
}tokenInfo;

typedef tokenInfo* TokenInfo;



//Buffer Variables
int begin,end,state;
char *buffer1, *buffer2,*inputBuffer;

//
Terminal* tokenList;
int transitionTable[numStates][numInputs];

//flag = 0 => ERROR | flag = 1+x => ACCEPT + x RETRACT
int *stateFlags;


//Current Line Number for error handling

int initFlag;
int retractionflag,checkFlag,overFlag,prevState,prevEnd,prevBegin,lineNumber,numRead;


#endif

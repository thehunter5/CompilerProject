
#ifndef _STDEF_
#define _STDEF_

#include "lexer.h"
#include "parser.h"


#define INT 0
#define REAL 1
#define REC 2
#define INT_WIDTH 2
#define REAL_WIDTH 4

int currFuncIndex;
int numFunctions;
int symTableLen;
int numRecords;
int currRecIndex;
int numGlobals;
int currGlobalIndex;
int yajatNum;

void initializeSymbolTableVariables();
void populateSymbolTable(TreeNode root);
void populateGlobalVars(TreeNode root);
void populateDeclarations(TreeNode root,int currentFunctionIndex);
void populateRecordDefs(TreeNode root);
void allocateSymbolTable();
int checkRecordRedeclaration(char* recordName);
int checkGlobalRedifinition(char* name);
void initializeRecordHashTable();
void insertIntoRecordHashTable(char* recName,int recType);
int getRecordIndex(char* recName);
void handleInputPar(TreeNode root,int currentFunctionIndex);
void handleOutputPar(TreeNode root,int currentFunctionIndex);
int checkVariableRedefinition(int funcIndex,char* string);
int checkGlobalRedeclaration(char* string);
void reallocateSymbolTable(int currentFunctionIndex);
void reallocateRecordTable();
int getFunctionIndex(char* funcName);
void printSymbolTable();
void printMemory();
void printGlobalRecords();
void calculateOffset();
char* getTpString(int type);
void printGlobalTable();

#endif

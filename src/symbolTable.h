/*
 Group - 5
 Vikram Waradpande - 2015B4A70454P
 Rinkesh Jain - 2015B4A70590P
 Yajat Dawar - 2015B4A70620P
 Anmol Naugaria - 2015B4A70835P
*/

#include "symbolTableDef.h"
#include "lexer.h"
#include "parser.h"

struct recTable;

struct symTableEntry{
  TreeNode ASTNode;
  char* symName;
  int type;
  int width;
  short varChanged;
  int offset;
  //Indexing into record table is done by type-2
};

typedef struct symTableEntry* SymTableEntry;

struct funcTable{
  char* funcName;
  int* inputParType;
  int numInputParams;
  int* outputParType;
  int numOutputParams;
  int tableSize;
  SymTableEntry symTable;
  int currIndex;
  int callFlag;
  TreeNode ASTNode;
};
typedef struct funcTable* FuncTable;


// Table for records
struct recordTable{
  char* recordName;
  SymTableEntry recordFields;
  int numFields;
  int recordWidth;
  int recordType;
};

typedef struct recordTable* RecTable;



//Hastable for records
struct recordHashNode
{
  char* recName;
  int recType;
  struct recordHashNode* next;
};
typedef struct recordHashNode* RecordHashNode;


//Global function table
FuncTable* functionTable;
RecTable recordTable;
SymTableEntry globalTable;

//Hashtable for recordtypes
RecordHashNode recordHashTable;
int printErrorFlag;
int semanticCorrectnessFlag;

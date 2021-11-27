/*
	Group - 5
	Vikram Waradpande - 2015B4A70454P
	Rinkesh Jain - 2015B4A70590P
	Yajat Dawar - 2015B4A70620P
	Anmol Naugaria - 2015B4A70835P
*/

#ifndef _PARSERDEFINITIONS_
#define _PARSERDEFINITIONS_

#include "lexerDef.h"

#include <stdio.h>
#include <stdlib.h>

# define MAX 500
# define numNonTerminals 51
# define numTerminals 56
# define MAX 500
# define numRules 92
# define maxRHSIndices 10
# define SYNCH -2


// A union for terminal and non terminal type tokens
typedef union term {
	NonTerminal nt;
	Terminal t;
}Term;


//Structure to represent a grammar rule's symbol
struct rhsNode{
	Term term;
	TermOrNonTerm id;
	Terminal t;
	NonTerminal nt;
	struct rhsNode* next;
};
typedef struct rhsNode* RhsNode;


//Structure to represent a production rule of a grammar
struct production{
	NonTerminal lhs;
	RhsNode head;
};
typedef struct production* Production;

//Grammar is an array of productions
typedef Production* Grammar;

//Global Variable to represent the grammar
Grammar g;


//Structure to represent a first and follow node
struct firstFollowNode{
	RhsNode head;
};
typedef struct firstFollowNode FirstFollowNode;


//Global variables to represent first and follow
FirstFollowNode* Ft;
FirstFollowNode* Fl;


struct leafNode;
struct treeNode;

int* currentIndex;
int ** RHSRuleIndices;
int** ParseTable;
typedef struct leafNode* LeafNode;
typedef struct treeNode* TreeNode;



struct treeNode
{
	TermOrNonTerm id;
	NonTerminal nt;
	Terminal t;
	TreeNode children;
	TreeNode next;
	TokenInfo leafInfo;
	int ruleNum;
	struct treeNode *inh, *syn;
};

typedef struct treeNode* TreeNode;



//Structure to represent a stack node
struct stackNode
{
	struct stackNode* next;
	TermOrNonTerm id;
	TreeNode tn;
};
typedef struct stackNode* StackNode;


//Strucrure to represent a stack
struct stack{
	StackNode top;
};
typedef struct stack* Stack;

//Global variables to represent a buffer and global stack
Stack globalStack;
Stack bufferStack;

//Global variable for
TreeNode root;
int n2,n1;
int syntacticCorrectnessFlag;
#endif

/*
	Group - 5
	Vikram Waradpande - 2015B4A70454P
	Rinkesh Jain - 2015B4A70590P
	Yajat Dawar - 2015B4A70620P
	Anmol Naugaria - 2015B4A70835P
*/


#include "lexer.h"
#include "parserDef.h"


void ComputeFirstAndFollow();
void initializeParser(FILE* fp);
void getGrammar(FILE* fp);
void computeFirst(int nonTerm);
void push(FirstFollowNode* list,int nonTerm, RhsNode t);
void merge(FirstFollowNode* insertInto, FirstFollowNode* insertFrom, int nonTerm, int t, int epsFlag);
int indexNonTermLHS(NonTerminal nt);
int visited[numNonTerminals];
int containsNullProd(int i);
void computeFollow(int nonTerm);
void computeFollowHelper(int RuleNum, int nonTerm);
void printFirstFollow(FirstFollowNode * list, int i);
void createParseTable();
void printParseTable();
void pushStack(Stack s,StackNode n);
void pop(Stack s);
StackNode getTop(Stack s);
void buildTreeAndParse(FILE* fp);
void printInorderTraversal(TreeNode root);
void prettyPrintParseTree(TreeNode root,int height);
void printParseTreeHelper(TreeNode root, FILE* fp, TreeNode parent);
void printParseTree(TreeNode root, char *outfile);
void createAST(TreeNode t);

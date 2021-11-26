#include<stdio.h>
#include "linkedlist.h"
#include "lexerDef.h"



char* getNextToken(FILE* fp);
void getStream();
void populateTransitionTable();
long hash(char* c);
char* getTokenString();
int getTransitionIndex(char c);
void populateTransitionTable();
void populateTokenList();

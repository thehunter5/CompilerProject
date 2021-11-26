#include<stdio.h>


struct keyNode{
	char* keyword;
	char* token;
	struct keyNode* next;
};

//typedef struct keyNode keywordNode;
typedef struct keyNode* KeywordNode;

struct linkedList{
	KeywordNode head;
	//int size;
};

typedef struct linkedList* LinkedList;

void push(LinkedList head, KeywordNode node);
KeywordNode getNode(LinkedList l, char* input);

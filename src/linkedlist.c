#include<stdio.h>
#include<string.h>
#include "linkedlist.h"

void push(LinkedList l, KeywordNode node)
{

	if(l->head == NULL)
		{l->head = node;return;}

	KeywordNode temp = l->head; 

	while(temp->next != NULL)
		temp = temp->next;

	temp->next = node;
}

KeywordNode getNode(LinkedList l, char* input)
{	
	if(l->head == NULL)
		return NULL;

	KeywordNode temp = l->head;
	
	while(temp != NULL)
	{
		if(strcmp(temp->keyword, input) == 0)
			return temp;
		temp = temp->next;
	}

	return NULL;

}
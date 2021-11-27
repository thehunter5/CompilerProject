/*
  Group - 5
  Vikram Waradpande - 2015B4A70454P
  Rinkesh Jain - 2015B4A70590P
  Yajat Dawar - 2015B4A70620P
  Anmol Naugaria - 2015B4A70835P
*/


#include<stdio.h>
#include<stdlib.h>
#include<string.h>


//Map from an enum of type nonterminal to its string
char* nonTerminalString[] = {"program","mainFunction","otherFunctions","function","input_par","output_par","parameter_list","dataType","primitiveDatatype",
"constructedDatatype","remaining_list","stmts","typeDefinitions","typeDefinition","fieldDefinitions","fieldDefinition","moreFields",
"declarations","declaration","global_or_not","otherStmts","stmt","assignmentStmt","singleOrRecId","new_24","funCallStmt","outputParameters",
"inputParameters","iterativeStmt","conditionalStmt","elsePart","ioStmt","allVar","arithmeticExpression","expPrime","term","termPrime",
"factor","highPrecedenceOperators","lowPrecedenceOperators","all","temp","booleanExpression","var","logicalOp","relationalOp","returnStmt",
"optionalReturn","idList","more_ids","allVarDash"};


//Map from an enum of type terminal to its string
char* TerminalString[] = {"TK_ASSIGNOP","TK_COMMENT","TK_FIELDID","TK_ID","TK_NUM","TK_RNUM","TK_FUNID","TK_RECORDID","TK_WITH",
"TK_PARAMETERS","TK_END","TK_WHILE","TK_TYPE","TK_MAIN","TK_GLOBAL","TK_PARAMETER","TK_LIST","TK_SQL","TK_SQR","TK_INPUT",
"TK_OUTPUT","TK_INT","TK_REAL","TK_COMMA","TK_SEM","TK_COLON","TK_DOT","TK_ENDWHILE","TK_OP","TK_CL","TK_IF","TK_THEN",
"TK_ENDIF","TK_READ","TK_WRITE","TK_RETURN","TK_PLUS","TK_MINUS","TK_MUL","TK_DIV","TK_CALL","TK_RECORD","TK_ENDRECORD",
"TK_ELSE","TK_AND","TK_OR","TK_NOT","TK_LT","TK_LE","TK_EQ","TK_GT","TK_GE","TK_NE","eps","$","TK_ERROR"};


#include "parser.h"


//Sets up all data structures and variables required for parser


void initializeParser(FILE* fp)
{

  //Initialize Grammar Production Rules
  g = (Grammar)malloc(sizeof(Production)*numRules);
  syntacticCorrectnessFlag = 1;
	RHSRuleIndices = (int**)malloc(numNonTerminals*sizeof(int*));
  currentIndex = (int*)malloc(sizeof(int)*numNonTerminals);

  for(int i=0;i<numNonTerminals;i++)
    currentIndex[i] = 0;

  for(int i=0;i<numNonTerminals;i++)
      {
        RHSRuleIndices[i] = (int*)malloc(sizeof(int)*maxRHSIndices);
        for(int j=0;j<maxRHSIndices;j++)
          RHSRuleIndices[i][j] = -1;
      }

  for(int i=0;i<numNonTerminals;i++)
    visited[i] = 0;


  //Variables for symbol table printing and evaluation use
  n1 = n2 = 0;

  //Parse the grammar and make liked lists corresponding to each rule
   getGrammar(fp);


  //Definitions and delcations for First and Follow computation
  Ft = (FirstFollowNode*)malloc(numNonTerminals*sizeof(FirstFollowNode));
  for(int i=0;i<numNonTerminals;i++)
    Ft->head = NULL;

  Fl = (FirstFollowNode*)malloc(numNonTerminals*sizeof(FirstFollowNode));
  for(int i=0;i<numNonTerminals;i++)
    Fl->head = NULL;


  RhsNode dollar = (RhsNode)malloc(sizeof(struct rhsNode));
  dollar->t = $;
  dollar->next = NULL;
  dollar->id = terminal;

  push(Fl,0,dollar);
  ComputeFirstAndFollow();

  //Definitions and delcations for Predictive Parsing Table
  ParseTable = (int**)malloc(sizeof(int*)*numNonTerminals);
  for(int i=0;i<numNonTerminals;i++)
    {
      ParseTable[i] = (int*)malloc(sizeof(int)*numTerminals);
      for(int j=0;j<numTerminals;j++)
        ParseTable[i][j] = -1;
    }

  createParseTable();


  globalStack = (Stack)malloc(sizeof(struct stack));
  globalStack->top = NULL;

  bufferStack = (Stack)malloc(sizeof(struct stack));
  bufferStack->top = NULL;

  //Start Symbol
  root = (TreeNode)malloc(sizeof(struct treeNode));
  root->id = nonterminal;
  root->nt = program;
  root->children = NULL;
  root->next = NULL;
	root->leafInfo = NULL;
  root->ruleNum = 0;

  StackNode init = (StackNode)malloc(sizeof(struct stackNode));
  init->tn = root;
  init->next = NULL;
  init->id = nonterminal;
  pushStack(globalStack,init);

}



//Get the index of the rule in whose LHS the nonterminal 'nt' appears
int indexNonTermLHS(NonTerminal nt)
{
  for(int i=0;i<numRules;i++)
    if(g[i]->lhs == nt)
      return i;
  return -1;
}


//Check if the nonterminal at index i is nullable
int containsNullProd(int i)
{
  RhsNode temp = Ft[i].head;
  while(temp!=NULL)
  {
    if(temp->t == eps)
      return 1;
    temp = temp->next;
  }
  return 0;
}


//Print First and Follow sets
void printFirstFollow(FirstFollowNode * list, int i){
  printf("%s: ", nonTerminalString[i]);
  RhsNode temp = list[i].head;
  while(temp!=NULL)
  {
    printf("%s ",TerminalString[temp->t]);
    temp = temp->next;
  }
  printf("\n");
}


//Compute First and Follow sets one after the other
void ComputeFirstAndFollow()
{
	//printf("--------------------First Set------------------\n");
  for(int i=0;i<numNonTerminals;i++)
		{
      if(visited[i]==0)
			   computeFirst(i);
       //printFirstFollow(Ft,i);
    }


  printf("\n\n");

  //Reset Visited rules for Follow
  for(int i=0;i<numNonTerminals;i++)
    visited[i] = 0;

  //printf("--------------------Follow Set------------------:\n");
  for(int i=0;i<numNonTerminals;i++)
  {  if(visited[i]==0)
        computeFollow(i);
      //printFirstFollow(Fl,i);
  }

  //}
}


//Compute Follow by calling a helper function for each nonterminal
void computeFollow(int nonTerm)
{
    if(visited[nonTerm]==1)
      return;

    visited[nonTerm] = 1;
    for(int i=0;i<currentIndex[nonTerm];i++)
    {
        computeFollowHelper(RHSRuleIndices[nonTerm][i], nonTerm);
    }
}


//Compute follow of a nonterminal recursively by traversing the rules
void computeFollowHelper(int RuleNum, int nonTerm)
{
  RhsNode temp = g[RuleNum]->head;

  //Flag to check if we encountered the nonterminal yet
  int sameNonTerm = 0;

  while(temp!=NULL)
  {
    if(sameNonTerm == 1)
    {
      //Encounter a Terminal
      if(temp->id == terminal)
        {
          push(Fl,nonTerm,temp);
          sameNonTerm = 0;
          temp = temp->next;
          continue;
        }

      //If it is a non terminal, merge the first of temp->term
      merge(Fl,Ft,nonTerm,temp->nt,0);

      //If this nonterminal doesn't go to eps, reset the flag
      if(!containsNullProd(temp->nt))
        sameNonTerm = 0;

    }
    else if(temp->id == terminal || (temp->id == nonterminal && nonTerm != temp->nt))
    {
      temp = temp->next;
      continue;
    }

    if(temp->id == nonterminal && nonTerm == temp->nt)
      sameNonTerm = 1;

    /*Recursive follow case:
        1) The last nonterminal goes to eps
        2) The last nonterminal is the one whose follow is to be computed
    */

    if(temp->next == NULL && ((sameNonTerm==1) || (nonTerm == temp->nt && temp->id == nonterminal)))
      {

        //If follow isn't computed yet
        if(Fl[g[RuleNum]->lhs].head == NULL)
          computeFollow(g[RuleNum]->lhs);

        merge(Fl,Fl,nonTerm,g[RuleNum]->lhs,0);
        return;
      }

    //We encountered the non-terminal whose follow is to be computed
    //sameNonTerm = 1;

    temp = temp->next;
  }
}



//Compute First set for each nonterminal
void computeFirst(int nonTerm)
{

	visited[nonTerm] = 1;
	int temp = 0;

    while(1)
  	{
  		int TermFlag = 0;
      if(g[indexNonTermLHS(nonTerm)+temp]->lhs != nonTerm)
  			return;

  		RhsNode rhs = g[indexNonTermLHS(nonTerm)+temp]->head;

      while(rhs!=NULL)
      {

      if(rhs->id == terminal)
  			{
          push(Ft,nonTerm,rhs);
          TermFlag = 1;
          break;
        }

  				if(Ft[rhs->nt].head == NULL)
  					computeFirst(rhs->nt);

          int epsFlag = 0;
          if(rhs->next == NULL)
            epsFlag = 1;
  				merge(Ft,Ft,nonTerm,rhs->nt,epsFlag);

  				if(!containsNullProd(rhs->nt))
  					break;
  				rhs = rhs->next;
  		}

  		temp++;
      if(indexNonTermLHS(nonTerm)+temp>=numRules)
            break;

  	}
}


//Push the RhsNode 't' into the Linkedlist 'list'
void push(FirstFollowNode* list, int nonTerm,RhsNode t)
{

	if(list[nonTerm].head == NULL)
	{
		list[nonTerm].head = (RhsNode)malloc(sizeof(struct rhsNode));
		list[nonTerm].head->t = t->t;
		list[nonTerm].head->id = terminal;
    list[nonTerm].head->next = NULL;
		return;
	}

	RhsNode temp = list[nonTerm].head;

  if(temp->t == t->t)
      return;

	while(temp->next != NULL)
	{
		temp = temp->next;
    if(temp->t == t->t)
      return;
	}
	temp->next = (RhsNode)malloc(sizeof(struct rhsNode));
	temp->next->t = t->t;
	temp->id = terminal;
  temp->next->next = NULL;
}




//Merge the list insertInto[nonTerm] and insertFrom[t]
void merge(FirstFollowNode* insertInto, FirstFollowNode* insertFrom, int nonTerm, int t, int epsFlag)
{

  RhsNode temp = insertFrom[t].head;
	while(temp!=NULL)
		{
			if(epsFlag == 0 && temp->t==eps)
        {temp = temp->next;continue;}

      push(insertInto,nonTerm,temp);
			temp = temp->next;
		}
}



//Return a set of tokens from a string separated by spaces
int parseWithSpaces(char* string, char** parsed)
{

	int len  = strlen(string);
	char* str = (char*)malloc(sizeof(char)*MAX);
	strcpy(str,string);

 	int i;

	for (i = 0; i < MAX; i++) {
		parsed[i] = strsep(&str, " ");

		if (parsed[i] == NULL)
			{
				break;
			}
		if (strlen(parsed[i]) == 0)
			i--;
	}

	return i;
}


//Get the enum of terminal from a string
int getIndexNonTerminal(char* string)
{
	for(int i=0;i<numNonTerminals;i++)
	{
		if(strcmp(string,nonTerminalString[i])==0)
			return i;
	}
	return -1;
}


//Get the enum of nonterminal from a string
int getIndexTerminal(char* string)
{
	for(int i=0;i<numTerminals;i++)
	{
		if(strcmp(string,TerminalString[i])==0)
			return i;
	}
	return -1;
}


//Make data structures to represent grammar from the file given by the file pointer fp
void getGrammar(FILE* fp)
{

	if (fp==NULL)
      {
          printf("no such file \n");
          return;
      }

 	int ruleIndex = 0;
 	char line[MAX];

 	while(fgets(line, MAX, fp)!=NULL)
 	{
 		line[strlen(line)-2] = '\0';

 		char* parsed[MAX];
 		int size = parseWithSpaces(line,parsed);

 		g[ruleIndex] = (Production)malloc(sizeof(struct production));

 		g[ruleIndex]->head = NULL;

 		for(int i=0;i<size;i++)
 		{
 				int len = strlen(parsed[i]);

 				if(parsed[i][0]=='-')
 					continue;

 				if(parsed[i][0]=='<' && parsed[i][len-1]=='>')
 				{
 					// case of Non Terminal
 					parsed[i][len-1] = '\0';
 					parsed[i]++;

 					int ind = getIndexNonTerminal(parsed[i]);

 					if(i==0)
 					{
 						// LHS
 						g[ruleIndex]->lhs = ind;
 					}
 					else
 					{
 						// RHS
 						RhsNode r = (RhsNode)malloc(sizeof(struct rhsNode));
  					RhsNode temp = g[ruleIndex]->head;

            //For follow computation
            RHSRuleIndices[ind][currentIndex[ind]] = ruleIndex;
            currentIndex[ind]++;

						if(temp == NULL){
  							g[ruleIndex]->head = r;
  						  r->nt = ind;
    				    r->id = nonterminal;
    				    r->next = NULL;
						}
						else
						{
    						while(temp->next != NULL)
    							 temp = temp->next;

    						temp->next = r;
  						  r->nt = ind;
	  				    r->id = nonterminal;
	  				    r->next = NULL;

  				    }
 					  }

 				 }
 				else
 				{
 					// case of Terminal
 					int ind = getIndexTerminal(parsed[i]);

					  RhsNode r = (RhsNode)malloc(sizeof(struct rhsNode));
					RhsNode temp = g[ruleIndex]->head;

						if(temp == NULL){

							 g[ruleIndex]->head = r;
						   r->t = ind;
  				     r->id = terminal;
  				     r->next = NULL;
						}
						else
						{
    						while(temp->next != NULL)
    							temp = temp->next;

    						temp->next = r;
    						r->t = ind;
  	  				  r->id = terminal;
  	  				  r->next = NULL;
  				    }

				}
 		}
  ruleIndex++;
 	}
}



//Generate a predictive parsing table from the given Terminals, Nonterminals, First and Follow sets
void createParseTable()
{
  //Iterate over rules
  for(int i=0;i<numRules;i++)
  {
    int ntIndex = g[i]->lhs;
    RhsNode rhsTerm = g[i]->head;

    //Flag to check if the current nonterminal is nullable
    int epsFlag = 0;
    while(rhsTerm!=NULL)
    {
      epsFlag = 0;
      if(rhsTerm->id == terminal)
      {
        if(rhsTerm->t == eps)
        {
          epsFlag = 1;
          break;
        }
        ParseTable[ntIndex][rhsTerm->t] = i;
        break;
      }

      RhsNode firstNode = Ft[rhsTerm->nt].head;

      while(firstNode!=NULL)
      {

          if(firstNode->t == eps)
          {
            epsFlag=1;
            firstNode = firstNode->next;
            continue;
          }

        ParseTable[ntIndex][firstNode->t] = i;
        firstNode = firstNode->next;

      }
      if(epsFlag == 0)
        break;
      rhsTerm = rhsTerm->next;
    }

    if(epsFlag == 1)
    {
      RhsNode temp =Fl[ntIndex].head;

      while(temp!=NULL)
      {

        ParseTable[ntIndex][temp->t] = i;
        temp=temp->next;
      }
    }
  }

  for(int i=0;i<numNonTerminals;i++)
  {
    RhsNode flNode = Fl[i].head;
    while(flNode!=NULL)
    {
      if(ParseTable[i][flNode->t] < 0)
        ParseTable[i][flNode->t] = SYNCH;
      flNode = flNode->next;
    }
  }

  /*
    TODO : Add manually curated rules like 'while endwhile'
  */

  //printParseTable();

}

//Print parse table column*row
void printParseTable(){

    for(int i=0;i<numNonTerminals;i++)
    {
      for(int j=0;j<numTerminals;j++)
        printf("%d ",ParseTable[i][j]);
      printf("\n");
    }
}



//Push a stacknode 'n' into stack 's'
void pushStack(Stack s,StackNode n)
{
  if(s->top ==NULL)
  {
    s->top = n;
    n->next = NULL;
  }
  else
  {
    StackNode temp = s->top;
    n->next = temp;
    s->top = n;
  }
}


//Pop a node from stack 's'
void pop(Stack s)
{
  if(s->top==NULL)
  {
    printf("Stack is Empty\n");
    return ;
  }
  StackNode temp=s->top;

  s->top = (s->top)->next;
  temp->next  = NULL;
}


//Extract the top node from stack s
StackNode getTop(Stack s)
{
  if(s->top==NULL){
    printf("Stack is Empty\n");
    return NULL;
  }
  return s->top;
}



//Print stack from top to bottom
void printStack(Stack s)
{
  StackNode temp = s->top;
  while(temp!=NULL)
  {
    if(temp->tn->id != terminal)
        printf("%s ",nonTerminalString[temp->tn->nt]);
    else
      printf("%s ",TerminalString[temp->tn->t]);

    temp = temp->next;
  }
  printf("\n");
}



void buildTreeAndParse(FILE* fp){

  TokenInfo newToken = getNextToken(fp);

  //While the new token is error or comment, skip it
  while(newToken->token == TK_COMMENT || newToken->token == TK_ERROR)
    {
      if(newToken->token == TK_ERROR)
      {
        printf("Line %d: Unknown symbol %s\n",newToken->lineNumber,TerminalString[newToken->token]);
        syntacticCorrectnessFlag = 0;
      }
      newToken = getNextToken(fp);
    }

  //Start parsing after getting a valid symbol
  while(getTop(globalStack) != NULL)
  {
    // printf("Lookahead: %s\n",newToken->lexeme);
    // printf("Global Stack : ");
    // printStack(globalStack);

    StackNode top = getTop(globalStack); //Current stacktop

    TreeNode currentRoot = top->tn; //Current treenode

    //If stack top has a terminal
    if(top->tn->id == terminal)
    {
      //If the terminal matches with lookahead
      if(top->tn->t == newToken->token)
       {
        //Get a new token from the stream
				top->tn->leafInfo = newToken;
				newToken = getNextToken(fp);

        //printf("Token: %s\n",TerminalString[newToken->token]);

        //If stream is empty and stack is also empty, it means parsing is successful
        if(newToken == NULL && top->tn->next == NULL)
          {
            //printf("Parsing successful: Program is syntactically correct.");
            pop(globalStack);
            return;
          }


        while(overFlag == 1 || newToken == NULL || newToken->token == TK_COMMENT || newToken->token == TK_ERROR)
        {

          if(newToken == NULL || overFlag == 1)
          {
            //printf("Program is syntactically incorret. Kindly recheck!\n");
            syntacticCorrectnessFlag = 0;
            return;
          }

          if(newToken->token == TK_ERROR)
            {
              printf("Line %d: Unknown symbol %s\n",newToken->lineNumber,newToken->lexeme);
              syntacticCorrectnessFlag = 0;
            }

          newToken = getNextToken(fp);
        }


        pop(globalStack);
        // printf("Global Stack : ");
        // printStack(globalStack);
        continue;
       }

       syntacticCorrectnessFlag = 0;
       //Terminal Terminal Mismatch Case
       /*
          *If the stack top has a terminal and it doesn't match the lookahead, keep popping
          *the global stack till stack has a nonterminal or stacktop matches the lookahead
       */
       printf("Line %d: Missing symbol %s\n",newToken->lineNumber,TerminalString[top->tn->t]);
       //printf("Line %d: The token %s for lexeme %s does not match with the expected token %s\n",newToken->lineNumber,TerminalString[newToken->token],newToken->lexeme,TerminalString[top->tn->t]);
       pop(globalStack);


       if(globalStack->top->tn->id == terminal)
        continue;

       //Reset stack top in case top is a nonterminal
       top = getTop(globalStack);

    }
    // printf("Lookahead: %s\n",newToken->lexeme);
    // printf("Global Stack : ");
    // printStack(globalStack);

    //Stacktop is non-terminal

    int ruleNum = ParseTable[top->tn->nt][newToken->token];


    //Error Handling using Synch
    if(ruleNum < 0)
    {
      syntacticCorrectnessFlag = 0;
      //Synch case
      if(ruleNum == SYNCH)
      {
        pop(globalStack);
        //printf("Synch Case\n");
        continue;
      }

      int printErrorFlag = 0;
      while(ParseTable[top->tn->nt][newToken->token] < 0)
      {

        newToken = getNextToken(fp);

        while(newToken == NULL || overFlag == 1 || newToken->token == TK_COMMENT || newToken->token==TK_ERROR)
        {

          if(newToken == NULL || overFlag == 1)
          {
            printf("Program is syntactically incorret. Kindly recheck!\n");
            return;
          }

          if(newToken->token == TK_ERROR)
            printf("Line %d: Unknown symbol %s\n",newToken->lineNumber,newToken->lexeme);

          newToken = getNextToken(fp);
        }

        //printf("New token in NT-T case: %s\n",TerminalString[newToken->token]);

        ruleNum = ParseTable[top->tn->nt][newToken->token];
        //printf("%d\n",ruleNum);
        if(ruleNum == SYNCH)
          break;
        printErrorFlag = 1;

        if(printErrorFlag == 0)
          printf("Line %d: The token %s does not match the expected type %s\n",newToken->lineNumber, TerminalString[newToken->token], nonTerminalString[top->tn->nt]);
        //printf("Line %d: Missing symbol %s\n",newToken->lineNumber, TerminalString[newToken->token], nonTerminalString[top->tn->nt]);
      }

      if(ruleNum == SYNCH)
      {
        pop(globalStack);
        continue;
      }

    }

    top->tn->ruleNum = ruleNum;
    RhsNode temp = g[ruleNum]->head;

    while(temp!=NULL)
    {

      // Make Tree Node Here
      TreeNode tn;
      StackNode s;

      tn = (TreeNode)malloc(sizeof(struct treeNode));
      s = (StackNode)malloc(sizeof(struct stackNode));

      if(temp->id == terminal)
      {
          tn->id = terminal;
          tn->t = temp->t;
          tn->leafInfo = NULL;
          //printf("lexToken : %s\n",TerminalString[(temp->term).t]);
          //(tn.l)->lexToken = newToken->token;
          //Value Computation
      }
      else
      {
          tn->id = nonterminal;
          tn->nt = temp->nt;
					tn->leafInfo = NULL;
      }

      tn->children = NULL;
      tn->next = NULL;
      s->next = NULL;
      s->tn = tn;


      pushStack(bufferStack,s);

       temp = temp->next;
    }

    TreeNode prev;
    prev = NULL;

    pop(globalStack);

    while(bufferStack->top!=NULL)
    {

      if(bufferStack->top->next == NULL)
        break;

      TreeNode topNode = bufferStack->top->tn;
      topNode->next = prev;
      prev = topNode;

      StackNode temp = getTop(bufferStack);


      pop(bufferStack);

      pushStack(globalStack,temp);
      temp = NULL;

    }

    currentRoot->children = bufferStack->top->tn;
    currentRoot->children->next = prev;

    if(!( bufferStack->top->tn->id==terminal  && bufferStack->top->tn->t == eps) )
      {

        StackNode temp = getTop(bufferStack);
        pop(bufferStack);
        pushStack(globalStack,temp);

        continue;
      }

    pop(bufferStack);
  }

  syntacticCorrectnessFlag = 0;
  //printf("The program is syntactically incorrect. Kindly recheck!\n");



return;
}


void prettyPrintParseTree(TreeNode root,int height)
{
  for(int i=0;i<height;i++)
    printf(" | ");

  if(root==NULL)
    return ;

  if(root->id==terminal){

    printf("%s\n", TerminalString[root->t]);
    //return ;
  }

  else{

    printf("%s\n",nonTerminalString[root->nt]);

  }

  TreeNode temp = root->children;

  while(temp!=NULL)
  {
    prettyPrintParseTree(temp,height+1);
    temp = temp->next;
  }
}




void printParseTreeHelper(TreeNode root, FILE* fp, TreeNode parent)
{

  if(root==NULL)
    return ;

  printParseTreeHelper(root->children,fp,root);

  if(root->id==terminal){
			if(root->leafInfo == NULL)
				fprintf(fp,"%s   ERROR CASE lineNumber =       parent -    isLeafNode = yes \n\n",TerminalString[root->t]);
			else
        fprintf(fp,"%s    lineNumber = %d    %s    parent - %s    isLeafNode = yes \n\n",root->leafInfo->lexeme,root->leafInfo->lineNumber,TerminalString[root->leafInfo->token],nonTerminalString[parent->nt]);

			return;
  }

  else{

        if(parent==NULL)
        {
          fprintf(fp,"---- %s    isLeafNode = no\n\n",nonTerminalString[root->nt]);
        }
        else
        fprintf(fp,"---- %s    parent - %s    isLeafNode = no \n\n",nonTerminalString[root->nt],nonTerminalString[parent->nt]);

  }


  TreeNode temp = root->children;

  while(temp!=NULL)
  {
    temp = temp->next;
    printParseTreeHelper(temp,fp,root);
  }


}
void printParseTree(TreeNode root, char *outfile){

  //char* temp = "./";
  //strcat(temp,outfile);

  FILE *fptr = fopen(outfile, "w");

  if (fptr == NULL)
    {
        printf("Could not open file");
        return;
    }

    printParseTreeHelper(root,fptr,NULL);
    fclose(fptr);

}

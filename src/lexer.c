#include<stdio.h>
#include<stdlib.h>
#include "lexer.h"

char* keywords[] = {"with","parameters","end","while","type","_main","global","parameter","list","input","output","int","real","endwhile","if","then","endif","read","write","return","call","record","endrecord","else"};
char* tokens[] = {"TK_WITH","TK_PARAMETERS","TK_END","TK_WHILE","TK_TYPE","TK_MAIN","TK_GLOBAL","TK_PARAMETER","TK_LIST","TK_INPUT","TK_OUTPUT","TK_INT","TK_REAL","TK_ENDWHILE","TK_IF","TK_THEN","TK_ENDIF","TK_READ","TK_WRITE","TK_RETURN","TK_CALL","TK_RECORD","TK_ENDRECORD","TK_ELSE"};


//Get the token string corresponding to the lexeme
char* getTokenString(char* lexeme)
{
	if(state == 2)
	{
		KeywordNode k = getNode(KeywordTable[hash(lexeme)],lexeme);
		if(k==NULL)
			return "TK_FIELDID";

		return k->token;
	}

	if(state ==6)
	{
		if(end-begin+1 <= 20)
			return tokenList[6];
		return "TK_ERROR";
	}

	if(state==13)
	{
		KeywordNode k = getNode(KeywordTable[hash(lexeme)],lexeme);
		
		if(k==NULL)
		{
			if(end-begin+1 <= 30)
				return tokenList[13];
			return "TK_ERROR";	
		}

		return k->token;

		

	}

	return tokenList[state];		
}


//Hash using Horner's rule
long hash(char* c)
{
	long temp = 0;
	for(int i=0;;i++)
	{
		if(c[i]=='\0')
			break;
		temp = temp*keywordHashMul + c[i];
		temp = temp%keywordHashModulo;
	}
	return temp;
}


//Get index in the transition table corresponding to the input
int getTransitionIndex(char c)
{
	if(c == '0' || c == '1' || c == '8' || c == '9')
		return 0;
	if('2'<=c && c<='7')
		return 1;

	if('A'<=c && c<='Z')
		return 2;

	if(c=='a' || ('e'<=c && c<='z'))
		return 3;

	if('b'<=c && c<='d')
		return 4;

	switch(c){
		case '<':
			return 5;
		case '-':
			return 6;
		case '%':
			return 7;
		case '_':
			return 8;
		case '#':
			return 9;
		case '[':
			return 10;
		case ']':
			return 11;
		case '.':
			return 12;
		case ',':
			return 13;
		case ';':
			return 14;
		case ':':
			return 15;
		case '(':
			return 16;
		case ')':
			return 17;
		case '+':
			return 18;
		case '*':
			return 19;
		case '/':
			return 20;
		case '&':
			return 21;
		case '@':
			return 22;
		case '~':
			return 23;
		case '>':
			return 24;
		case '=':
			return 25;
		case '!':
			return 26;
		case ' ':
			return 27;
		case '\n':
			return 28;
		case '\t':
			return 29;
		default :
			return -1;

	}

}

//Generate table for tokens
void populateTokenList()
{
	for(int i=0;i<numStates;i++)
		tokenList[i] = (char*)malloc(maxTokenLength*sizeof(char));
		

	tokenList[6] = "TK_ID";
	tokenList[9] = "TK_RECORDID";
	tokenList[13] = "TK_FUNID";
	tokenList[16] = "TK_OR";
	tokenList[19] = "TK_AND";
	tokenList[21] = "TK_NUM";
	tokenList[24] = "TK_RNUM";
	tokenList[28] = "TK_ASSIGNOP";
	tokenList[29] = "TK_LT";
	tokenList[30] = "TK_LT";
	tokenList[31] = "TK_LE";
	tokenList[32] = "TK_LT";
	tokenList[34] = "TK_GT";
	tokenList[35] = "TK_GE";
	tokenList[37] = "TK_EQ";
	tokenList[39] = "TK_NE";
	tokenList[43] = "TK_COMMENT";
	tokenList[44] = "TK_DIV";
	tokenList[45] = "TK_MUL";
	tokenList[46] = "TK_MINUS";
	tokenList[47] = "TK_PLUS";
	tokenList[48] = "TK_SQL";
	tokenList[49] = "TK_SQR";
	tokenList[50] = "TK_OP";
	tokenList[51] = "TK_CL";
	tokenList[52] = "TK_COMMA";
	tokenList[53] = "TK_SEM";
	tokenList[54] = "TK_COLON";
	tokenList[55] = "TK_DOT";
	tokenList[56] = "TK_NOT";

}

//Make hash table for keywords
void populateKeywordTable()
{


	for(int i=0;i<numKeywords;i++)
	{
		KeywordNode temp = (KeywordNode)malloc(sizeof(struct keyNode));
		temp->token = tokens[i];
		temp->keyword = keywords[i];
		push(KeywordTable[hash(keywords[i])],temp);
	}
}


//Set up data structures and variables for lexer
void initializeLexer()
{
	state = 0;
	buffer1 = (char*)malloc(bufferLength*sizeof(char));
	buffer2 = (char*)malloc(bufferLength*sizeof(char));
	inputBuffer = buffer1;
	tokenList = (char**)malloc(numStates*sizeof(char*));
	begin = end = 0;
	stateFlags = (int*)malloc(numStates*sizeof(int));
	KeywordTable = (LinkedList*)malloc(keywordHashModulo*sizeof(LinkedList));

	for(int i=0;i<keywordHashModulo;i++)
		{
			KeywordTable[i] = (LinkedList)malloc(sizeof(struct linkedList));
		}

	for(int i=0;i<numStates;i++)
		{
			stateFlags[i] = 0;
		
			for(int j=0;j<numInputs;j++)		
			 	transitionTable[i][j] = -1;
   		}

   	populateTransitionTable();
   	populateTokenList();
   	populateKeywordTable();

}

//Define transitions for the DFA
void populateTransitionTable()
{
	/*
	TODO:
	* For each state set the stateFlags properly
	* For each state's every transition, fill the transition accordingly
	* Handle states with 'others' transition separately
	*/
	//State 0
	transitionTable[0][0] = 20;
	transitionTable[0][1] = 20;
	transitionTable[0][3] = 1;
	transitionTable[0][4] = 3;
	transitionTable[0][5] = 25;
	transitionTable[0][6] = 46;
	transitionTable[0][7] = 42;
	transitionTable[0][8] = 10;
	transitionTable[0][9] = 7;
	transitionTable[0][10] = 48;
	transitionTable[0][11] = 49;
	transitionTable[0][12] = 55;
	transitionTable[0][13] = 52;
	transitionTable[0][14] = 53;
	transitionTable[0][15] = 54;
	transitionTable[0][16] = 50;
	transitionTable[0][17] = 51;
	transitionTable[0][18] = 47;
	transitionTable[0][19] = 45;
	transitionTable[0][20] = 44;
	transitionTable[0][21] = 17;
	transitionTable[0][22] = 14;
	transitionTable[0][23] = 56;
	transitionTable[0][24] = 33;
	transitionTable[0][25] = 36;
	transitionTable[0][26] = 38;
	transitionTable[0][27] = 40;
	transitionTable[0][28] = 40;
	transitionTable[0][29] = 40;
	
	
	stateFlags[0] = 1;
	
	//State 1
	transitionTable[1][3] = 1;
	transitionTable[1][4] = 1;
	for(int i=0;i<numInputs;i++)
		if(i!=3 && i!=4)
			transitionTable[1][i] = 2;
	
	stateFlags[1] = 1;
	
	//State 2
	stateFlags[2] = 3;
	
	//State 3
	transitionTable[3][3] = 1;
	transitionTable[3][4] = 1;
	transitionTable[3][1] = 4;
	for(int i=0;i<numInputs;i++)
		if(i!=1 && i!=3 && i!=4)
			transitionTable[3][i] = 2;
		
	stateFlags[3] = 1;
	
	//State 4
	transitionTable[4][1] = 5;
	transitionTable[4][4] = 4;
	for(int i=0;i<numInputs;i++)
		if(i!=1 && i!=4)
			transitionTable[4][i] = 6;
		
	stateFlags[4] = 1;
	
	//State 5
	transitionTable[5][1] = 5;
	for(int i=0;i<numInputs;i++)
		if(i!=1)
			transitionTable[5][i] = 6;
	
	stateFlags[5] = 1;
	
	//State 6
	stateFlags[6] = 3;
	
	//State 7
	transitionTable[7][3] = 8;
	transitionTable[7][4] = 8;
	
	stateFlags[7] = 1;
	
	//State 8
	transitionTable[8][3] = 8;
	transitionTable[8][4] = 8;
	for(int i=0;i<numInputs;i++)
		if(i!=3 && i!=4)
			transitionTable[8][i] = 9;
	
	stateFlags[8] = 1;
	
	//State 9
	stateFlags[9] = 3;

	//State 10
	transitionTable[10][3] = 11;
	transitionTable[10][4] = 11;
	transitionTable[10][2] = 11;
	
	stateFlags[10] = 1;
	
	//State 11
	transitionTable[11][3] = 11;
	transitionTable[11][4] = 11;
	transitionTable[11][2] = 11;
	transitionTable[11][0] = 12;
	transitionTable[11][1] = 12;
	for(int i=5;i<numInputs;i++)
		transitionTable[11][i] = 13;
	
	stateFlags[11] = 1;
	
	//State 12
	transitionTable[12][0] = 12;
	transitionTable[12][1] = 12;
	for(int i=2;i<numInputs;i++)
		transitionTable[12][i] = 13;
	
	stateFlags[12] = 1;
	
	//State 13
	stateFlags[13] = 3;
	
	//State 14
	transitionTable[14][22] = 15;
	
	stateFlags[14] = 1;
	
	//State 15
	transitionTable[15][22] = 16;
	
	stateFlags[15] = 1;
	
	//State 16
	stateFlags[16] = 2;
	
	//State 17
	transitionTable[17][21] = 18;
	
	stateFlags[17] = 1;
	
	//State 18
	transitionTable[18][21] = 19;
	
	stateFlags[18] = 1;
	
	//State 19
	stateFlags[19] = 2;
	
	//State 20
	transitionTable[20][0] = 20;
	transitionTable[20][1] = 20;
	transitionTable[20][12] = 22;
	for(int i=2;i<numInputs;i++)
		if(i!=12)
			transitionTable[20][i] = 21;
	
	stateFlags[20] = 1;
	
	//State 21
	stateFlags[21] = 3;
	
	//State 22
	transitionTable[22][0] = 23;
	transitionTable[22][1] = 23;
	
	stateFlags[22] = 1;
	
	//State 23
	transitionTable[23][0] = 24;
	transitionTable[23][1] = 24;
	
	stateFlags[23] = 1;
	
	//State 24
	stateFlags[24] = 2;
	
	//State 25
	transitionTable[25][25] = 31;
	transitionTable[25][6] = 26;
	for(int i=0;i<numInputs;i++)
		if(i!=25 && i!=6)
			transitionTable[25][i] = 30;
		
	stateFlags[25] = 1;
	
	//State 26
	transitionTable[26][6] = 27;
	for(int i=0;i<numInputs;i++)
		if(i!=6)
			transitionTable[26][i] = 32;
			
	stateFlags[26] = 1;
	
	//State 27
	transitionTable[27][6] = 28;
	for(int i=0;i<numInputs;i++)
		if(i!=6)
			transitionTable[27][i] = 29;
			
	stateFlags[27] = 1;
	
	//State 28
	stateFlags[28] = 2;
	
	//State 29
	stateFlags[29] = 5;
	
	//State 30
	stateFlags[30] = 3;
	
	//State 31
	stateFlags[31] = 2;
	
	//State 32
	stateFlags[32] = 4;
	
	//State 33
	transitionTable[33][25] = 35;
	for(int i=0;i<numInputs;i++)
		if(i!=25)
			transitionTable[33][i] = 34;
		
	stateFlags[33] = 1;
	
	//State 34
	stateFlags[34] = 3;
	
	//State 35
	stateFlags[35] = 2;
	
	//State 36
	transitionTable[36][25] = 37;
	
	stateFlags[36] = 1;
	
	//State 37
	stateFlags[37] = 2;
	
	//State 38
	transitionTable[38][25] = 39;
	
	stateFlags[38] = 1;
	
	//State 39
	stateFlags[39] = 2;
	
	//State 40
	transitionTable[40][27] = 40;
	transitionTable[40][28] = 40;
	transitionTable[40][29] = 40;
	for(int i=0;i<(numInputs-3);i++)
		transitionTable[40][i] = 41;
	
	stateFlags[40] = 1;
	
	//State 41
	stateFlags[41] = 0;
	
	//State 42
	transitionTable[42][28] = 43;
	for(int i=0;i<numInputs;i++)
		if(i!=28)
			transitionTable[42][i] = 42;
		
	stateFlags[42] = 1;
	
	//State 43
	stateFlags [43] = 2;
	
	//State 44,45,.....,56
	for(int i=44;i<=56;i++)
		stateFlags[i] = 2;	

}

//Read input stream into the buffer
void getStream(FILE* fp)
{
	int numRead = fread(inputBuffer, sizeof(char), bufferLength, fp);
	if(numRead<bufferLength)
		inputBuffer[numRead] = ' ';
}


//Get the next token from the input stream
char* getNextToken(FILE* fp)
{
	//Reset state to 0
	state = 0;

	if(end == bufferLength || end == 0)
		getStream(fp);

	while(1)
	{

		char ch = inputBuffer[end];
		if(ch == NULL)
			return NULL;

		int index = getTransitionIndex(ch);

		//Transit to next state
		state = transitionTable[state][index];

		//Invalid state
		if(state == -1)
			return "TK_ERROR";
		


		if(stateFlags[state] == 0)
			{
				
				begin = end;
				end--;
				state = 0;
			}


		if(stateFlags[state] >= 2)
			{
				
				//Retract using flag
				end -= (stateFlags[state]-2);

				//Get lexeme
				char* lexeme = (char*)malloc(sizeof(char)*(end-begin+2));
				lexeme[end-begin+1] = '\0';

				for(int i=begin;i<=end;i++)
					lexeme[i-begin] = inputBuffer[i];

				
				/*
				*TODO:
					1) Make Token Info structure
					2) Convert strings to Enum defined in masterHeader.h
				*/
				//TokenInfo newToken = (TokenInfo)malloc(sizeof(tokenInfo));
				//newToken->lexeme = (char*)malloc(sizeof(char)*(end-begin+2));
				//newToken->[end-begin+1] = '\0';

				end++;
				begin=end;
				//Get token corresponding to lexeme;



				return getTokenString(lexeme); 
			}
		end++;
	}

	return NULL;
}


int main()
{

	initializeLexer();
	//printf("%s\n",KeywordTable[hash("while")]->head->next->token);

	
    FILE* fp = fopen("sample.txt","r");
    //getStream(fp);
    char* t = getNextToken(fp);
    
    while(t!=NULL)
   	{ 
   		printf("%s ",t);
    	t = getNextToken(fp);
	}

	return 0;
}

#define numStates 60
#define numInputs 40
#define bufferLength 500
#define keywordHashModulo 111
#define keywordHashMul 31
#define maxTokenLength 15
#define numKeywords 24


typedef struct tInfo{
	char* lexeme;
	char* token;
	int flag;
	int lineNumber;
}tokenInfo;

typedef struct tokenInfo* TokenInfo;

//typedef struct transNode* TransNode;


//Buffer Variables
int begin,end,state;
char *buffer1, *buffer2,*inputBuffer;

//
char** tokenList;
int transitionTable[numStates][numInputs];

//flag = 0 => ERROR | flag = 1+x => ACCEPT + x RETRACT
int *stateFlags;


//Hash table for keywords
LinkedList* KeywordTable;

//Current Line Number for error handling
int currentLineNumber = 1;
/*
 Group - 5
 Vikram Waradpande - 2015B4A70454P
 Rinkesh Jain - 2015B4A70590P
 Yajat Dawar - 2015B4A70620P
 Anmol Naugaria - 2015B4A70835P
*/


#include "symbolTable.h"
#include "lexer.h"
#include "parser.h"
#include<string.h>
#include<stdio.h>
#include<stdlib.h>

/*
  Pass 1: Handle all Record typeDefinitions
  Pass 2: Handle all global variables
  Pass 3: Handle param lists, definitions and calculate attributes
*/

int getFunctionIndex(char* funcName)
{
  for(int i=0;i<currFuncIndex;i++)
    if(strcmp(funcName,functionTable[i]->funcName) == 0)
      return i;
  return -1;
}

void initializeSymbolTableVariables()
{
  currFuncIndex = 0;
  numFunctions = 1;
  symTableLen = 1;
  numRecords = 1;
  currRecIndex = 0;
  numGlobals = 1;
  currGlobalIndex = 0;
  yajatNum=1;

  functionTable = (FuncTable*)malloc(numFunctions*sizeof(FuncTable));

  for(int i=0;i<numFunctions;i++)
  {
    functionTable[i] = (FuncTable)malloc(sizeof(struct funcTable));
    functionTable[i]->funcName = NULL;
    functionTable[i]->symTable = NULL;
    functionTable[i]->inputParType = NULL;
    functionTable[i]->numInputParams = 0;
    functionTable[i]->outputParType = NULL;
    functionTable[i]->numOutputParams = 0;
    functionTable[i]->currIndex = 0;
    functionTable[i]->tableSize = 0;
    functionTable[i]->callFlag = 0;
  }

  recordTable = (RecTable)malloc(sizeof(struct recordTable)*numRecords);

  //Allocate global table
  globalTable = (SymTableEntry)malloc(sizeof(struct symTableEntry)*numGlobals);

  //Initialize hash table for records
  initializeRecordHashTable();

}

void populateSymbolTable(TreeNode root)
{
    TreeNode tempRoot = root;

    //-------------Pass 1------------------

    // tempRoot = function->function->...->mainFunction
    tempRoot = tempRoot->children;
    while(tempRoot!=NULL)
      {
        populateRecordDefs(tempRoot);
        tempRoot = tempRoot->next;
        currFuncIndex++;
      }

    //------------Pass 2--------------------

    //tempRoot = function->function->...->mainFunction
    tempRoot = root->children;
    while(tempRoot!=NULL)
      {
        populateGlobalVars(tempRoot);
        tempRoot = tempRoot->next;
      }

    // //------------Pass 3--------------------
    //
    // // tempRoot = function->function->...->mainFunction
    tempRoot = root->children;
    int currentFunctionIndex = 0;
    while(tempRoot!=NULL)
      {
        populateDeclarations(tempRoot,currentFunctionIndex);
        currentFunctionIndex++;
        tempRoot = tempRoot->next;
      }

}


void populateRecordDefs(TreeNode root)
{

    char* functionName = (char*)malloc(sizeof(char)*31);
    if(root->nt != mainFunction)
      strcpy(functionName,root->children->leafInfo->lexeme);
    else
      strcpy(functionName,"_main");

    for(int i=0;i<currFuncIndex;i++)
    {
      if(strcmp(functionTable[i]->funcName,functionName) == 0)
      {
        semanticCorrectnessFlag = 0;
        if(printErrorFlag==1)printf("Line %d: Function %s already declared\n",root->children->leafInfo->lineNumber,functionName);
        return;
      }
    }

    allocateSymbolTable();

    functionTable[currFuncIndex]->funcName = (char*)malloc(sizeof(char)*31);
    functionTable[currFuncIndex]->ASTNode = root;
    strcpy(functionTable[currFuncIndex]->funcName,functionName);

    if(root->nt != mainFunction)
      root = root->children->next;
    else
      root = root->children;

    if(root->nt == input_par)
      root = root->next;

    if(root->nt == output_par)
      root = root->next;

    if(root->nt == typeDefinitions)
    {

      TreeNode recIDNode = root->children;
      while(recIDNode!=NULL)
      {

        if(numRecords == currRecIndex)
          reallocateRecordTable();

        char* recordName = (char*)malloc(sizeof(char)*31);
        strcpy(recordName, recIDNode->leafInfo->lexeme);

        //Check if record already defined
        if(checkRecordRedeclaration(recordName))
        {
          semanticCorrectnessFlag = 0;
          if(printErrorFlag==1)printf("Line %d: Record %s already declared\n",recIDNode->leafInfo->lineNumber,recordName);
          recIDNode = recIDNode->next;
          continue;
        }
        //Make an extry in the global hash table for records
        insertIntoRecordHashTable(recordName,currRecIndex);

        //Type of a record is defined as currentIndex+2
        recordTable[currRecIndex].recordType = currRecIndex+2;
        recordTable[currRecIndex].recordName = (char*)malloc(sizeof(char)*31);
        strcpy(recordTable[currRecIndex].recordName,recordName);

        TreeNode temp = recIDNode;
        int numEntries = 0;

        //Calculate number of fields
        while(temp!=NULL)
        {
          if(numEntries == 0)
            {temp = temp->children;temp = temp->next;}
          else
            temp = temp->next;
          temp = temp->next;
          numEntries++;
        }

        recordTable[currRecIndex].numFields = numEntries;
        recordTable[currRecIndex].recordFields = (SymTableEntry)malloc(sizeof(struct symTableEntry)*numEntries);

        temp = recIDNode->children;
        numEntries = 0;
        int totalWidth = 0;

        //Fill fields of the record in recordFields
        while(temp!=NULL)
        {
          if(temp->t == TK_INT)
            {
              recordTable[currRecIndex].recordFields[numEntries].type = INT;
              totalWidth += INT_WIDTH;
              recordTable[currRecIndex].recordFields[numEntries].width = INT_WIDTH;
            }
          else
            {
              recordTable[currRecIndex].recordFields[numEntries].type = REAL;
              totalWidth += REAL_WIDTH;
              recordTable[currRecIndex].recordFields[numEntries].width = REAL_WIDTH;
            }

           recordTable[currRecIndex].recordFields[numEntries].symName = (char*)malloc(strlen(temp->next->leafInfo->lexeme)*sizeof(char));
           strcpy(recordTable[currRecIndex].recordFields[numEntries].symName,temp->next->leafInfo->lexeme);
           temp = temp->next->next;
           numEntries++;
        }

        recordTable[currRecIndex].recordWidth = totalWidth;
        currRecIndex++;
        recIDNode = recIDNode->next;
      }
    }
}


void populateGlobalVars(TreeNode root)
{
    if(root->nt == mainFunction)
      root = root->children;
    else
      root = root->children->next;
    if(root->nt == input_par)
        root = root->next;

    if(root->nt == output_par)
        root = root->next;

    if(root->nt == typeDefinitions)
        root = root->next;

    if(root->nt == declarations)
    {
        root = root->children;
        while(root!=NULL)
        {
            TreeNode globalOrNot = root->next->next;
            if(globalOrNot->t == TK_GLOBAL)
            {

                if(checkGlobalRedifinition(root->next->leafInfo->lexeme))
                {
                  semanticCorrectnessFlag = 0;
                  if(printErrorFlag==1)printf("Line %d: Global variable %s redeclared.\n",root->next->leafInfo->lineNumber,root->next->leafInfo->lexeme);
                  root = root->next->next->next;
                  continue;
                }

                if(currGlobalIndex == numGlobals)
                {
                    globalTable = (SymTableEntry)realloc(globalTable,2*numGlobals*sizeof(struct symTableEntry));
                    numGlobals*= 2;
                    for(int i = currGlobalIndex ; i < numGlobals ; i++ )
                    {
                        globalTable[i].ASTNode = NULL;
                        globalTable[i].symName = NULL;
                        globalTable[i].type = -1;
                        globalTable[i].width = 0;
                    }
                }

                if(root->t == TK_INT)
                {
                    globalTable[currGlobalIndex].type = INT;
                    globalTable[currGlobalIndex].width = INT_WIDTH;
                }
                else if(root->t == TK_REAL)
                {
                    globalTable[currGlobalIndex].type = REAL;
                    globalTable[currGlobalIndex].width = REAL_WIDTH;
                }
                else if(root->t == TK_RECORDID)
                {
                    int recordIndex = getRecordIndex(root->leafInfo->lexeme);
                    if(recordIndex == -1)
                    {
                        printf("%d Record not defined\n",root->leafInfo->lineNumber);
                        continue;
                    }
                    else
                    {
                        globalTable[currGlobalIndex].type = recordIndex + 2;
                        globalTable[currGlobalIndex].width = recordTable[recordIndex].recordWidth;
                    }
                }
                else{
                    //error
                }

                globalTable[currGlobalIndex].ASTNode = root;
                globalTable[currGlobalIndex].symName = (char*)malloc(sizeof(char)*21);
                strcpy(globalTable[currGlobalIndex].symName,root->next->leafInfo->lexeme);


                currGlobalIndex++;
            }
            root = root->next->next->next;
        }
    }
}

void populateDeclarations(TreeNode root,int currentFunctionIndex)
{


  if(root->nt == mainFunction)
    root = root->children;
  else
    root = root->children->next;

  if(root->nt == input_par)
    {
       handleInputPar(root,currentFunctionIndex);
       root = root->next;
    }

  if(root->nt == output_par)
    {
      handleOutputPar(root,currentFunctionIndex);
      root = root->next;
    }

  if(root->nt == typeDefinitions)
      root = root->next;

  int symTableIndex = functionTable[currentFunctionIndex]->currIndex;

  if(root->nt == declarations)
  {
      root = root->children;
      while(root!=NULL)
      {
          TreeNode globalOrNot = root->next->next;
          if(globalOrNot->t == eps)
          {

            if(checkVariableRedefinition(currentFunctionIndex,root->next->leafInfo->lexeme))
            {
              semanticCorrectnessFlag = 0;
              if(printErrorFlag==1)printf("Line %d: Variable %s redeclared.\n",root->next->leafInfo->lineNumber,root->next->leafInfo->lexeme);
              root = root->next->next->next;
              continue;
            }

            //Symbol table full
            if(functionTable[currentFunctionIndex]->currIndex == functionTable[currentFunctionIndex]->tableSize)
              reallocateSymbolTable(currentFunctionIndex);


              if(root->t == TK_INT)
              {
                  functionTable[currentFunctionIndex]->symTable[symTableIndex].type = INT;
                  functionTable[currentFunctionIndex]->symTable[symTableIndex].width = INT_WIDTH;
              }
              else if(root->t == TK_REAL)
              {
                functionTable[currentFunctionIndex]->symTable[symTableIndex].type = REAL;
                functionTable[currentFunctionIndex]->symTable[symTableIndex].width = REAL_WIDTH;
              }
              else if(root->t == TK_RECORDID)
              {
                  int recordIndex = getRecordIndex(root->leafInfo->lexeme);
                  if(recordIndex == -1)
                  {
                      printf("%d Record not defined\n",root->leafInfo->lineNumber);
                      root = root->next->next->next;
                      continue;
                  }
                  else
                  {
                    functionTable[currentFunctionIndex]->symTable[symTableIndex].type = recordIndex+2;
                    functionTable[currentFunctionIndex]->symTable[symTableIndex].width = recordTable[recordIndex].recordWidth;
                  }
              }
              else{
                  //error
              }

              functionTable[currentFunctionIndex]->symTable[symTableIndex].ASTNode = root;
              functionTable[currentFunctionIndex]->symTable[symTableIndex].symName = (char*)malloc(sizeof(char)*21);
              strcpy(functionTable[currentFunctionIndex]->symTable[symTableIndex].symName,root->next->leafInfo->lexeme);
              symTableIndex++;
              functionTable[currentFunctionIndex]->currIndex++;
          }
          root = root->next->next->next;

      }
  }
  functionTable[currentFunctionIndex]->currIndex = symTableIndex;
}


void allocateSymbolTable()
{
  if(currFuncIndex == numFunctions)
  {
    functionTable = (FuncTable*)realloc(functionTable,2*numFunctions*sizeof(FuncTable));

    for(int i=numFunctions;i<2*numFunctions;i++)
    {
      functionTable[i] = (FuncTable)malloc(sizeof(struct funcTable));
      functionTable[i]->funcName = NULL;
      functionTable[i]->symTable = NULL;
      functionTable[i]->inputParType = NULL;
      functionTable[i]->numInputParams = 0;
      functionTable[i]->outputParType = NULL;
      functionTable[i]->numOutputParams = 0;
      functionTable[i]->currIndex = 0;
      functionTable[i]->tableSize = 0;
      functionTable[i]->callFlag = 0;
      functionTable[i]->ASTNode = NULL;
    }
    numFunctions*=2;
  }

  functionTable[currFuncIndex]->symTable = (SymTableEntry)malloc(symTableLen*sizeof(struct symTableEntry));

  for(int i=0;i<symTableLen;i++)
    functionTable[currFuncIndex]->symTable[i].varChanged = 0;

  functionTable[currFuncIndex]->tableSize = symTableLen;
}

void handleInputPar(TreeNode root,int currentFunctionIndex)
{
    //make root = input_par
    TreeNode temp = root->children;
    int numParams = 0;

    while(temp!=NULL)
    {
      temp = temp->next;
      temp = temp->next;
      numParams++;
    }

    functionTable[currentFunctionIndex]->inputParType = (int*)malloc(sizeof(int)*numParams);
    functionTable[currentFunctionIndex]->numInputParams = numParams;
    //functionTable[currIndex]->symTable->ASTNode = root;

    //make temp = input_par
    temp = root->children;
    numParams = 0;

    while(temp!=NULL)
    {

      if(functionTable[currentFunctionIndex]->currIndex == functionTable[currentFunctionIndex]->tableSize)
        reallocateSymbolTable(currentFunctionIndex);

      TreeNode varNameNode = temp->next;

      if(checkGlobalRedifinition(varNameNode->leafInfo->lexeme))
      {
        semanticCorrectnessFlag = 0;
        if(printErrorFlag==1)printf("Line %d: Variable %s already declared as a global.\n",varNameNode->leafInfo->lineNumber,varNameNode->leafInfo->lexeme);
        functionTable[currentFunctionIndex]->symTable[numParams].type = -1;
        functionTable[currentFunctionIndex]->symTable[numParams].width = 0;
        functionTable[currentFunctionIndex]->inputParType[numParams] = -1;
        functionTable[currentFunctionIndex]->symTable[numParams].symName = (char*)malloc(sizeof(char)*21);
        strcpy(functionTable[currentFunctionIndex]->symTable[numParams].symName,temp->next->leafInfo->lexeme);
        numParams++;
        functionTable[currentFunctionIndex]->currIndex++;
        temp = temp->next->next;
        continue;
      }

      if(temp->t == TK_RECORDID)
        {
          if(!checkRecordRedeclaration(temp->leafInfo->lexeme))
          {
              semanticCorrectnessFlag = 0;
              if(printErrorFlag==1)printf("Line %d: Record %s not defined.\n",temp->leafInfo->lineNumber,temp->leafInfo->lexeme);
              functionTable[currentFunctionIndex]->symTable[numParams].type = -1;
              functionTable[currentFunctionIndex]->symTable[numParams].width = 0;
              functionTable[currentFunctionIndex]->inputParType[numParams] = -1;
              functionTable[currentFunctionIndex]->symTable[numParams].symName = (char*)malloc(sizeof(char)*21);
              strcpy(functionTable[currentFunctionIndex]->symTable[numParams].symName,temp->next->leafInfo->lexeme);
              numParams++;
              functionTable[currentFunctionIndex]->currIndex++;
              temp = temp->next->next;
              continue;
          }

          functionTable[currentFunctionIndex]->inputParType[numParams] = getRecordIndex(temp->leafInfo->lexeme)+2;
          functionTable[currentFunctionIndex]->symTable[numParams].type = getRecordIndex(temp->leafInfo->lexeme)+2;
          functionTable[currentFunctionIndex]->symTable[numParams].width = recordTable[getRecordIndex(temp->leafInfo->lexeme)].recordWidth;
        }

      if(temp->t == TK_INT)
        {
          functionTable[currentFunctionIndex]->symTable[numParams].type = INT;
          functionTable[currentFunctionIndex]->symTable[numParams].width = INT_WIDTH;
          functionTable[currentFunctionIndex]->inputParType[numParams] = INT;
        }
      else if(temp->t == TK_REAL)
        {
          functionTable[currentFunctionIndex]->symTable[numParams].type = REAL;
          functionTable[currentFunctionIndex]->symTable[numParams].width = REAL_WIDTH;
          functionTable[currentFunctionIndex]->inputParType[numParams] = REAL;
        }

      temp = temp->next;

      functionTable[currentFunctionIndex]->symTable[numParams].symName = (char*)malloc(sizeof(char)*21);
      strcpy(functionTable[currentFunctionIndex]->symTable[numParams].symName,temp->leafInfo->lexeme);
      numParams++;
      functionTable[currentFunctionIndex]->currIndex++;

      temp = temp->next;

    }
}

void handleOutputPar(TreeNode root,int currentFunctionIndex)
{
  //make root = input_par
  TreeNode temp = root->children;
  int numParams = functionTable[currentFunctionIndex]->currIndex;
  int prevParams = functionTable[currentFunctionIndex]->currIndex;
  while(temp!=NULL)
  {
    temp = temp->next;
    temp = temp->next;
    numParams++;
  }

  functionTable[currentFunctionIndex]->outputParType = (int*)malloc(sizeof(int)*(numParams-prevParams));
  functionTable[currentFunctionIndex]->numOutputParams = numParams-prevParams;

  //functionTable[currIndex]->symTable->ASTNode = root;

  //make temp = output_par
  temp = root->children;
  numParams = functionTable[currentFunctionIndex]->currIndex;

  while(temp!=NULL)
  {

    if(functionTable[currentFunctionIndex]->currIndex == functionTable[currentFunctionIndex]->tableSize)
      reallocateSymbolTable(currentFunctionIndex);

    TreeNode varNameNode = temp->next;

    if(checkGlobalRedifinition(varNameNode->leafInfo->lexeme))
    {
      semanticCorrectnessFlag = 0;
      if(printErrorFlag==1)printf("Line %d: Variable %s already declared as a global.\n",varNameNode->leafInfo->lineNumber,varNameNode->leafInfo->lexeme);
      functionTable[currentFunctionIndex]->symTable[numParams].type = -1;
      functionTable[currentFunctionIndex]->symTable[numParams].width = 0;
      functionTable[currentFunctionIndex]->outputParType[numParams-prevParams] = -1;
      functionTable[currentFunctionIndex]->symTable[numParams].symName = (char*)malloc(sizeof(char)*21);
      strcpy(functionTable[currentFunctionIndex]->symTable[numParams].symName,temp->next->leafInfo->lexeme);
      numParams++;
      functionTable[currentFunctionIndex]->currIndex++;
      temp = temp->next->next;
      continue;
    }

    if(temp->t == TK_RECORDID)
      {
        if(!checkRecordRedeclaration(temp->leafInfo->lexeme))
        {
            semanticCorrectnessFlag = 0;
            if(printErrorFlag==1)printf("Line %d: Record %s not defined.\n",temp->leafInfo->lineNumber,temp->leafInfo->lexeme);
            functionTable[currentFunctionIndex]->symTable[numParams].type = -1;
            functionTable[currentFunctionIndex]->symTable[numParams].width = 0;
            functionTable[currentFunctionIndex]->outputParType[numParams-prevParams] = -1;
            functionTable[currentFunctionIndex]->symTable[numParams].symName = (char*)malloc(sizeof(char)*21);
            strcpy(functionTable[currentFunctionIndex]->symTable[numParams].symName,temp->next->leafInfo->lexeme);
            numParams++;
            functionTable[currentFunctionIndex]->currIndex++;
            temp = temp->next->next;
            continue;
        }

        functionTable[currentFunctionIndex]->outputParType[numParams-prevParams] = getRecordIndex(temp->leafInfo->lexeme)+2;
        functionTable[currentFunctionIndex]->symTable[numParams].type = getRecordIndex(temp->leafInfo->lexeme)+2;
        functionTable[currentFunctionIndex]->symTable[numParams].width = recordTable[getRecordIndex(temp->leafInfo->lexeme)].recordWidth;
      }

    if(temp->t == TK_INT)
      {
        functionTable[currentFunctionIndex]->symTable[numParams].type = INT;
        functionTable[currentFunctionIndex]->symTable[numParams].width = INT_WIDTH;
        functionTable[currentFunctionIndex]->outputParType[numParams-prevParams] = INT;
      }
    else if(temp->t == TK_REAL)
      {
        functionTable[currentFunctionIndex]->symTable[numParams].type = REAL;
        functionTable[currentFunctionIndex]->symTable[numParams].width = REAL_WIDTH;
        functionTable[currentFunctionIndex]->outputParType[numParams-prevParams] = REAL;
      }

    temp = temp->next;

    functionTable[currentFunctionIndex]->symTable[numParams].symName = (char*)malloc(sizeof(char)*21);
    strcpy(functionTable[currentFunctionIndex]->symTable[numParams].symName,temp->leafInfo->lexeme);
    numParams++;
    functionTable[currentFunctionIndex]->currIndex++;

    temp = temp->next;

  }
}

void initializeRecordHashTable()
{
	recordHashTable = (RecordHashNode)malloc(sizeof(struct recordHashNode)*yajatNum);

	for(int i=0;i<yajatNum;i++)
		{
			recordHashTable[i].recType = -1;
			recordHashTable[i].next = NULL;
			recordHashTable[i].recName = NULL;
		}
}

void insertIntoRecordHashTable(char* recName,int recType)
{

	int index = hash(recName);
  index = index%yajatNum;
	int len = strlen(recName);

	if(recordHashTable[index].recType==-1)
	{
		recordHashTable[index].recName = (char*)malloc(sizeof(char)*len+1);
		strcpy(recordHashTable[index].recName,recName);
		recordHashTable[index].recType = recType;
		recordHashTable[index].next = NULL;
		return ;
	}

	else
	{
		// travel in the list;
		RecordHashNode temp = recordHashTable[index].next;
		if(temp==NULL)
		{
			recordHashTable[index].next =  (struct recordHashNode*)malloc(sizeof(struct recordHashNode));
			recordHashTable[index].next->recName = (char*)malloc(sizeof(char)*len+1);
			strcpy(recordHashTable[index].next->recName,recName);
			recordHashTable[index].next->recType = recType;
			recordHashTable[index].next->next = NULL;
			return ;
		}
		while(temp->next!=NULL)
		{
			temp = temp->next;
		}

		temp->next = (struct recordHashNode*)malloc(sizeof(struct recordHashNode));
		temp->next->recName = (char*)malloc(sizeof(char)*len+1);
		strcpy(temp->next->recName,recName);
		temp->next->recType = recType;
		temp->next->next = NULL;

	}
}

int getRecordIndex(char* recName)
{
	int index = hash(recName);
  index = index%yajatNum;
	if(recordHashTable[index].recType==-1)
		return -1;

	else if(strcmp(recordHashTable[index].recName,recName)==0)
		return recordHashTable[index].recType;

	else
	{
		RecordHashNode temp = recordHashTable[index].next;
		if(temp==NULL)
			return -1;

		while(temp!=NULL)
		{
			if(strcmp(temp->recName,recName)==0)
				return temp->recType;

			temp = temp->next;
		}

		return -1;
	}

	return -1;
}


void reallocateSymbolTable(int currentFunctionIndex)
{
  int symTableSize = functionTable[currentFunctionIndex]->tableSize;
  functionTable[currentFunctionIndex]->symTable = (SymTableEntry)realloc(functionTable[currentFunctionIndex]->symTable,2*symTableSize*sizeof(struct symTableEntry));
  functionTable[currentFunctionIndex]->tableSize*= 2;
  for(int i = symTableSize ; i < 2*symTableSize ; i++ )
  {
      functionTable[currentFunctionIndex]->symTable[i].ASTNode = NULL;
      functionTable[currentFunctionIndex]->symTable[i].symName = NULL;
      functionTable[currentFunctionIndex]->symTable[i].type = -1;
      functionTable[currentFunctionIndex]->symTable[i].width = 0;
      functionTable[currentFunctionIndex]->symTable[i].varChanged = 0;
  }
}

void reallocateRecordTable()
{
  recordTable = (RecTable)realloc(recordTable,2*numRecords*sizeof(struct recordTable));
  for(int i=numRecords;i<2*numRecords;i++)
    {
      recordTable[i].recordName = NULL;
      recordTable[i].recordFields = NULL;
      recordTable[i].numFields = -1;
      recordTable[i].recordWidth = -1;
      recordTable[i].recordType = -1;
    }
    numRecords*=2;
}


//Check if the record is redeclared
int checkRecordRedeclaration(char* recordName)
{
  for(int i=0;i<currRecIndex;i++)
    if(strcmp(recordName,recordTable[i].recordName) == 0)
      return 1;

  return 0;
}

int checkVariableRedefinition(int funcIndex,char* string)
{
	SymTableEntry t = functionTable[funcIndex]->symTable;
	int symbolTableLen = functionTable[funcIndex]->currIndex;
	for(int i=0;i<symbolTableLen;i++)
		if(strcmp(string,t[i].symName)==0)
			return 1;
	return 0;
}



int checkGlobalRedifinition(char* string)
{
	for(int i=0;i<currGlobalIndex;i++)
		if(strcmp(string,globalTable[i].symName)==0)
			return 1;
	return 0;
}

char* getTpString(int type)
{
  if(type==-1)
    return "NotDefined";
  if(type == 0)
    return "integer";
  else if(type == 1)
    return "real";
  else
    return recordTable[type-2].recordName;
}

void printSymbolTable()
{
printf("\n");

printf("Printing Symbol Table\n\n");
printf("%20s\t\t%20s\t\t%20s\t\t%20s\n\n","Lexeme","Type","Scope","Offset");
//printf("\nLexeme\t\ttype\t\tscope\t\toffset\n\n");

 for(int i=0;i<currGlobalIndex;i++)
  {
    if(globalTable[i].type>=2)
    {
        printf("%20s\t\t",globalTable[i].symName);
        int type = globalTable[i].type;

        int index = type-2;

        for(int j=0;j<recordTable[index].numFields;j++)
        {
          if(j!=recordTable[index].numFields-1)
            printf("%s x ",getTpString(recordTable[index].recordFields[j].type));
          else
            printf("%s\t\t",getTpString(recordTable[index].recordFields[j].type));

        }

        printf("%20s\t\t%20s","global","-");
    }
    else if(globalTable[i].type!=-1)
    {
      printf("%20s\t\t%20s\t\t%20s\t\t%20s",globalTable[i].symName,getTpString(globalTable[i].type),"global","-");
    }
    printf("\n");
  }


  for(int i=0;i<currFuncIndex;i++)
  {
    // printf("\nFunction - %s\n",functionTable[i]->funcName);

    for(int j=0;j<functionTable[i]->currIndex;j++)
    {
      if(functionTable[i]->symTable[j].type >= 2)
      {
        printf("%20s\t\t",functionTable[i]->symTable[j].symName);
        int type = functionTable[i]->symTable[j].type;

        int index = type-2;


        for(int k=0;k<recordTable[index].numFields;k++)
        {
          if(k!=recordTable[index].numFields-1)
            printf("%s x ",getTpString(recordTable[index].recordFields[k].type));
          else
            printf("%s\t\t",getTpString(recordTable[index].recordFields[k].type));

        }

        printf("%20s\t\t%20d\n",functionTable[i]->funcName,functionTable[i]->symTable[j].offset);

      }
      else if(functionTable[i]->symTable[j].type != -1)
      {
          printf("%20s\t\t%20s\t\t%20s\t\t%20d\n",functionTable[i]->symTable[j].symName,getTpString(functionTable[i]->symTable[j].type),functionTable[i]->funcName,functionTable[i]->symTable[j].offset);

      }

    }

  }
}
void printMemory(){
  printf("\nPrinting memory used by each function\n");
  int sum=0;
  for(int i=0;i<currFuncIndex;i++){
    sum=0;
    for(int j=0;j<functionTable[i]->currIndex;j++){
      sum+=functionTable[i]->symTable[j].width;
    }
    printf("\n%s\t%d\n", functionTable[i]->funcName,sum);
  }

}
void printGlobalRecords(){
  printf("\nPrinting details of global records\n\n");
  for(int i=0;i<currRecIndex;i++){
    printf("%s\t\t",recordTable[i].recordName);
    for(int j=0;j< recordTable[i].numFields;j++){
      if(j==recordTable[i].numFields-1){
        printf("%s",getTpString(recordTable[i].recordFields[j].type));
      }
      else printf("%s,",getTpString(recordTable[i].recordFields[j].type));
    }
    printf("\t\t%d\n",recordTable[i].recordWidth);
  }
}
void printGlobalTable(){

  printf("Printing Global table\n\n");

printf("%20s\t\t%20s\t\t%20s\n\n","Lexeme","Type","Offset");

  int offset = 0;
  for(int i=0;i<currGlobalIndex;i++)
  {
    if(globalTable[i].type>=2)
    {
        printf("%20s\t\t",globalTable[i].symName);
        int type = globalTable[i].type;

        int index = type-2;

        for(int j=0;j<recordTable[index].numFields;j++)
        {
          if(j!=recordTable[index].numFields-1)
            printf("%s x ",getTpString(recordTable[index].recordFields[j].type));
          else
            printf("%s\t\t",getTpString(recordTable[index].recordFields[j].type));

        }

        printf("%20d\n",offset);

    }
    else
    {
      printf("%20s\t\t%20s\t\t%20d-",globalTable[i].symName,getTpString(globalTable[i].type),offset);
    }

    offset+= globalTable[i].width;

    printf("\n");
  }

}
void calculateOffset()
{
  for(int i=0;i<currFuncIndex;i++)
  {
    int offset = 0;

    for(int j=0;j<functionTable[i]->currIndex;j++)
    {
      functionTable[i]->symTable[j].offset = offset;
      offset+= functionTable[i]->symTable[j].width;
    }
  }
}

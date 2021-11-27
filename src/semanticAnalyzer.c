/*
 Group - 5
 Vikram Waradpande - 2015B4A70454P
 Rinkesh Jain - 2015B4A70590P
 Yajat Dawar - 2015B4A70620P
 Anmol Naugaria - 2015B4A70835P
*/

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include "lexer.h"
#include "parser.h"
#include "typeChecker.h"
#include "symbolTable.h"
#include "semanticAnalyzer.h"


void checkFunctionCallSemantics(TreeNode root, int functionIndex)
{
  TreeNode inputParNode = root->children->next;
  TreeNode outputParNode = root->children->next->next;
  int lineNumber = root->children->leafInfo->lineNumber;
  int calleeIndex = getFunctionIndex(root->children->leafInfo->lexeme);
  SymTableEntry symbolTable = functionTable[functionIndex]->symTable;
  int symTableLen = functionTable[functionIndex]->currIndex;

  //Invalid call
  if(calleeIndex == -1)
  {
    semanticCorrectnessFlag = 0;
    printf("Line %d: No function named %s declared before use\n",lineNumber,root->children->leafInfo->lexeme);
    return;
  }
  if(calleeIndex == functionIndex)
  {
    semanticCorrectnessFlag = 0;
    printf("Line %d : Function %s cant call itself \n",lineNumber,root->children->leafInfo->lexeme);
    return;
  }
  //Called before declaration
  if(calleeIndex>functionIndex)
  {
    semanticCorrectnessFlag = 0;
    printf("Line %d: Function %s called before defining\n",lineNumber,root->children->leafInfo->lexeme);
    return;
  }

  //Count number of params
  int numInputParams = 0;
  TreeNode temp = inputParNode->children;
  while(temp!=NULL)
  {
    numInputParams++;
    temp = temp->next;
  }
  int numOutputParams = 0;
  temp = outputParNode->children;
  while(temp!=NULL)
  {
    numOutputParams++;
    temp = temp->next;
  }

  //Number mismatch
  if(numOutputParams != functionTable[calleeIndex]->numOutputParams)
  {
    semanticCorrectnessFlag = 0;
    printf("Line %d: Function %s returns %d parameters, recieved %d\n",lineNumber,root->children->leafInfo->lexeme,functionTable[calleeIndex]->numOutputParams,numOutputParams);
    return;
  }

  if(numInputParams != functionTable[calleeIndex]->numInputParams)
  {
    semanticCorrectnessFlag = 0;
    printf("Line %d: Number of actual and formal parameters of function %s don't match\n",lineNumber,root->children->leafInfo->lexeme);
    return;
  }


  temp = inputParNode->children;
  //Check types of input parameters
  for(int i=0;i<numInputParams;i++)
  {
    int actualType = getType(functionIndex, temp);
    int formalType = functionTable[calleeIndex]->inputParType[i];
    if(actualType == -1)
    {
      //printf("Line %d: Invalid variable %s\n", temp->leafInfo->lineNumber, temp->leafInfo->lexeme);
      break;
    }
    //If formalType is -1, it means the error was already declared
    if(formalType == -1)
      break;

    if(formalType!=actualType)
    {
      printf("Line %d: Function type mismatch. Input parameter number %d is expected to be of type %s\n",temp->leafInfo->lineNumber,i+1,getTypeString(formalType));
    }
    temp = temp->next;
  }

 //Check type for output parameters
 temp = outputParNode->children;
 for(int i=0;i<numOutputParams;i++)
 {
   int actualType = getType(functionIndex, temp);
   int formalType = functionTable[calleeIndex]->outputParType[i];
   if(actualType == -1)
   {
     //printf("Line %d: Invalid variable %s\n", temp->leafInfo->lineNumber, temp->leafInfo->lexeme);
     temp=temp->next;
     continue;
   }
   //If formalType is -1, it means the error was already declared
   if(formalType == -1)
   {
     temp=temp->next;
     continue;
   }

   if(formalType!=actualType)
   {
     semanticCorrectnessFlag = 0;
     printf("Line %d: Function type mismatch.Output parameter number %d is expected to be of type %s\n",temp->leafInfo->lineNumber,i+1,getTypeString(formalType));
   }

    int symTableIndex = getVarIndex(functionIndex,temp->leafInfo->lexeme);
    if(symTableIndex == -1)
      {
        int globalTableIndex = getGlobalIndex(temp->leafInfo->lexeme);
        globalTable[globalTableIndex].varChanged = 1;
      }
      else
        symbolTable[symTableIndex].varChanged = 1;
   temp = temp->next;
 }


}



void checkIterativeStatementSemantics(TreeNode root,int functionIndex)
{
  int startLine = root->children->children->leafInfo->lineNumber;
  int endLine = 0;
  TreeNode stmtNode = root->children->next;
  while(stmtNode->id!=terminal)
  {
    stmtNode = stmtNode->next;
  }
  endLine = stmtNode->leafInfo->lineNumber;
  if(isValidWhile(root->children->children,root->children->next) == 0)
  {
    printf("Line %d-%d None of the variables participating in the iterations of the while loop gets updated \n",startLine,endLine);
    semanticCorrectnessFlag = 0;
  }
  checkBooleanSemantics(root->children->children,functionIndex);
  checkStatementSemantics(root->children->next,functionIndex);
}




int checkIfVarUpdated(char* name,TreeNode stmtNode)
{
  while(stmtNode!=NULL)
  {
    if(stmtNode->nt == assignmentStmt)
      if(strcmp(stmtNode->children->leafInfo->lexeme,name)==0)
        return 1;
    if(stmtNode->nt == ioStmt)
      if(stmtNode->children->t == TK_READ)
        if(strcmp(stmtNode->children->next->leafInfo->lexeme,name)==0)
          return 1;
    if(stmtNode->nt == funCallStmt)
    {
      TreeNode outputParList = stmtNode->children->next->next->children;
      while(outputParList!=NULL)
      {
        // printf("%s%s\n",outputParList->leafInfo->lexeme,name);
        if(strcmp(outputParList->leafInfo->lexeme,name)==0)
          return 1;
        outputParList = outputParList->next;
      }
    }
    stmtNode = stmtNode->next;
  }
  return 0;
}


int isValidWhile(TreeNode root,TreeNode stmtNode)
{
  if(root->t == TK_AND || root->t==TK_OR)
    return isValidWhile(root->children,stmtNode) + isValidWhile(root->children->next,stmtNode);
  if(root->t == TK_LT || root->t == TK_LE || root->t == TK_EQ || root->t == TK_GT || root->t == TK_GE || root->t == TK_NE)
    return isValidWhile(root->children,stmtNode) + isValidWhile(root->children->next,stmtNode);
  if(root->t == TK_NOT)
    return isValidWhile(root->children,stmtNode);
  if(root->t == TK_NUM || root->t == TK_RNUM)
    return 0;
  if(root->t == TK_ID)
    return checkIfVarUpdated(root->leafInfo->lexeme,stmtNode);
  return 0;
}

void checkConditionalStatementSemantics(TreeNode root,int functionIndex)
{
  checkBooleanSemantics(root->children->children,functionIndex);
  if(root->children->next->nt ==elsePart)
  {
    checkStatementSemantics(root->children->next->children,functionIndex);
    checkStatementSemantics(root->children->next->next,functionIndex);
    return;
  }
  checkStatementSemantics(root->children->next,functionIndex);
}



void checkStatementSemantics(TreeNode root,int functionIndex)
{
  if(root==NULL)
    return;
  while(root!=NULL || (root->id == terminal && root->t == TK_ENDWHILE))
  {
    if(root->nt == ioStmt)
      checkIOStatementSemantics(root,functionIndex);
    else if(root->nt == assignmentStmt)
      checkAssignmentStatement(root,functionIndex);
    else if(root->nt == conditionalStmt)
      checkConditionalStatementSemantics(root,functionIndex);
    else if(root->nt == iterativeStmt)
      checkIterativeStatementSemantics(root,functionIndex);
    else if(root->nt == funCallStmt)
      checkFunctionCallSemantics(root,functionIndex);
    else if(root->nt == returnStmt)
      checkReturnStatementSemantics(root,functionIndex);

    root = root->next;
    if(root == NULL)
      return;

  }
}
void checkIOStatementSemantics(TreeNode root , int functionIndex)
{
  SymTableEntry symbolTable = functionTable[functionIndex]->symTable;
  int symTableLen = functionTable[functionIndex]->currIndex;
  TreeNode idNode = root->children->next;
  if(root->children->t == TK_READ)
  {
    if(idNode->next->t == eps)
    {
      int type = getType(functionIndex,idNode);
      if(type>=2)
        {
          printf("Line : %d Record type can't be read \n",idNode->leafInfo->lineNumber);
          semanticCorrectnessFlag = 0;
        }
      if(type == 0 || type == 1)
      {
        int symTableIndex = getVarIndex(functionIndex,idNode->leafInfo->lexeme);
        if(symTableIndex == -1)
        {
          int globalTableIndex = getGlobalIndex(idNode->leafInfo->lexeme);
          globalTable[globalTableIndex].varChanged = 1;
        }
        else
          symbolTable[symTableIndex].varChanged = 1;
      }
      return;
    }
    else
    {
      int type = getType(functionIndex,idNode);
      if(type>=2)
      {
        int recordIndex = type-2;
        int flag=0;
        for(int i=0;i<recordTable[recordIndex].numFields;i++)
        {
          if(strcmp(recordTable[recordIndex].recordFields[i].symName,idNode->next->leafInfo->lexeme)==0)
          {
            flag=1;
            break;
          }
        }
        if(flag==0)
          {
            printf("Line %d:%s has no field named %s\n",idNode->leafInfo->lineNumber,idNode->leafInfo->lexeme,idNode->next->leafInfo->lexeme);
            semanticCorrectnessFlag = 0;
          }
        if(flag==1)
        {
          int symTableIndex = getVarIndex(functionIndex,idNode->leafInfo->lexeme);
          if(symTableIndex == -1)
          {
            int globalTableIndex = getGlobalIndex(idNode->leafInfo->lexeme);
            globalTable[globalTableIndex].varChanged = 1;
          }
          else
            symbolTable[symTableIndex].varChanged = 1;
        }
        return;
      }
      if(type == 0 || type == 1)
      {
        printf("Line %d:%s has no field named %s\n",idNode->leafInfo->lineNumber,idNode->leafInfo->lexeme,idNode->next->leafInfo->lexeme);
        semanticCorrectnessFlag = 0;
        return;
      }
    }
  }
  if(root->children->t == TK_WRITE)
  {
    if(idNode->id == nonterminal && idNode->nt ==allVarDash)
    {
      TreeNode recId = idNode->children;
      int type = getType(functionIndex,recId);
      if(type>=2)
      {
        int recordIndex = type-2;
        int flag=0;
        for(int i=0;i<recordTable[recordIndex].numFields;i++)
        {
          if(strcmp(recordTable[recordIndex].recordFields[i].symName,recId->next->leafInfo->lexeme)==0)
          {
            flag=1;
            break;
          }
        }
        if(flag==0)
          {
            printf("Line %d:%s has no field named %s\n",recId->leafInfo->lineNumber,recId->leafInfo->lexeme,recId->next->leafInfo->lexeme);
            semanticCorrectnessFlag = 0;
          }
        return;
      }
      if(type == 0 || type == 1)
      {
        printf("Line %d:%s has no field named %s\n",recId->leafInfo->lineNumber,recId->leafInfo->lexeme,recId->next->leafInfo->lexeme);
        semanticCorrectnessFlag = 0;
        return;
      }
      return;
    }
    int type = getType(functionIndex,idNode);
    if(type>=2){}
      //printf("Line : %d Record type can't be written\n",idNode->leafInfo->lineNumber);
  }
}

void checkReturnStatementSemantics(TreeNode root,int functionIndex)
{
  root = root->children;
  SymTableEntry symbolTable = functionTable[functionIndex]->symTable;
  if(root == NULL)
  {
    if(functionTable[functionIndex]->numOutputParams != 0)
    {
      semanticCorrectnessFlag = 0;
      printf("Line %d : Function expects %d output parameters but returning 0 output parameters\n",functionTable[functionIndex]->ASTNode->children->leafInfo->lineNumber,functionTable[functionIndex]->numOutputParams);
      return;
    }
    else
      return;
  }

  if(functionTable[functionIndex]->numOutputParams == 0)
    {
    semanticCorrectnessFlag = 0;
    printf("Line %d : Function expects no output parameters but returning some variables\n",root->leafInfo->lineNumber);
    return;
    }

  TreeNode outputParList = functionTable[functionIndex]->ASTNode->children->next->next->children;
  int returnCount = 0;
  while(root!=NULL)
  {
    outputParList=outputParList->next;
    if(strcmp(outputParList->leafInfo->lexeme,root->leafInfo->lexeme)!=0)
    {
      semanticCorrectnessFlag = 0;
      printf("Line %d:%s does not match with %s in output parameter list\n",root->leafInfo->lineNumber,root->leafInfo->lexeme,outputParList->leafInfo->lexeme);
      returnCount++;
      outputParList = outputParList->next;
      if(root->next == NULL)
      {
        if(returnCount!=functionTable[functionIndex]->numOutputParams)
        {
          semanticCorrectnessFlag = 0;
          printf("Line %d:Function expects %d output parameters but returning %d variables\n",root->leafInfo->lineNumber,functionTable[functionIndex]->numOutputParams,returnCount);
          return;
        }
      }
      root = root->next;
      continue;
    }
    int symTableIndex = getVarIndex(functionIndex,root->leafInfo->lexeme);
    if(symTableIndex == -1)
    {
      int globalTableIndex = getGlobalIndex(root->leafInfo->lexeme);
      if(globalTableIndex == -1)
      {
        semanticCorrectnessFlag = 0;
        printf("Line %d:%s is not declared\n",root->leafInfo->lineNumber,root->leafInfo->lexeme);
      }
      else if(globalTable[globalTableIndex].varChanged ==0)
      {
        semanticCorrectnessFlag = 0;
        printf("Line %d:%s returned is not changed in the function\n",root->leafInfo->lineNumber,outputParList->leafInfo->lexeme);
      }
    }
    else
      if(symbolTable[symTableIndex].varChanged == 0)
      {
        semanticCorrectnessFlag = 0;
        printf("Line %d:%s returned is not changed in the function\n",root->leafInfo->lineNumber,outputParList->leafInfo->lexeme);
      }
    returnCount++;
    outputParList = outputParList->next;
    if(root->next == NULL)
    {
      if(returnCount!=functionTable[functionIndex]->numOutputParams)
      {
        semanticCorrectnessFlag = 0;
        printf("Line %d:Function expects %d output parameters but returning %d variables\n",root->leafInfo->lineNumber,functionTable[functionIndex]->numOutputParams,returnCount);
        return;
      }
    }
    root = root->next;
  }
}

void semanticAnalyzer(TreeNode root)
{
  int functionIndex = 0;
  TreeNode function = root->children;
  while(function!=NULL)
  {
    checkFunctionSemantics(function,functionIndex);
    functionIndex++;
    function=function->next;
  }
}

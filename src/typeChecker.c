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
#include "lexer.h"
#include "parser.h"
#include "symbolTable.h"
#include "typeChecker.h"
#include "semanticAnalyzer.h"


//Get the index of global variable
int getGlobalIndex(char* varName)
{
  for(int i=0;i<currGlobalIndex;i++)
    if(strcmp(varName,globalTable[i].symName)==0)
      return i;
  return -1;
}

//Get index of variable from symbol table
int getVarIndex(int funcIndex,char* string)
{
  SymTableEntry t = functionTable[funcIndex]->symTable;
  int symbolTableLen = functionTable[funcIndex]->currIndex;
  for(int i=0;i<symbolTableLen;i++)
    if(strcmp(string,t[i].symName)==0)
      return i;
  return -1;
}

//Get integer type corresponding to the root
int getType(int functionIndex,TreeNode root)
{
  if(root->t == TK_NUM)
    return 0;
  if(root->t==TK_RNUM)
    return 1;
  int symIndex = getVarIndex(functionIndex,root->leafInfo->lexeme);
  int globalIndex = getGlobalIndex(root->leafInfo->lexeme);

  if(symIndex!=-1)
    return functionTable[functionIndex]->symTable[symIndex].type;
  else if(globalIndex!=-1)
    return globalTable[globalIndex].type;

  semanticCorrectnessFlag = 0;
  printf("Line %d: Variable %s not declared before use.\n",root->leafInfo->lineNumber,root->leafInfo->lexeme);
  return -1;
}


//Get string corresponding to the type
char* getTypeString(int type)
{
  if(type == 0)
    return "integer";
  else if(type == 1)
    return "real";
  else
    return recordTable[type-2].recordName;
}



void checkFunctionSemantics(TreeNode root, int functionIndex)
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
    root = root->next;

  checkStatementSemantics(root,functionIndex);

}


void checkAssignmentStatement(TreeNode root, int functionIndex)
{
    SymTableEntry symbolTable = functionTable[functionIndex]->symTable;
    int symTableLen = functionTable[functionIndex]->currIndex;
    TreeNode lhs = root->children;

    int lhsType = -1,rhsType = -1;

    //Not a record field
    if(lhs->next->t == eps)
      {
        lhsType = getType(functionIndex,lhs);
      }
    else{
      int type = getType(functionIndex,lhs);
      if(type==-1)
      {
        return;
      }
      int recIndex = type-2;
      for(int i=0;i<recordTable[recIndex].numFields;i++)
        if(strcmp(recordTable[recIndex].recordFields[i].symName,lhs->next->leafInfo->lexeme) == 0)
          lhsType = recordTable[recIndex].recordFields[i].type;

      if(lhsType == -1)
        {
          semanticCorrectnessFlag = 0;
          printf("Line %d: Record %s has no field %s not defined.\n",lhs->leafInfo->lineNumber,lhs->leafInfo->lexeme,lhs->next->leafInfo->lexeme);
        }
      }

      if(lhsType == -1)
      {
        rhsType = evalArithmeticExpressionType(lhs->next->next,functionIndex);
        return;
      }

      int symTableIndex = getVarIndex(functionIndex,lhs->leafInfo->lexeme);
      if(symTableIndex == -1)
      {
        int globalTableIndex = getGlobalIndex(lhs->leafInfo->lexeme);
        globalTable[globalTableIndex].varChanged = 1;
      }
      else
        symbolTable[symTableIndex].varChanged = 1;


      rhsType = evalArithmeticExpressionType(lhs->next->next->children,functionIndex);

      if(rhsType == -1)
      {
        semanticCorrectnessFlag = 0;
        printf("Line %d: Incorrect arithmetic expression.\n", lhs->leafInfo->lineNumber);
        return;
      }

      if(lhsType!=rhsType)
      {
        semanticCorrectnessFlag = 0;
        printf("Line %d: Type mismatch: Error assigning type %s to %s of type %s\n",lhs->leafInfo->lineNumber,getTypeString(rhsType),lhs->leafInfo->lexeme,getTypeString(lhsType));
        return;
      }

      return;

}



int evalArithmeticExpressionType(TreeNode root,int functionIndex)
{
  if(root->id == terminal)
  {
    if(root->t == TK_ID)
    {
      int symIndex = getVarIndex(functionIndex,root->leafInfo->lexeme);
      if(symIndex!=-1)
        return functionTable[functionIndex]->symTable[symIndex].type;

      semanticCorrectnessFlag = 0;
      printf("Line %d: Variable %s not declared before use.\n",root->leafInfo->lineNumber,root->leafInfo->lexeme);
      return -1;
    }

    if(root->t == TK_NUM)
      return 0;

    if(root->t == TK_RNUM)
      return 1;

    if(root->t == TK_PLUS || root->t == TK_MINUS)
      {
        int leftType = evalArithmeticExpressionType(root->children,functionIndex);
        int rightType = evalArithmeticExpressionType(root->children->next,functionIndex);

        if(leftType == rightType && (leftType == 0 || leftType == 1))
          return leftType;

        if(leftType == rightType && leftType >= 2)
          return leftType;

        return -1;

      }

      if(root->t == TK_MUL || root->t == TK_DIV)
        {
          int leftType = evalArithmeticExpressionType(root->children,functionIndex);
          int rightType = evalArithmeticExpressionType(root->children->next,functionIndex);

          if(leftType == -1 || rightType == -1)
          {
            return -1;
          }

          if(leftType == rightType && (leftType == 0 || leftType == 1))
            return leftType;

          if(root->t == TK_MUL)
          {
            if(leftType>=2 && rightType<2)
              return leftType;

            if(rightType>=2 && leftType<2)
              return rightType;

            if(rightType+leftType >= 4)
              {
                semanticCorrectnessFlag = 0;
                printf("Line %d: Records can't be multipled\n",root->leafInfo->lineNumber);
              }

            return -1;
          }

          if(root->t == TK_DIV)
          {
            if(leftType>=2 && rightType<2)
              return leftType;
            if(rightType>=2)
            {
              semanticCorrectnessFlag = 0;
              printf("Line %d: Cannot divide by a record\n",root->leafInfo->lineNumber);
            }
            return -1;
          }



          return -1;

        }

  }

  if(root->nt == temp)
  {
    int index = getVarIndex(functionIndex,root->children->leafInfo->lexeme);
    if(index == -1)
      {
        //TODO error
        return -1;
      }
    int recordIndex = functionTable[functionIndex]->symTable[index].type-2;
    if(recordIndex < -1)
      return -1;

    for(int i=0;i<recordTable[recordIndex].numFields;i++)
    {
      if(strcmp(root->children->next->leafInfo->lexeme,recordTable[recordIndex].recordFields[i].symName) == 0)
        return recordTable[recordIndex].recordFields[i].type;
    }

    semanticCorrectnessFlag = 0;
    printf("Line %d: Record %s has no field named %s\n",root->children->next->leafInfo->lineNumber,recordTable[recordIndex].recordName, root->children->next->leafInfo->lexeme);
    return -1;
  }
  return -1;

}



void checkBooleanSemantics(TreeNode root,int functionIndex)
{
  if(root->t == TK_LT || root->t == TK_LE || root->t == TK_EQ || root->t == TK_GT || root->t == TK_GE || root->t == TK_NE)
  {
    int lhsType = getType(functionIndex,root->children);
    int rhsType = getType(functionIndex,root->children->next);
    if(lhsType == -1 || rhsType == -1)
      return;
    else if(lhsType == rhsType)
      return;
    else
    {
      semanticCorrectnessFlag = 0;
      printf("Line %d: Type Mismatch -> Comparison of %s of type %s with %s of type %s\n",root->children->leafInfo->lineNumber,root->children->leafInfo->lexeme,getTypeString(lhsType),root->children->next->leafInfo->lexeme,getTypeString(rhsType));
      return;
    }
  }
  if(root->t == TK_AND || root->t == TK_OR)
  {
    checkBooleanSemantics(root->children,functionIndex);
    checkBooleanSemantics(root->children->next,functionIndex);
  }
  if(root->t == TK_NOT)
  {
    checkBooleanSemantics(root->children,functionIndex);
  }

}

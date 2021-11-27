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
#include "codeGen.h"



void reserveMemory(FILE* fp)
{
  labelCount = 0;
  fprintf(fp,"section .bss\n");
  for(int i=0;i<functionTable[0]->currIndex;i++)
    {
      int type = functionTable[0]->symTable[i].type;
      if(type < 2 )
        fprintf(fp,"%s resd 1\n",functionTable[0]->symTable[i].symName);
      else
      {
        for(int j=0;j<recordTable[type].numFields;j++)
          fprintf(fp,"%s.%s resd 1\n",functionTable[0]->symTable[i].symName,recordTable[type-2].recordFields[j].symName);
      }

    }

  for(int i=0;i<currGlobalIndex;i++)
  {
    int type = globalTable[i].type;
    if(type < 2 )
      fprintf(fp,"%s resd 1\n",globalTable[i].symName);
    else
    {
      for(int j=0;j<recordTable[type].numFields;j++)
        fprintf(fp,"%s.%s resd 1\n",globalTable[i].symName,recordTable[type-2].recordFields[j].symName);
    }
  }

  fprintf(fp,"\n\nsection .text\n");

}


void generateStmtCode(TreeNode root, FILE* fp)
{

  TreeNode temp = root->children;
  while(temp!=NULL)
    {
      generateStmtCodeHelper(temp,fp);
      temp = temp->next;
    }
  generateStmtCodeHelper(root->children->children,fp);
}


void generateStmtCodeHelper(TreeNode root, FILE* fp)
{
  if(root->nt == typeDefinition)
    return;

  if(root->nt == declarations)
    return;

  if(root->nt == assignmentStmt)
    generateAssignmentStmt(root,fp);

  if(root->nt == conditionalStmt)
    generateConditionStmt(root,fp);

  // if(root->nt == booleanExpression)
  //   generateBoolean(root,fp);
}


void generateAssignmentStmt(TreeNode root,FILE* fp)
{
  fprintf(fp, "mov ecx localBuff\n");
  int prevOffset = 0;
  int offset  = generateArithmeticStmt(root->children->next->next->children,0,fp);
  int type = getType(0,root->children);
  root = root->children;
  if(type>=2)
  {
    for(int i=0;i<type-2;i++)
    {
      fprintf(fp,"mov eax ecx[%d]\n",4*i);
      fprintf(fp,"mov dword[%s.%s], eax\n",root->leafInfo->lexeme,root->next->leafInfo->lexeme);
    }
  }
  else
  {
    fprintf(fp,"mov eax, dword[ecx]\n");
    fprintf(fp,"mov dword[%s], eax\n",root->leafInfo->lexeme);
  }


}

int generateArithmeticStmt(TreeNode root,int offset,FILE* fp)
{
  if(root->id == terminal)
  {

    if(root->t == TK_NUM || root->t == TK_RNUM)
    {
        fprintf(fp,"mov dword[ecx+%d], %s\n",offset,root->leafInfo->lexeme);
        return offset;
    }

    if(root->t == TK_ID && getType(0,root)<2)
    {
        fprintf(fp, "mov edx, dword[%s]\n",root->leafInfo->lexeme);
        fprintf(fp, "mov dword[ecx+%d], edx\n",offset);
        return offset+4;
    }
    else if(root->t == TK_ID)
    {
        int prevOffset = offset;
        int type = getType(0,root);
        for(int i=0;i<recordTable[type-2].numFields;i++)
        {
          fprintf(fp, "mov edx, dword[%s.%s]",root->leafInfo->lexeme,recordTable[type-2].recordFields[i].symName);
          fprintf(fp, "mov dword[ecx+%d], edx\n",offset);
          offset+=4;
        }
        return offset;
    }

    if(root->t == TK_PLUS)
    {
      int prevOffset = offset;
      offset = generateArithmeticStmt(root->children,offset,fp);
      offset = generateArithmeticStmt(root->children->next,offset,fp);

      for(int i=0;i<(offset-prevOffset)/2;i+=4)
      {
        fprintf(fp,"mov edx, dword[ecx+%d]\n",(prevOffset+offset)/2+i);
        fprintf(fp,"add dword[ecx+%d], edx\n",prevOffset+i);
      }
      return (prevOffset+offset)/2;
    }

    else if(root->t == TK_MINUS)
    {
      int prevOffset = offset;
      offset = generateArithmeticStmt(root->children,offset,fp);
      offset = generateArithmeticStmt(root->children->next,offset,fp);

      for(int i=0;i<(offset-prevOffset)/2;i+=4)
      {
        fprintf(fp,"mov edx, dword[ecx+%d]\n",(prevOffset+offset)/2+i);
        fprintf(fp,"sub dword[ecx+%d], edx\n",prevOffset+i);
      }
      return (offset+prevOffset)/2;
    }

    else if(root->t == TK_DIV)
    {
      int prevOffset = offset;
      offset = generateArithmeticStmt(root->children,offset,fp);
      int recOrIdOffset = offset;
      offset = generateArithmeticStmt(root->children->next,offset,fp);

      fprintf(fp,"mov edx, dword[ecx+%d]\n",recOrIdOffset);

      for(int i=prevOffset;i<recOrIdOffset;i+=4)
        fprintf(fp,"div dword[ecx+%d], edx",i);

      return recOrIdOffset;
    }

    else if(root->t == TK_MUL)
    {
      int prevOffset = offset;
      offset = generateArithmeticStmt(root->children,offset,fp);
      int recOrIdOffset = offset;
      offset = generateArithmeticStmt(root->children->next,offset,fp);

      fprintf(fp,"mov edx, dword[ecx+%d]\n",recOrIdOffset);

      for(int i=prevOffset;i<recOrIdOffset;i+=4)
        fprintf(fp,"div dword[ecx+%d], edx",i);

      return recOrIdOffset;
    }

  }
  else{
        root = root->children;
        fprintf(fp, "mov edx, dword[%s.%s]\n",root->leafInfo->lexeme,root->next->leafInfo->lexeme);
        fprintf(fp, "mov dword[ecx+%d], edx\n",offset);
        return offset+4;
  }
  return 0;
}



void generateConditionStmt(TreeNode root,FILE* fp)
{
  TreeNode boolNode = root->children;
  TreeNode elsePartNode = root->children->next->children;
  TreeNode thenPart = root->children->next->next;

  int label = generateBoolean(boolNode,fp);
  fprintf(fp,"jmp label%d:\n",label);
  fprintf(fp,"pop eax\n");
  fprintf(fp,"pop ebx\n");
  generateStmtCode(elsePartNode,fp);
  fprintf(fp,"label%d:\n",label);
  labelCount++;
  fprintf(fp,"pop eax\n");
  fprintf(fp,"pop ebx\n");
  generateStmtCode(thenPart,fp);
  fprintf(fp, "label%d: \n",labelCount);

}

int generateBoolean(TreeNode root,FILE* fp)
{
  if(root->t == TK_AND)
  {
    int leftLabel = generateBoolean(root->children,fp);
    fprintf(fp,"jmp ");
    fprintf(fp,"label%d\n",leftLabel);

  }
  else if(root->t == TK_OR)
  {

  }
  else if(root->t == TK_NOT)
  {

  }
  else if(root->t == TK_LE)
  {
    fprintf(fp,"push eax\n");
    fprintf(fp,"push ebx\n");
    fprintf(fp,"mov eax,dword[%s]\n",root->children->leafInfo->lexeme);
    fprintf(fp,"mov ebx,dword[%s]\n",root->children->next->leafInfo->lexeme);
    labelCount++;
    fprintf(fp,"jle label%d\n",labelCount);
    return labelCount;
  }
  else if(root->t == TK_GE)
  {
    fprintf(fp,"push eax\n");
    fprintf(fp,"push ebx\n");
    fprintf(fp,"mov eax,dword[%s]\n",root->children->leafInfo->lexeme);
    fprintf(fp,"mov ebx,dword[%s]\n",root->children->next->leafInfo->lexeme);
    labelCount++;
    fprintf(fp,"jge label%d\n",labelCount);
    return labelCount;
  }
  else if(root->t == TK_LT)
  {
    fprintf(fp,"push eax\n");
    fprintf(fp,"push ebx\n");
    fprintf(fp,"mov eax,dword[%s]\n",root->children->leafInfo->lexeme);
    fprintf(fp,"mov ebx,dword[%s]\n",root->children->next->leafInfo->lexeme);
    labelCount++;
    fprintf(fp,"jl label%d\n",labelCount);
    return labelCount;
  }
  else if(root->t == TK_GT)
  {
    fprintf(fp,"push eax\n");
    fprintf(fp,"push ebx\n");
    fprintf(fp,"mov eax,dword[%s]\n",root->children->leafInfo->lexeme);
    fprintf(fp,"mov ebx,dword[%s]\n",root->children->next->leafInfo->lexeme);
    labelCount++;
    fprintf(fp,"jg label%d\n",labelCount);
    return labelCount;
  }
  return 0;
}

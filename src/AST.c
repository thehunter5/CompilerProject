#include<stdio.h>
#include<stdlib.h>
#include "parser.h"


void createAST(TreeNode root)
{

  n1 = n1+1;

  if(root->id == terminal)
    return;

  TreeNode temp = root->children;

  //Post-order
  while(temp!=NULL)
    {
      createAST(temp);
      temp=temp->next;
    }


  switch(root->ruleNum)
  {

    //program -> otherFunctions mainFunction
    //program->children = otherFunctions->children; program->children->next = mainFunction
    case 0:
    {
        TreeNode temp = root->children->next;
        TreeNode otherFunctionsNode = root->children;
        TreeNode mainFunctionNode = root->children->next;

        //If no other functions
        if(root->children->children != NULL)
          {
            root->children = root->children->children;
            TreeNode t = root->children;

            while(t->next!=NULL)
            {
              t = t->next;
            }

            t->next = mainFunctionNode;
          }
        else
          root->children = mainFunctionNode;

        free(otherFunctionsNode);
        n2++;
        break;
    }

    //mainFunction -> TK_MAIN stmts TK_END
    //mainFunction->children = stmts->children
    case 1:
    {
      TreeNode stmtsNode = root->children->next;
      TreeNode mainNode = root->children;
      TreeNode endNode = root->children->next->next;

      free(mainNode);
      n2++;
      free(endNode);
      n2++;
      root->children = stmtsNode->children;
      free(stmtsNode);
      n2++;
      break;
    }

    //otherFunctions -> function otherFunctions
    case 2:
    {
      TreeNode functionNode = root->children;
      TreeNode otherFunctionsNode = root->children->next;

      functionNode->next = otherFunctionsNode->children;
      free(otherFunctionsNode);
      n2++;
      break;
    }

    //otherFunctions -> eps
    case 3:
    {
      free(root->children);
      n2++;
      root->children = NULL;
      break;
    }

    //function -> TK_FUNID input_par output_par TK_SEM stmts TK_END
    case 4:
    {
      TreeNode ipNode = root->children->next;
      TreeNode opNode = ipNode->next;
      TreeNode stmtsNode = opNode->next->next;

      //free(root->children);
      //root->children = ipNode;

      free(opNode->next);
      n2++;
      opNode->next = stmtsNode->children;

      free(stmtsNode->next);
      n2++;
      //stmtsNode->children->next = NULL;
      free(stmtsNode);
      n2++;

      break;
    }

    // input_par -> TK_INPUT TK_P TK_L TK_SQL parameter_list TK_SQR
    case 5:
    {
        TreeNode paramListNode = root->children->next->next->next->next;

        free(root->children->next->next);
        free(root->children->next);
        free(root->children);

        root->children = paramListNode->children;
        free(paramListNode->next);
        free(paramListNode);
        n2+= 5;
        //paramListNode->next = NULL;
        break;
    }


    // output_par -> TK_OP TK_PARAM TK_LIST TK_SQL parameter_list TK_SQR
    case 6:
    {
      TreeNode paramListNode = root->children->next->next->next->next;

      free(root->children->next->next);
      free(root->children->next);
      free(root->children);

      root->children = paramListNode->children;
      free(paramListNode->next);
      free(paramListNode);
      n2+= 5;
      //paramListNode->next = NULL;
      break;
    }

    // output_par -> eps
    case 7:
    {
      free(root->children);
      n2++;
      root->children = NULL;
      break;
    }

    //param_list -> dataType ID remList
    case 8:
    {
      TreeNode dataTypeNode = root->children;
      TreeNode IDNode = dataTypeNode->next;
      TreeNode remListNode = IDNode->next;

      root->children = dataTypeNode->children;
      root->children->next = IDNode;
      IDNode->next = remListNode->children;

      free(remListNode);
      free(dataTypeNode);
      n2+= 2;
      break;

    }


    case 9:
    case 10:
    {
      TreeNode dt = root->children;
      TreeNode type = root->children->children;

      root->children = type;

      free(dt);
      n2++;
      break;
    }

    //<primitiveDatatype> ===> TK_INT
    //<primitiveDatatype> ===> TK_REAL
    case 11:
    case 12:
    {
      //do nothing?
      break;
    }

    //<constructedDatatype> ===> TK_RECORD  TK_RECORDID
    case 13:
    {
      TreeNode record = root->children;
      TreeNode recordId = root->children->next;

      root->children = recordId;

      free(record);
      n2++;
      break;
    }

    //<remaining_list> ===> TK_COMMA <parameter_list>
    case 14:
    {
      TreeNode comma = root->children;
      TreeNode paraList = root->children->next;

      root->children = paraList->children;

      free(comma);
      free(paraList);
      n2+= 2;
      break;
    }

    //<remaining_list> ===> eps
    case 15:
    {
      free(root->children);
      n2++;
      root->children = NULL;
      break;
    }

    // <stmts> ===> <typeDefinitions> <declarations> <otherStmts> <returnStmt>
    case 16:
    {
      TreeNode typeDef = root->children;
      TreeNode dec = root->children->next;
      TreeNode os = root->children->next->next;
      TreeNode ret = root->children->next->next->next;

      if(typeDef->children == NULL)
      {
        if(dec->children == NULL)
        {
          if(os->children == NULL)
          {
            root->children = ret;
          }
          else
          {
            root->children = os->children;
            TreeNode temp = os->children;
            while(temp->next!=NULL)
            {
              temp=temp->next;
            }
            temp->next=ret;
          }
          free(dec);
          free(os);
          n2+= 2;
        }
        else
        {
          root->children = dec;
          if(os->children == NULL)
          {
            root->children->next = ret;
            free(os);
            n2++;
          }
          else
          {
            root->children->next = os->children;
            TreeNode temp = os->children;
            while(temp->next!=NULL)
            {
              temp=temp->next;
            }
            temp->next=ret;
          }
        }
        free(typeDef);
        n2++;
      }
      else
      {
        root->children = typeDef;
        if(dec->children == NULL)
        {
          if(os->children == NULL)
          {
            root->children->next = ret;
          }
          else
          {
            root->children->next = os->children;
            TreeNode temp = os->children;
            while(temp->next!=NULL)
            {
              temp=temp->next;
            }
            temp->next=ret;
          }
          free(dec);
          free(os);
          n2+=2;
        }
        else
        {
          root->children->next = dec;
          if(os->children == NULL)
          {
            root->children->next->next = ret;
            free(os);
            n2++;
          }
          else
          {
            root->children->next->next = os->children;
            TreeNode temp = os->children;
            while(temp->next!=NULL)
            {
              temp=temp->next;
            }
            temp->next=ret;
          }
        }
      }
      break;
    }

    // <typeDefinitions> ===> <typeDefinition> <typeDefinitions>
    case 17:
    {
      TreeNode td = root->children;
      TreeNode tds = root->children->next;

      TreeNode rec = td->children;
      TreeNode fd = rec->next;
      root->children = rec;
      rec->next = tds->children;
      rec->children = fd;
      // td->next = tds->children;
      free(tds);
      free(td);
      n2+= 2;
      break;
    }

    // <typeDefinitions> ===>  eps
    case 18:
    {
      free(root->children);
      n2++;
      root->children = NULL;
      break;
    }

    //<typeDefinition> ===> TK_RECORD TK_RECORDID <fieldDefinitions> TK_ENDRECORD TK_SEM
    case 19:
    {
      TreeNode recid = root->children->next;
      TreeNode fds = root->children->next->next;

      free(root->children->next->next->next->next);
      free(root->children->next->next->next);
      free(root->children);

      root->children = recid;
      root->children->next = fds->children;

      free(fds);
      n2+= 4;
      break;
    }

    // <fieldDefinitions> ===> <fieldDefinition> <fieldDefinition> <moreFields>
    case 20:
    {
      TreeNode fd1 = root->children;
      TreeNode fd2 = root->children->next;
      TreeNode moreFd = root->children->next->next;

      root->children = fd1->children;
      root->children->next->next = fd2->children;
      root->children->next->next->next->next = moreFd->children;

      free(fd1);
      free(fd2);
      free(moreFd);
      n2+= 3;
      break;
    }

    // <fieldDefinition> ===> TK_TYPE <primitiveDatatype> TK_COLON TK_FIELDID TK_SEM
    case 21:
    {
      TreeNode pdt = root->children->next->children;
      TreeNode fid = root->children->next->next->next;

      free(root->children->next->next->next->next);
      free(root->children->next->next);
      free(root->children->next);
      free(root->children);
      n2+= 4;
      root->children = pdt;
      pdt->next = fid;
      fid->next = NULL;
      break;
    }

    //<moreFields> ===> <fieldDefinition> <moreFields>
    case 22:
    {
      TreeNode fd = root->children;
      TreeNode mfd = root->children->next;

      root->children = fd->children;
      fd->children->next->next = mfd->children;

      free(fd);
      free(mfd);
      n2+= 2;
      break;
    }

    //<moreFields> ===> eps
    // <declarations> ===> eps
    // <otherStmts> ===> eps
    case 30:
    case 25:
    case 23:
    {
      free(root->children);
      n2++;
      root->children = NULL;
      break;
    }

    //<declarations> ===> <declaration> <declarations>
    case 24:
    {
      TreeNode dc = root->children;
      TreeNode dcs = root->children->next;

      root->children = dc->children;
      dc->children->next->next->next = dcs->children;
      // td->next = tds->children;
      free(dcs);
      free(dc);
      n2+= 2;
      break;
    }

    // <declaration> ===> TK_TYPE <dataType> TK_COLON TK_ID  <global_or_not> TK_SEM
    case 26:
    {
      TreeNode dt = root->children->next;
      TreeNode id = root->children->next->next->next;
      TreeNode gl = id->next;

      free(root->children->next->next->next->next->next);
      free(root->children->next->next);
      free(root->children);
      root->children =dt->children;
      dt->children->next = id;
      id->next = gl->children;
      gl->children->next = NULL;

      free(dt);
      free(gl);
      n2+= 5;
      break;

    }

    //<global_or_not> ===> TK_COLON  TK_GLOBAL
    case 27:
    {
      TreeNode tkg = root->children->next;
      free(root->children);
      n2++;
      root->children = tkg;
      tkg->next = NULL;
      break;
    }

    //<global_or_not> ===> eps
    case 28:
    {
      //keep the eps node so as to differentiate between global and local
      break;
    }

    //<otherStmts> ===> <stmt> <otherStmts>
    case 29:
    {
      TreeNode stmt = root->children;
      TreeNode os = root->children->next;

      root->children = stmt->children;
      stmt->children->next = os->children;

      free(stmt);
      free(os);
      n2+= 2;
      break;
    }

    /*
    <stmt> - <assignmentStmt>
    <stmt> - <iterativeStmt>
    <stmt> - <conditionalStmt>
    <stmt> - <ioStmt>
    <stmt> - <funCallStmt>
    */
    case 31:
    case 32:
    case 33:
    case 34:
    case 35:
    {
      //do nothing?
      break;
    }

    // <assignmentStmt> ===> <SingleOrRecId> TK_ASSIGNOP  <arithmeticExpression> TK_SEM
    case 36:
    {
      TreeNode sor = root->children;
      TreeNode ae = root->children->next->next;
      free(ae->next);
      free(root->children->next);

      root->children = sor->children;
      sor->children->next->next = ae;
      ae->next = NULL;

      free(sor);
      n2+= 3;
      break;
    }

    //<singleOrRecId> - TK_ID <new_24>
    case 37:
    {
      TreeNode n = root->children->next;
      root->children->next = n->children;
      n->children->next = NULL;

      free(n);
      n2++;
      break;
    }
    //<new_24> - eps
    case 38:
    {
      //keep the node to diff from id or id.fieldid
      break;
    }

    //<new_24> - TK_DOT TK_FIELDID
    case 39:
    {
      TreeNode n = root->children->next;
      free(root->children);
      n2++;
      root->children = n;
      n->next = NULL;
      break;
    }

    //<funCallStmt> - <outputParameters> TK_CALL TK_FUNID TK_WITH TK_PARAMETERS <inputParameters> TK_SEM
    case 40:
    {
      TreeNode op = root->children;
      TreeNode fid = root->children->next->next;
      TreeNode ip = fid->next->next->next;
      free(root->children->next->next->next->next->next->next);
      free(root->children->next->next->next->next);
      free(root->children->next->next->next);
      free(root->children->next);
      n2+= 4;
      root->children = fid;
      fid->next = ip;
      ip->next = op;
      op->next = NULL;

      break;
    }
    //<outputParameters> - TK_SQL <idList> TK_SQR TK_ASSIGNOP
    case 41:
    {
      TreeNode idl = root->children->next;
      free(root->children->next->next->next);
      free(root->children->next->next);
      free(root->children);
      n2+= 3;
      root->children = idl->children;
      break;
    }
    //<outputParameters> - eps
    case 42:
    {
      free(root->children);
      n2++;
      root->children = NULL;
      break;
    }
    //<inputParameters> - TK_SQL <idList> TK_SQR
    case 43:
    {
      TreeNode idl = root->children->next;
      free(root->children->next->next);
      free(root->children);
      n2+= 2;
      root->children = idl->children;
      break;
    }

    //<iterativeStmt> - TK_WHILE TK_OP <booleanExpression> TK_CL <stmt> <otherStmts> TK_ENDWHILE
    case 44:
    {
      TreeNode be = root->children->next->next;
      TreeNode st = root->children->next->next->next->next;
      TreeNode ost = st->next;

      TreeNode endwhileNode = root->children->next->next->next->next->next->next;
      free(root->children->next->next->next);
      free(root->children->next);
      free(root->children);

      root->children = be;
      be->next = st->children;
      st->children->next = ost->children;

      TreeNode temp = st->children;
      while(temp->next!=NULL)
        temp = temp->next;
      temp->next = endwhileNode;
      free(st);
      free(ost);

      n2+= 5;
      break;
    }
    //<conditionalStmt> - TK_IF TK_OP <booleanExpression> TK_CL TK_THEN <stmt> <otherStmts> <elsePart>
    case 45:
    {
      TreeNode be = root->children->next->next;
      TreeNode st = root->children->next->next->next->next->next;
      TreeNode ost = st->next;
      TreeNode elsePart = ost->next;

      free(root->children->next->next->next->next);
      free(root->children->next->next->next);
      free(root->children->next);
      free(root->children);

      root->children = be;
      be->next = elsePart;
      elsePart->next = st->children;
      st->children->next = ost->children;
      free(st);
      free(ost);
      n2+= 6;
      break;
    }
    //<elsePart> - TK_ELSE <stmt> <otherStmts> TK_ENDIF
    case 46:
    {
      TreeNode st = root->children->next;
      TreeNode ost = st->next;

      free(root->children->next->next->next);
      free(root->children);

      root->children = st->children;
      st->children->next = ost->children;
      free(ost);
      free(st);
      n2+= 4;
      break;
    }
    //<elsePart> - TK_ENDIF
    case 47:
    {
      free(root->children);
      root->children = NULL;
      n2++;
      break;
    }

    //<ioStmt> - TK_READ TK_OP <singleOrRecId> TK_CL TK_SEM
    case 48:
    {
      TreeNode read = root->children;
      TreeNode sor = root->children->next->next;

      free(root->children->next->next->next->next);
      free(root->children->next->next->next);
      free(root->children->next);

      root->children = read;
      read->next = sor->children;
      free(sor);
      n2+= 4;
      break;
    }

    //<ioStmt> - TK_WRITE TK_OP <allVar> TK_CL TK_SEM
    case 49:
    {
      TreeNode writeNode = root->children;
      TreeNode alvarNode = root->children->next->next;

      free(root->children->next->next->next->next);
      free(root->children->next->next->next);
      free(root->children->next);

      root->children = writeNode;
      writeNode->next = alvarNode->children;
      free(alvarNode);

      n2+= 4;
      break;
    }

    // <arithmeticExpression> - <term> <expPrime>

    // <expPrime> - <lowPrecedenceOperators> <term> <expPrime>
    // <expPrime> - eps
    // <term> - <factor> <termPrime>
    // <termPrime> - <highPrecedenceOperators> <factor> <termPrime>
    // <termPrime> - eps
    // <factor> - TK_OP <arithmeticExpression> TK_CL
    // <factor> - <all>
    // <highPrecedenceOperators> - TK_MUL
    // <highPrecedenceOperators> - TK_DIV
    // <lowPrecedenceOperators> - TK_PLUS
    // <lowPrecedenceOperators> - TK_MINUS
    // <all> - TK_ID <temp>
    // <all> - TK_NUM
    // <all> - TK_RNUM
    // <temp> - eps
    // <temp> - TK_DOT TK_FIELDID


    // <booleanExpression> - TK_OP <booleanExpression> TK_CL <logicalOp> TK_OP <booleanExpression> TK_CL
    case 72:
    {
      TreeNode be1 = root->children->next->children;
      TreeNode op = root->children->next->next->next->children;
      TreeNode be2 = root->children->next->next->next->next->next->children;

      free(root->children->next->next->next->next->next->next);
      free(root->children->next->next->next->next->next);
      free(root->children->next->next->next->next);
      free(root->children->next->next->next);
      free(root->children->next->next);
      free(root->children->next);
      free(root->children);

      n2+= 7;
      root->children = op;
      op->next =NULL;
      op->children = be1;
      be1->next = be2;
      be2->next =NULL;
      break;
    }
    // <booleanExpression> - <var> <relationalOp> <var>
    case 73:
    {
      TreeNode op = root->children->next->children;
      TreeNode v1 = root->children;
      TreeNode v2 = root->children->next->next;
      free(root->children->next);
      root->children = op;
      op->children = v1->children;
      op->children->next = v2->children;

      free(v1);
      free(v2);
      n2+= 3;
      break;
    }
    // <booleanExpression> - TK_NOT TK_OP <booleanExpression> TK_CL
    case 74:
    {
      TreeNode nt = root->children;
      TreeNode be = root->children->next->next->children;

      free(root->children->next->next->next);
      free(root->children->next->next);
      free(root->children->next);

      n2+= 3;
      root->children = nt;
      nt->next =NULL;
      nt->children = be;
      be->next =NULL;
      break;
    }



    // <var> - TK_ID
    // <var> - TK_NUM
    // <var> - TK_RNUM
    // <logicalOp> - TK_AND
    // <logicalOp> - TK_OR
    // <relationalOp> - TK_LT
    // <relationalOp> - TK_LE
    // <relationalOp> - TK_EQ
    // <relationalOp> - TK_GT
    // <relationalOp> - TK_GE
    // <relationalOp> - TK_NE
    // case :
    // case :

    // {
    //   //do nothing
    //   break;
    // }

    //arithmeticExpression -> term expPrime
    case 55:
    {
      TreeNode expPrimeNode = root->children->next;
      TreeNode tempNode = root->children->children;
      free(root->children);
      n2++;
      if(expPrimeNode->children == NULL)
      {
        free(expPrimeNode);
        n2++;
        root->children = tempNode;
      }
      else{
        TreeNode lop = expPrimeNode->children;
        TreeNode t = lop->next;

        root->children = lop;
        lop->next = NULL;
        lop->children = tempNode;
        tempNode->next = t;
        t->next = NULL;
        free(expPrimeNode);
        n2++;
      }
      break;
    }

    // exPrime -> lOP term exPrime
    case 56:
    {
      TreeNode opNode = root->children->children;
      TreeNode varNode = root->children->next->children;
      TreeNode expNode = root->children->next->next;
      free(root->children->next);
      free(root->children);
      if(expNode->children == NULL)
      {
        free(expNode);
        n2++;
        root->children = opNode;
        opNode->next = varNode;
        varNode->next = NULL;
      }
      else
      {
        TreeNode op = expNode->children;
        TreeNode f = op->next;
        root->children =opNode;
        opNode->next = op;
        op->next = NULL;
        op->children = varNode;
        varNode->next = f;
        f->next = NULL;
        free(expNode);
        n2++;
      }
      break;
    }
    //term ->  factor termPrime
    case 58:
    {
      TreeNode termPrimeNode = root->children->next;
      TreeNode f = root->children->children;
      free(root->children);
      if(termPrimeNode->children == NULL)
      {
        free(termPrimeNode);
        n2++;
        root->children = f;
        f->next = NULL;
        break;
      }
      else
      {
          TreeNode opNode = termPrimeNode->children;
          TreeNode varNode = opNode->next;

          root->children = opNode;
          opNode->next = NULL;
          opNode->children = f;
          //varNode->children = NULL;
          f->next = varNode;
          varNode->next = NULL;
          free(termPrimeNode);
          n2++;
      }
      break;
    }

    //<termPrime> - <highPrecedenceOperators> <factor> <termPrime>
    case 59:{
      TreeNode opNode = root->children->children;
      TreeNode varNode = root->children->next->children;
      TreeNode tpNode = root->children->next->next;
      free(root->children->next);
      free(root->children);
      n2+= 2;
      if(tpNode->children == NULL)
      {
        free(tpNode);
        n2++;
        root->children =opNode;
        opNode->next =varNode;
        varNode->next = NULL;
      }
      else
      {
        TreeNode op = tpNode->children;
        TreeNode f = op->next;
        root->children = opNode;
        opNode->next = op;
        op->next = NULL;
        op->children = varNode;
        varNode->next = f;
        f->next = NULL;
        free(tpNode);
        n2++;
      }
      break;
    }

    //termPrime -> N
    case 57:
    case 60:
    {
      free(root->children);
      n2++;
      root->children = NULL;
      break;
    }

    //<factor> - TK_OP <arithmeticExpression> TK_CL
    case 61:
    {
      TreeNode aeNode = root->children->next;
      free(root->children);
      free(aeNode->next);
      root->children = aeNode->children;
      free(aeNode);
      n2+= 3;
      break;
    }

    //factor -> all
    case 62:
    {
      TreeNode allNode = root->children;
      root->children = allNode->children;
      free(allNode);
      n2++;
      break;
    }




    //High and Low precedence operators
    case 63:
    case 64:
    case 65:
    case 66:
    {
      // TreeNode precNode = root->children;
      // root->children = precNode->children;
      // free(precNode);
      break;
    }


    case 50:
    case 67:
    {
      TreeNode tempNode = root->children->next;
      TreeNode id = root->children;
      if(tempNode->children == NULL)
      {
        free(tempNode);
        n2++;
        root->children->next = NULL;
      }
      else
      {
        TreeNode f = tempNode->children;
        root->children = tempNode;
        tempNode->next =NULL;
        tempNode->children = id;
        id->next = f;
        f->next = NULL;

      }
      break;
    }
    case 53:
    //<temp> - TK_DOT TK_FIELDID
    case 71:
    {
      TreeNode f = root->children->next;
      free(root->children);
      n2++;
      root->children = f;
      f->next =NULL;
      break;
    }

    case 54:
    //temp -> eps
    case 70:
    {
      free(root->children);
      n2++;
      root->children = NULL;
      break;
    }
    // <returnStmt> - TK_RETURN <optionalReturn> TK_SEM
    case 86:
    {
      TreeNode or = root->children->next;
      free(root->children->next->next);
      free(root->children);

      root->children = or->children;
      free(or);
      n2+= 3;
      break;
    }
    // <optionalReturn> - TK_SQL <idList> TK_SQR
    case 87:
    {
      TreeNode idl = root->children->next;
      free(root->children->next->next);
      free(root->children);

      root->children = idl->children;
      free(idl);
      n2+= 3;
      break;
    }
    // <optionalReturn> - eps
    case 88:
    {
      free(root->children);
      n2++;
      root->children = NULL;
      break;
    }
    // <idList> - TK_ID <more_ids>
    case 89:
    {
      TreeNode id = root->children;
      TreeNode mid = root->children->next;
      root->children = id;
      id->next = mid->children;
      free(mid);
      n2++;
      break;
    }
    // <more_ids> - TK_COMMA <idList>
    case 90:
    {
      TreeNode idl = root->children->next;
      free(root->children);
      root->children = idl->children;
      free(idl);
      n2+= 2;
      break;
    }
    // <more_ids> - eps
    case 91:
    {
      free(root->children);
      n2++;
      root->children = NULL;
      break;
    }
    default :break;

  }

}

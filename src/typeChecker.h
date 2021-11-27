/*
 Group - 5
 Vikram Waradpande - 2015B4A70454P
 Rinkesh Jain - 2015B4A70590P
 Yajat Dawar - 2015B4A70620P
 Anmol Naugaria - 2015B4A70835P
*/

void checkFunctionSemantics(TreeNode root, int functionIndex);
int evalArithmeticExpressionType(TreeNode root,int functionIndex);
void checkAssignmentStatement(TreeNode root, int functionIndex);
int getGlobalIndex(char* varName);
int getVarIndex(int funcIndex,char* string);
int getType(int functionIndex,TreeNode root);
char* getTypeString(int type);
void checkBooleanSemantics(TreeNode root,int functionIndex);
int checkIfVarUpdated(char* name,TreeNode stmtNode);
int isValidWhile(TreeNode root,TreeNode stmtNode);

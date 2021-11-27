/*
 Group - 5
 Vikram Waradpande - 2015B4A70454P
 Rinkesh Jain - 2015B4A70590P
 Yajat Dawar - 2015B4A70620P
 Anmol Naugaria - 2015B4A70835P
*/

void reserveMemory(FILE* fp);
void generateStmtCode(TreeNode root,FILE *fp);
void generateStmtCodeHelper(TreeNode root,FILE* fp);
void generateAssignmentStmt(TreeNode root,FILE* fp);
int generateArithmeticStmt(TreeNode root,int offset,FILE* fp);
void generateConditionStmt(TreeNode root,FILE* fp);
int generateBoolean(TreeNode root,FILE* fp);

int labelCount;

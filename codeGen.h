#include "astDef.h"
#include "symbolTableDef.h"

void printLeaf(ASTNode* leaf, FILE* fp);
void generateCode(ASTNode* root, symbolTable* symT, FILE* fp);
void genExpr(ASTNode *astNode, FILE *fp, bool firstCall, int lr, gSymbol expType);
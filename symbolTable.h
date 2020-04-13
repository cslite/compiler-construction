// Group Number: 31
// MADHUR PANWAR   2016B4A70933P
// TUSSANK GUPTA   2016B3A70528P
// SALMAAN SHAHID  2016B4A70580P
// APURV BAJAJ     2016B3A70549P
// HASAN NAQVI     2016B5A70452P
#include <stdio.h>
#include "astDef.h"
#include "symbolTableDef.h"
#include "error.h"

#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

//______________________________
// both currently reside in printSymTable.c, to move to symbolTable.c post testing
void printSymbolTable(symbolTable* st, FILE *fp);
void printSymbolTable2(symbolTable *st, FILE *fp);
void getSymNode(symTableNode *node, char *pstr);
// returns a sample symbolTable structure with 3 functions in total (2 hashed to same slot) and 1 in different slot
void printCurrSymTable(symbolTable *st,int level, FILE *fp);
void printCurrentSymTable(symTableNode *stn, FILE *fp);
// gets the symTableNode entry corresponding to a function and prints scope hierarchy of that function
//______________________________


//TODO: put all the declarations here once symbolTable.c is done
void initSymFuncInfo(symFuncInfo *funcInfo, char *funcName);
void initSymVarInfo(symVarInfo *varInfo);
int getSizeByType(gSymbol gs);
void throwSemanticError(unsigned int lno, char* errStr1, char *errStr2, SemanticErrorType errorType);
void setAssignedOutParam(paramOutNode *outNode);
symbolTable *newScope(symbolTable *currST);
bool matchStaticBounds(symTableNode *passedParam, paramInpNode *inplistNode, unsigned int lno);
bool matchDataType(symTableNode *passedOrGot, unsigned int lno, symTableNode *plistNode, pListType pt);
void checkModuleSignature(ASTNode *moduleReuseNode, symFuncInfo *funcInfo, symbolTable *currST);
void initVarType(varType *vt);
varType getVtype(ASTNode *typeOrDataTypeNode, symFuncInfo *funcInfo, symbolTable *currST);
paramInpNode *inpListSearchID(ASTNode *idNode, symFuncInfo *funcInfo);
paramOutNode *outListSearchID(ASTNode *idNode, symFuncInfo *funcInfo);
paramInpNode *createParamInpNode(ASTNode *idNode, ASTNode *dataTypeNode,symFuncInfo *funcInfo);
paramInpNode *createParamInpList(ASTNode *inputPlistNode);
paramOutNode *createParamOutNode(ASTNode *idNode, ASTNode *dataTypeNode,symFuncInfo *funcInfo);
paramOutNode *createParamOutList(ASTNode *outputPlistNode);
symTableNode* findType(ASTNode* node, symbolTable* currST, symFuncInfo* funcInfo, int* isVar, gSymbol* ty);
bool assignIDinScope(ASTNode *idNode, symFuncInfo *funcInfo, symbolTable *currST);
bool useIDinScope(ASTNode *idNode, symFuncInfo *funcInfo, symbolTable *currST);
void handleModuleDeclaration(ASTNode *moduleIDNode);
symTableNode *checkIDInScopesAndLists(ASTNode *idNode, symFuncInfo *funcInfo, symbolTable *currST, bool assign);
void handleIOStmt(ASTNode *ioStmtNode, symFuncInfo *funcInfo, symbolTable *currST);
void handleModuleReuse(ASTNode *moduleReuseNode, symFuncInfo *funcInfo, symbolTable *currST);
bool boundsCheckIfStatic(ASTNode *idNode, ASTNode *idOrNumNode, symFuncInfo *funcInfo, symbolTable *currST);
void handleExpression(ASTNode *someNode, symFuncInfo *funcInfo, symbolTable *currST);
void handleAssignmentStmt(ASTNode *assignmentStmtNode, symFuncInfo *funcInfo, symbolTable *currST);
void handleSimpleStmt(ASTNode *simpleStmtNode, symFuncInfo *funcInfo, symbolTable *currST);
void handleDeclareStmt(ASTNode *declareStmtNode, symFuncInfo *funcInfo, symbolTable *currST);
void handleConditionalStmt(ASTNode *conditionalStmtNode, symFuncInfo *funcInfo, symbolTable *currST);
void handleIterativeStmt(ASTNode *iterativeStmtNode, symFuncInfo *funcInfo, symbolTable *currST);
void handleStatements(ASTNode *statementsNode, symFuncInfo *funcInfo, symbolTable *currST);
void handleModuleDef(ASTNode *startNode, symFuncInfo *funcInfo);
void handlePendingCalls(symFuncInfo *funcInfo);
void handleOtherModule(ASTNode *moduleNode);
void handleUndefinedModules();
void buildSymbolTable(ASTNode *root);



#endif //SYMBOLTABLE_H

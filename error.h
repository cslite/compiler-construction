// Group Number: 31
// MADHUR PANWAR   2016B4A70933P
// TUSSANK GUPTA   2016B3A70528P
// SALMAAN SHAHID  2016B4A70580P
// APURV BAJAJ     2016B3A70549P
// HASAN NAQVI     2016B5A70452P

#ifndef ERROR_H
#define ERROR_H

#include "parserDef.h"
#include "lexerDef.h"
#include "config.h"

typedef enum {
    NO_ERROR, SYNTAX_TT, SYNTAX_NTT, LEXICAL,STACK_NON_EMPTY, E_SEMANTIC, E_TYPE_MISMATCH, E_EXPRESSION_ERROR
} ErrorType;

typedef enum{
    // codes beginning with 'PARAM_PASS' are related to parameter passing (moduleReuse)
    SEME_REDECLARATION,
    SEME_UNDECLARED,
    SEME_UNASSIGNED,
    SEME_REDUNDANT_DECLARATION,
    SEME_OUT_OF_BOUNDS,
    SEME_REDEFINITION,
    SEME_RECURSION,
    SEME_INVALID_BOUNDS,
    SEME_PARAM_PASS_ARR_LBOUND_MISMATCH,
    SEME_PARAM_PASS_ARR_RBOUND_MISMATCH,
    SEME_PARAM_PASS_VAR_BASE_TYPE_MISMATCH,
    SEME_PARAM_PASS_ARR_BASE_TYPE_MISMATCH,
    SEME_PARAM_PASS_ARR_TO_VAR,
    SEME_PARAM_PASS_VAR_TO_STAT_ARR,
    SEME_PARAM_PASS_VAR_TO_DYN_L_ARR,
    SEME_PARAM_PASS_VAR_TO_DYN_R_ARR,
    SEME_PARAM_PASS_VAR_TO_DYN_ARR,
    SEME_PARAM_RECV_VAR_IN_ARR,
    SEME_PARAM_RECV_VAR_BASE_TYPE_MISMATCH,
    SEME_PARAM_RECV_TOO_MANY_RET_VALS_EXPECTED,
    SEME_PARAM_RECV_TOO_FEW_RET_VALS_EXPECTED,
    SEME_PARAM_PASS_TOO_MANY_ARGS_PASSED,
    SEME_PARAM_PASS_TOO_FEW_ARGS_PASSED,
    SEME_LOOP_VAR_REDEFINED,
    SEME_MODULE_REDECLARED,
    SEME_LOOP_VAR_REDECLARED,
    SEME_SWITCH_VAR_UNDECLARED,
    SEME_SWITCH_VAR_TYPE_INVALID,
    SEME_SWITCH_VAR_TYPE_ARR,
    SEME_DEFAULT_IN_BOOLEAN_SWITCH,
    SEME_NON_BOOLEAN_IN_SWITCH,
    SEME_TOO_MANY_BOOLEAN_CASES_IN_SWITCH,
    SEME_MISSING_DEFAULT_IN_INTEGER_SWITCH,
    SEME_NON_INTEGER_IN_SWITCH,
    SEME_FOR_VAR_UNDECLARED,
    SEME_FOR_VAR_TYPE_ARR,
    SEME_FOR_VAR_TYPE_INVALID,
    SEME_RETURN_VALUES_NOT_CAPTURED,
    SEME_ARR_IDX_NOT_INT,
    SEME_MODULE_USED_NOT_DEFINED,
    SEME_FUNCTION_NOT_FOUND,
    SEME_NOT_A_ARRAY,
    SEME_WHILE_COND_TYPE_MISMATCH,
    SEME_WHILE_COND_VARS_UNASSIGNED,
    SEME_FOR_RANGE_L_NUM_EXCEED_R_NUM,
    SEME_DUPLICATE_CASE//add others here
} SemanticErrorType;

typedef struct{
    tokenInfo *tkinfo;
    gSymbol stackTopSymbol;
} syntaxError;

typedef struct{
    char *errTk;
} lexicalError;

typedef struct{
    SemanticErrorType etype;
    char errStr1[ERROR_STRING_LENGTH];
    char errStr2[ERROR_STRING_LENGTH];
} semanticError;

typedef struct{
    ErrorType errType;
    unsigned int lno;
    union {
        syntaxError se;
        lexicalError le;
        semanticError seme;
    } edata;

} error;

void foundNewError(error e);
void initErrorStack();
void printAllErrors();
void destroyErrorStack();
void printError(error e);
#endif //ERROR_H

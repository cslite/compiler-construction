// Group Number: 31
// MADHUR PANWAR   2016B4A70933P
// TUSSANK GUPTA   2016B3A70528P
// SALMAAN SHAHID  2016B4A70580P
// APURV BAJAJ     2016B3A70549P
// HASAN NAQVI     2016B5A70452P

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "hash.h"
#include "parserDef.h"
#include "parser.h"
#include "set.h"
#include "util.h"
#include "config.h"
#include "lexer.h"
#include "errorPtr_stack.h"
#include "typeCheck.h"

#include "astDef.h"
#include "ast.h"
#include "symbolHash.h"
#include "symbolTable.h"
#include "archive.h"
#include "codeGen.h"



//destroy trees, close if open, do not close STDOUT

// Variables defined in lexer.c
extern unsigned int fp;
extern unsigned int bp;
extern unsigned int line_number;
extern int status;
extern int count;
extern hashTable *keyword_ht;

// Variables defined in parser.c
extern grammarNode *grammarArr;
extern struct hashTable *mt;
extern intSet* firstSet;
extern intSet* followSet;
extern char *inverseMappingTable[];
extern int **parseTable;
errorPtr_stack *errorStack;

// error flags
extern bool stage1ErrorFree;
extern bool haveSemanticErrors;

// variables defined in symbolTable.c
extern symbolTable funcTable;

int main(int argc, char *argv[]) {
    if(argc != 3) {
        printf("Usage: %s <source code file> <assembly code output file>\n", argv[0]);
        exit(1);
    }



//    printf("\n######################## STAGE 2 INFO ########################\n");
    printf("\n\t LEVEL 4: AST / Symbol Table / Type Checking / Semantic Rules modules work / Handled static and dynamic arrays in type checking and code generation.\n");

//    printf("\n########################### ~ ** ~ ###########################\n\n");

    printf("\n\t######################## STAGE 1 INFO ########################\n");
    printf("\t FIRST and FOLLOW set automated. \n");
    printf("\t Both lexical and syntax analysis modules implemented. \n");
    printf("\t Modules work with all stage 1 testcases. \n");
    printf("\t Source code is parsed successfully. \n");
    printf("\t Parse tree is printed on the console (option 2). \n");
    printf("\t Error detection and recovery done.");
    printf("\n\t########################### ~ ** ~ ###########################\n\n");

    mt = createHashTable(SYMBOL_HT_SIZE); // 131 is the nearest prime > 114 (# of symbols (NT + T))
    fillHashTable(inverseMappingTable,mt);

    // Create and populate hash table for keywords
    keyword_ht = createHashTable(KEYWORD_HT_SIZE);

    char* keywords[] = {
        #define K(a,b,c) c,
        #include "keywords.txt"
        #undef K
        "#"
    };

    fillHashTable(keywords,keyword_ht);

    populateGrammarStruct("grammar.txt");


    populateFirstSet();
    populateFollowSet();
    populateParseTable();
    modifyParseTable_Err_Recovery();


    char userInput;

    while(1) {
        printf("\n\t Press 0 to exit."
               "\n\t Press 1 to print the token list generated by the lexer."
               "\n\t Press 2 to parse the input source code and to print parse tree."
               "\n\t Press 3 to build and print the Abstract Syntax Tree."
               "\n\t Press 4 to print the memory usage by Parse Tree and AST and to find compression percentage."
               "\n\t Press 5 to build and print Symbol Table."
               "\n\t Press 6 to print Activation Record sizes of every function."
               "\n\t Press 7 to print the type expressions and width of array variables ."
               "\n\t Press 8 to report errors and display total compiling time."
               "\n\t Press 9 to generate assembly code.\n");
            /* "\n\t Press # to build and printSymTable1 (archived)."
               "\n\t Press $ to build and printSymTable2 (archived)."
               "\n\t Press q to remove comments (archived)." */

        printf("------------------------------------------------------------------------------------------------"
               "--------------------------------------------------------------------\n");

        scanf(" %c", &userInput);

        switch(userInput) {

            case '0':
                return 0;
                break;

            case '1':
            {
                FILE *fp_arg = fopen(argv[1], "r");
                tokenInfo *tk;

                printf("\n################ TOKEN LIST ################\n");
                printf("\n%-12s %-20s %-20s\n", "LINE NO.", "LEXEME", "TOKEN NAME");

                // Initialise lexer every time.
                fp = 0; bp = 0; line_number = 1; status = 1; count = 0;
                while((tk = getNextToken(fp_arg)) != NULL) {
                    printf("%-12d %-20s %-20s\n", tk->lno, tk->lexeme, inverseMappingTable[tk->type]);
                    free(tk);
                }
                printf("\n################## ~ ** ~ ##################\n\n");
                fcloseSafe(fp_arg);
            }
            break;

            case '2':
            {
                // Initialise lexer every time.
                fp = 0; bp = 0; line_number = 1; status = 1; count = 0;

                treeNode *root = parseInputSourceCode(argv[1]); //this also frees the error stack
                printf("\n####################################################################### PARSE TREE "
                        "#######################################################################\n");
                printf("\nThe following parse tree has been printed in inorder traversal order:"
                       "\nLeftmost child ‐‐> parent node‐‐> remaining siblings (excluding the leftmost child)\n\n");
                printTree(root, NULL);   //printTree also frees the tree after printing it.
                printf("\n########################################################################## ~ ** ~ "
                       "##########################################################################\n\n");

                // passing NULL prints on stdout instead of in a file. Pass a file name to print in that file
                destroyTree(root);
            }
            break;

            case '3':
            {
                // Initialise lexer every time.
                fp = 0; bp = 0; line_number = 1; status = 1; count = 0;

                treeNode *root = parseInputSourceCode(argv[1]); //this also frees the error stack
                if(!stage1ErrorFree) {
                    printf("\nThe given source code has syntactic errors. Cannot print Abstract Syntax Tree.\n");
                    break;
                }
                if(!root){
                    break;
                }
                ASTNode *ASTroot = buildASTTree(root);
                if(!ASTroot) {
                    printf("\nTrivial source code. AST is empty. Hence, cannot print AST.\n");
                    break;
                }
                printf("\n################################################################### ABSTRACT SYNTAX TREE "
                       "###################################################################\n");
                printf("\nThe following AST has been printed in inorder traversal order:"
                       "\nLeftmost child ‐‐> parent node‐‐> remaining siblings (excluding the leftmost child)\n\n");
                print_Inorder_ASTTree(ASTroot, NULL); // NULL prints to console, pass a file name to print in the file
                printf("\n########################################################################## ~ ** ~ "
                       "##########################################################################\n\n");

                // The following will print the AST in Level-Depth traversal order
//                print_ASTTree(ASTroot, NULL); // NULL prints to console, pass a file name to print in the file

                destroyAST(ASTroot);
                destroyTree(root);
            }
            break;

            case '4':
            {
                // Initialise lexer every time.
                fp = 0; bp = 0; line_number = 1; status = 1; count = 0;

                treeNode *root = parseInputSourceCode(argv[1]); //this also frees the error stack
                int numParseNodes = count_nodes_parseTree(root);
                unsigned long memParseTree = numParseNodes * (sizeof(treeNode) + sizeof(tokenInfo));
                printf("\n%-27s %-1s %-10d %-20s %-10lu\n", "Parse tree Number of nodes ","=",numParseNodes, "Allocated Memory =",memParseTree);
                if(!stage1ErrorFree) {
                    printf("\nThe given source code has syntactic errors. Cannot compute memory usage by Abstract Syntax Tree.\n");
                    break;
                }
                if(!root){
                    break;
                }
                ASTNode *ASTroot = buildASTTree(root);
                if(!ASTroot) {
                    printf("\nTrivial source code. AST is empty. Hence, cannot compute its memory usage.\n");
                    break;
                }
                int numASTNodes = count_nodes_ASTTree(ASTroot);
                unsigned long memAST = numASTNodes * (sizeof(ASTNode) + sizeof(tokenInfo));
                printf("%-27s %-1s %-10d %-20s %-10lu\n", "AST Number of nodes ","=",numASTNodes, "Allocated Memory =",memAST);
                double comprPct = ((double )(memParseTree - memAST)/(double)memParseTree)*100.0;
                printf("Compression percentage = %lf\n", comprPct);

                destroyTree(root);
                destroyAST(ASTroot);
            }
            break;

            case '5':
            {
                // Initialise lexer every time.
                fp = 0; bp = 0; line_number = 1; status = 1; count = 0;

                treeNode *root = parseInputSourceCode(argv[1]); //this also frees the error stack
                if(!stage1ErrorFree) {
                    printf("\nThe given source code has syntactic errors. Cannot print Symbol Table.\n");
                    break;
                }
                if(!root){
                    break;
                }
                ASTNode *ASTroot = buildASTTree(root);

                if(!ASTroot) {
                    printf("\nTrivial source code. AST is empty. Hence, Symbol Table is also empty.\n");
                    break;
                }

                buildSymbolTable(ASTroot);
                printSymbolTable(&funcTable,false,NULL);// NULL prints to console, pass a file name to print in the file

                destroySymbolTable(&funcTable,true);
                destroyAST(ASTroot);
                destroyTree(root);
            }
            break;

            case '6':
            {
                fp = 0; bp = 0; line_number = 1; status = 1; count = 0;
                treeNode *root = parseInputSourceCode(argv[1]); //this also frees the error stack
                if(!stage1ErrorFree)
                    printf("\nThe given source code has syntactic errors. Cannot print Activation Record Sizes.\n");
                if(!root){
                    break;
                }
                ASTNode *ASTroot = buildASTTree(root);

                if(!ASTroot) {
                    printf("\nTrivial source code. AST is empty. Hence, AR sizes are undefined.\n");
                    break;
                }

                buildSymbolTable(ASTroot);
                printARSizes(&funcTable,NULL); // NULL prints to console, pass file name to print in the file

                destroySymbolTable(&funcTable,true);
                destroyAST(ASTroot);
                destroyTree(root);
            }
            break;

            case '7':
            {
                // Initialise lexer every time.
                fp = 0; bp = 0; line_number = 1; status = 1; count = 0;

                treeNode *root = parseInputSourceCode(argv[1]); //this also frees the error stack
                if(!stage1ErrorFree) {
                    printf("\nThe given source code has syntactic errors. Cannot print array information.\n");
                    break;
                }
                if(!root){
                    break;
                }
                ASTNode *ASTroot = buildASTTree(root);


                if(!ASTroot) {
                    printf("\nTrivial source code. AST is empty. Hence, there is no array information to be printed.\n");
                    break;
                }

                buildSymbolTable(ASTroot);
                printSymbolTable(&funcTable, true, NULL);// NULL prints to console, pass a file name to print in the file

                destroySymbolTable(&funcTable,true);
                destroyAST(ASTroot);
                destroyTree(root);
            }
            break;

            case '8':
            {
                clock_t start_time, end_time;
                double total_CPU_time, total_CPU_time_in_seconds;

                start_time = clock();

                //__________________________________________________
                // Initialise lexer every time.
                fp = 0; bp = 0; line_number = 1; status = 1; count = 0;

                treeNode *root = parseInputSourceCode(argv[1]); //this also frees the error stack

                if(!stage1ErrorFree) {
                    printf("\nThe given source code has syntactic errors. Cannot proceed to built AST.\n");
                    break;
                }

                if(!root){
                    break;
                }
                ASTNode *ASTroot = buildASTTree(root);
                if(!ASTroot) {
                    printf("\nTrivial source code. AST is empty. Hence, cannot proceed for building symbol table.\n");
                    break;
                }

                buildSymbolTable(ASTroot);

                //____________________________________________________
                end_time = clock();

                total_CPU_time  =  (double) (end_time - start_time);
                total_CPU_time_in_seconds =   total_CPU_time / CLOCKS_PER_SEC;

                printf("\nTotal CPU time = %lf \nTotal CPU time in secs = %lf \n", total_CPU_time, total_CPU_time_in_seconds);
                destroySymbolTable(&funcTable,true);
                destroyAST(ASTroot);
                destroyTree(root);
            }
            break;

            case '9':
            {
                // Initialise lexer every time.
                fp = 0; bp = 0; line_number = 1; status = 1; count = 0;

                treeNode *root = parseInputSourceCode(argv[1]); //this also frees the error stack

                if(!stage1ErrorFree) {
                    printf("\nThe given source code has syntactic errors. Cannot proceed to build AST.\n");
                    break;
                }

                if(!root){
                    break;
                }
                ASTNode *ASTroot = buildASTTree(root);


                if(!ASTroot) {
                    printf("\nTrivial source code. AST is empty. Hence, assembly code cannot be generated.\n");
                    break;
                }

                buildSymbolTable(ASTroot);
                if(!haveSemanticErrors){
                    FILE *fpout = fopen(argv[2],"w");
                    generateCode(ASTroot, &funcTable, fpout);
                    printf("Code compiles successfully..........\n");
                    fcloseSafe(fpout);
                }
                else{
                    printf("\nThe given source code has semantic errors. Cannot proceed to generate assembly code.\n");
                }
                destroySymbolTable(&funcTable,true);
                destroyAST(ASTroot);
                destroyTree(root);

            }
            break;

            // --------------------------------- ARCHIVED CASES FOR TESTING --------------------------------

            /*
            case 'q':
                printf("\n############################## SOURCE CODE W/O COMMENTS ##############################\n");
                removeComments(argv[1], NULL);
                printf("\n####################################### ~ ** ~ #######################################\n\n");
                break;

            case '#':
            {
                // Initialise lexer every time.
                fp = 0; bp = 0; line_number = 1; status = 1; count = 0;

                treeNode *root = parseInputSourceCode(argv[1]); //this also frees the error stack
                if(!stage1ErrorFree) {
                    printf("\nThe given source code has syntactic errors. Cannot print Symbol Table.\n");
                    break;
                }
                if(!root){
                    break;
                }
                ASTNode *ASTroot = buildASTTree(root);

                if(!ASTroot) {
                    printf("\nTrivial source code. AST is empty. Hence, Symbol Table is also empty.\n");
                    break;
                }

                buildSymbolTable(ASTroot);
                if(!haveSemanticErrors){
                    printf("Input source code is semantically correct and type checked..........\n");
                }
                printSymbolTable1(&funcTable,NULL);// NULL prints to console, pass a file name to print in the file

                destroySymbolTable(&funcTable,true);
                destroyAST(ASTroot);
                destroyTree(root);
            }
            break;

            case '$':
            {
                // Initialise lexer every time.
                fp = 0; bp = 0; line_number = 1; status = 1; count = 0;

                treeNode *root = parseInputSourceCode(argv[1]); //this also frees the error stack
                if(!stage1ErrorFree) {
                    printf("\nThe given source code has syntactic errors. Cannot print Symbol Table.\n");
                    break;
                }
                if(!root){
                    break;
                }
                ASTNode *ASTroot = buildASTTree(root);

                if(!ASTroot) {
                    printf("\nTrivial source code. AST is empty. Hence, Symbol Table is also empty.\n");
                    break;
                }

                buildSymbolTable(ASTroot);
                if(!haveSemanticErrors){
                    printf("Input source code is semantically correct and type checked..........\n");
                }
                printSymbolTable2(&funcTable,NULL);// NULL prints to console, pass a file name to print in the file

                destroySymbolTable(&funcTable,true);
                destroyAST(ASTroot);
                destroyTree(root);
            }
            break;
            */

            default:
                printf("Invalid Choice. Please try again! \n");
        }
        printf("------------------------------------------------------------------------------------------------"
               "--------------------------------------------------------------------\n");

    }

    return 0;
}

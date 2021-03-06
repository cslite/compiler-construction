// Group Number: 31
// MADHUR PANWAR   2016B4A70933P
// TUSSANK GUPTA   2016B3A70528P
// SALMAAN SHAHID  2016B4A70580P
// APURV BAJAJ     2016B3A70549P
// HASAN NAQVI     2016B5A70452P

#include <stdbool.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"
#include "util.h"
#include "error.h"
#include "lexerDef.h"
#include "config.h"
#include "errorPtr_stack.h"

extern errorPtr_stack *errorStack;

typedef unsigned int uint;

hashTable *keyword_ht;


uint TWIN_BUFFER_SIZE = 2 * BUFFER_SIZE;
uint line_number = 1;
uint bp = 0; // begin ptr
uint fp = 0; // forward ptr
char buffer_for_tokenization[2 * BUFFER_SIZE]; // a global buffer of size 2 * BUFFER_SIZE

// Used only in function getStream().
int status = 1;
int count = 0;


void getStream(FILE *file_ptr) {

    count++;

    // Selecting a half from the global buffer, alternatively.
    status ^= 1;

    // Invalid call, discard all changes
    if((fp == 0 && status != 0) || (fp == BUFFER_SIZE && status != 1)) {
        status ^= 1;
        count--;
        return;
    }

    // Fill one half of the global buffer with zeros.
    uint i;
    for(i=0; i<BUFFER_SIZE; i++)
        buffer_for_tokenization[(status * BUFFER_SIZE) + i] = 0;

    if(feof(file_ptr))
        return;

    fread(buffer_for_tokenization + (status * BUFFER_SIZE), BUFFER_SIZE, sizeof(char), file_ptr);

    // if(feof(file_ptr))
    //     fprintf(stderr, "EOF REACHED : %d buffer reads\n", count);

    return;
}

// only for removeComments, don't need twin buffers there
char* getBlock(FILE *fp) {
    if(feof(fp))
        return NULL;

    char *buffer_from_file = (char*) calloc(BUFFER_SIZE, sizeof(char));
    fread(buffer_from_file, BUFFER_SIZE, sizeof(char), fp);
    return buffer_from_file;
}

void removeComments(char *testcaseFile, char *cleanFile) {
    FILE *fp_testcaseFile = fopen(testcaseFile, "r");

    if(fp_testcaseFile == NULL) {
        fprintf(stderr,"ERROR: Failed to open %s", testcaseFile);
        return;
    }

    FILE *fp_cleanFile;

    // Defaults to printing to stdout if no output file specified.
    if(cleanFile == NULL)
        fp_cleanFile = stdout;
    else
        fp_cleanFile = fopen(cleanFile, "w");

    if(fp_cleanFile == NULL) {
        fprintf(stderr,"ERROR: Failed to open %s", cleanFile);
        return;
    }

    char *buffer_to_read = getBlock(fp_testcaseFile);
    char *buffer_to_write = (char*) malloc(BUFFER_SIZE * sizeof(char));

    int isComment = 0, wasAsterisk = 0;

    while(buffer_to_read) {
        uint i = 0, j = 0;

        if(wasAsterisk && buffer_to_read[0] == '*'){
            isComment ^= 1;
            i++;
        }

        for(; i < BUFFER_SIZE && buffer_to_read[i] != '\0'; i++){

            if(buffer_to_read[i] == '*' && (i < BUFFER_SIZE - 1 && buffer_to_read[i+1] == '*')){
                isComment ^= 1;
                i += 2;
            }

            if(!isComment || buffer_to_read[i] == '\n')
                buffer_to_write[j++] = buffer_to_read[i];
        }

        fwrite(buffer_to_write, sizeof(char), j, fp_cleanFile);

        if(buffer_to_read[BUFFER_SIZE - 1] == '*')
            wasAsterisk = 1;
        else
            wasAsterisk = 0;

        free(buffer_to_read);
        buffer_to_read = getBlock(fp_testcaseFile);
    }

    free(buffer_to_write);


    fcloseSafe(fp_cleanFile);

    fcloseSafe(fp_testcaseFile);
}

// Prints the string in global buffer from start to end, not including end.
// [start, end)
void print_lexical_error(uint start, uint end) {
//    fprintf(stderr, "LEXICAL ERROR: invalid token on line number %u.\n \t", line_number);
    start = start % TWIN_BUFFER_SIZE;
    end = end % TWIN_BUFFER_SIZE;

    char *errTk = allocString((end+TWIN_BUFFER_SIZE-start)%TWIN_BUFFER_SIZE);

    uint i = start;
    do{
        errTk[(i+TWIN_BUFFER_SIZE-start)%TWIN_BUFFER_SIZE] = buffer_for_tokenization[i];
        i = (i + 1) % TWIN_BUFFER_SIZE;
    }while(i != end);

    errTk[(end+TWIN_BUFFER_SIZE-start)%TWIN_BUFFER_SIZE] = '\0';

    error e = {LEXICAL,line_number};
    e.edata.le.errTk = errTk;
    foundNewError(e);

}

tokenInfo* getNextToken(FILE *file_ptr) {

    int state = 0;

    if(fp == 0 || fp == BUFFER_SIZE) {
        getStream(file_ptr);
    }

    tokenInfo* tkin = (tokenInfo*) malloc(sizeof(tokenInfo));
    char lookahead;

    while(1) {
        lookahead = buffer_for_tokenization[fp];
        fp = (fp + 1) % TWIN_BUFFER_SIZE;
        
        if(fp == 0 || fp == BUFFER_SIZE)
            getStream(file_ptr);

        switch(state) {

            case 0:
            {
                switch (lookahead)
                {
                    case '+':
                    {
                        state = 17;
                        tkin->type = PLUS;
                        tkin->lno = line_number;
                        tkin->lexeme[0] = '+';
                        tkin->lexeme[1] = '\0';
                        bp = fp;
                        return tkin;
                    }
                    break;

                    case '-':
                    {
                        state = 18;
                        tkin->type = MINUS;
                        tkin->lno = line_number;
                        tkin->lexeme[0] = '-';
                        tkin->lexeme[1] = '\0';
                        bp = fp;
                        return tkin;
                    }
                    break;

                    case '/':
                    {
                        state = 19;
                        tkin->type = DIV;
                        tkin->lno = line_number;
                        tkin->lexeme[0] = '/';
                        tkin->lexeme[1] = '\0';
                        bp = fp;
                        return tkin;
                    }
                    break;

                    case '[':
                    {
                        state = 28;
                        tkin->type = SQBO;
                        tkin->lno = line_number;
                        tkin->lexeme[0] = '[';
                        tkin->lexeme[1] = '\0';
                        bp = fp;
                        return tkin;
                    }
                    break;

                    case ']':
                    {
                        state = 29;
                        tkin->type = SQBC;
                        tkin->lno = line_number;
                        tkin->lexeme[0] = ']';
                        tkin->lexeme[1] = '\0';
                        bp = fp;
                        return tkin;
                    }
                    break;

                    case '(':
                    {
                        state = 30;
                        tkin->type = BO;
                        tkin->lno = line_number;
                        tkin->lexeme[0] = '(';
                        tkin->lexeme[1] = '\0';
                        bp = fp;
                        return tkin;
                    }
                    break;

                    case ')':
                    {
                        state = 31;
                        tkin->type = BC;
                        tkin->lno = line_number;
                        tkin->lexeme[0] = ')';
                        tkin->lexeme[1] = '\0';
                        bp = fp;
                        return tkin;
                    }
                    break;

                    case ',':
                    {
                        state = 32;
                        tkin->type = COMMA;
                        tkin->lno = line_number;
                        tkin->lexeme[0] = ',';
                        tkin->lexeme[1] = '\0';
                        bp = fp;
                        return tkin;
                    }
                    break;

                    case ';':
                    {
                        state = 33;
                        tkin->type = SEMICOL;
                        tkin->lno = line_number;
                        tkin->lexeme[0] = ';';
                        tkin->lexeme[1] = '\0';
                        bp = fp;
                        return tkin;
                    }
                    break;

                    case '!':
                        state = 34;
                    break;

                    case '=':
                        state = 36;
                    break;

                    case ':':
                        state = 38;
                    break;

                    case '.':
                        state = 41;
                    break;

                    case '<':
                        state = 20;
                    break;

                    case '>':
                        state = 24;
                    break;

                    case '*':
                        state = 12;
                    break;

                    // Run these cases together. No change in state.
                    case '\n': line_number += 1;
                    case '\t':
                    case '\r': // Ignore \r when running on Windows.
                    case ' ':
                        bp = fp;
                    break;

                    // Check for NUM, RNUM, ID/Keywords and Invalid characters.
                    default: 
                    {
                        // Check for ID/Keywords.
                        if((lookahead >= 'a' && lookahead <= 'z') || (lookahead >= 'A' && lookahead <= 'Z'))
                            state = 10;

                        else if(lookahead >= '0' && lookahead <= '9') 
                            state = 1;

                        else if(lookahead == '\0'){
                            free(tkin);
                            return NULL;
                        }


                        else {   
                            state = 0;
                            print_lexical_error(bp, fp);
                            bp = fp;
                            continue;
                        }
                    }
                }
            }
            break;

            case 1:
            {
                if (lookahead >= '0' && lookahead <= '9'){
                    // Do nothing. State remains 1
                }
                
                else if(lookahead == '.')  {
                    state = 3;
                }

                else {
                    state = 2;
                    fp = (fp + TWIN_BUFFER_SIZE - 1) % TWIN_BUFFER_SIZE; // Retract 1

                    if((fp - bp + TWIN_BUFFER_SIZE) % TWIN_BUFFER_SIZE > 100) {
                        print_lexical_error(bp, fp);
                        bp = fp;
                        state = 0;
                        continue;
                    }

                    if(fp > bp)
                        strncpy(tkin->lexeme, buffer_for_tokenization + bp, fp - bp);
                    else {
                        strncpy(tkin->lexeme, buffer_for_tokenization + bp, TWIN_BUFFER_SIZE - bp);
                        strncpy(tkin->lexeme + TWIN_BUFFER_SIZE - bp, buffer_for_tokenization, fp);
                    }

                    tkin->lexeme[(fp - bp + TWIN_BUFFER_SIZE) % TWIN_BUFFER_SIZE] = '\0';

                    tkin->type = NUM;
                    tkin->lno = line_number;
                    tkin->value.num = atoi(tkin->lexeme);
                    bp = fp;
                    return tkin;
                }
            }
            break;

            case 3:
            {
                if(lookahead == '.') {
                    state = 4;
                    fp = (fp + TWIN_BUFFER_SIZE - 2) % TWIN_BUFFER_SIZE; // Retract 2

                    if((fp - bp + TWIN_BUFFER_SIZE) % TWIN_BUFFER_SIZE > 100) {
                        print_lexical_error(bp, fp);
                        bp = fp;
                        state = 0;
                        continue;
                    }

                    if(fp > bp)
                        strncpy(tkin->lexeme, buffer_for_tokenization + bp, fp - bp);
                    else {
                        strncpy(tkin->lexeme, buffer_for_tokenization + bp, TWIN_BUFFER_SIZE - bp);
                        strncpy(tkin->lexeme + TWIN_BUFFER_SIZE - bp, buffer_for_tokenization, fp);
                    }

                    tkin->lexeme[(fp - bp + TWIN_BUFFER_SIZE) % TWIN_BUFFER_SIZE] = '\0';

                    tkin->type = NUM;
                    tkin->lno = line_number;
                    tkin->value.num = atoi(tkin->lexeme);
                    bp = fp;
                    return tkin;
                }
                else if(lookahead >= '0' && lookahead <= '9') {
                    state = 5;
                }
                else {   
                    state = 0;
                    fp = (fp + TWIN_BUFFER_SIZE - 1) % TWIN_BUFFER_SIZE; // Retract 1
                    print_lexical_error(bp, fp);
                    bp = fp;
                    continue;
                }
            }
            break;

            case 5:
            {
                if(lookahead == 'e' || lookahead == 'E')
                    state = 6;
                
                else if(lookahead >= '0' && lookahead <= '9') {
                    // Do nothing. State remains 5.
                }

                else{
                    state = 9;
                    fp = (fp + TWIN_BUFFER_SIZE - 1) % TWIN_BUFFER_SIZE; //Retract 1

                    if((fp - bp + TWIN_BUFFER_SIZE) % TWIN_BUFFER_SIZE > 100) {
                        print_lexical_error(bp, fp);
                        bp = fp;
                        state = 0;
                        continue;
                    }

                    if(fp > bp)
                        strncpy(tkin->lexeme, buffer_for_tokenization + bp, fp - bp);
                    else {
                        strncpy(tkin->lexeme, buffer_for_tokenization + bp, TWIN_BUFFER_SIZE - bp);
                        strncpy(tkin->lexeme + TWIN_BUFFER_SIZE - bp, buffer_for_tokenization, fp);
                    }

                    tkin->lexeme[(fp - bp + TWIN_BUFFER_SIZE) % TWIN_BUFFER_SIZE] = '\0';

                    tkin->type = RNUM;
                    tkin->lno = line_number;
                    tkin->value.rnum = atof(tkin->lexeme);
                    bp = fp;
                    return tkin;
                }
            }
            break;

            case 6:
            {
                if(lookahead == '+' || lookahead == '-') {
                    state = 7;
                }

                else if(lookahead >= '0' && lookahead <= '9') {
                    state = 8;
                }

                else {   
                    state = 0;
                    fp = (fp + TWIN_BUFFER_SIZE - 1) % TWIN_BUFFER_SIZE; // Retract 1
                    print_lexical_error(bp, fp);
                    bp = fp;
                    continue;
                }
            }
            break;

            case 7:
            {
                if(lookahead >= '0' && lookahead <= '9') {
                    state = 8;
                }

                else {   
                    state = 0;
                    fp = (fp + TWIN_BUFFER_SIZE - 1) % TWIN_BUFFER_SIZE; // Retract 1
                    print_lexical_error(bp, fp);
                    bp = fp;
                    continue;
                }
            }
            break;

            case 8:
            {
                if(lookahead >= '0' && lookahead <= '9') {
                    // Do nothing. State remains 8.
                }

                else {
                    state = 9;
                    fp = (fp + TWIN_BUFFER_SIZE - 1) % TWIN_BUFFER_SIZE; // Retract 1

                    if((fp - bp + TWIN_BUFFER_SIZE) % TWIN_BUFFER_SIZE > 100) {
                        print_lexical_error(bp, fp);
                        bp = fp;
                        state = 0;
                        continue;
                    }

                    if(fp > bp)
                        strncpy(tkin->lexeme, buffer_for_tokenization + bp, fp - bp);
                    else {
                        strncpy(tkin->lexeme, buffer_for_tokenization + bp, TWIN_BUFFER_SIZE - bp);
                        strncpy(tkin->lexeme + TWIN_BUFFER_SIZE - bp, buffer_for_tokenization, fp);
                    }

                    tkin->lexeme[(fp - bp + TWIN_BUFFER_SIZE) % TWIN_BUFFER_SIZE] = '\0';

                    tkin->type = RNUM;
                    tkin->lno = line_number;
                    tkin->value.rnum = atof(tkin->lexeme);
                    bp = fp;

                    return tkin;
                }
            }
            break;

            case 10:
            {
                if(isalnum(lookahead) || lookahead == '_') {
                    // Do nothing. Stay at state 10;
                }

                else {
                    state = 11;
                    fp = (fp + TWIN_BUFFER_SIZE - 1) % TWIN_BUFFER_SIZE; //Retract 1

                    if((fp - bp + TWIN_BUFFER_SIZE) % TWIN_BUFFER_SIZE > 20) {
                        print_lexical_error(bp, fp);
                        bp = fp;
                        state = 0;
                        continue;
                    }

                    if(fp > bp)
                        strncpy(tkin->lexeme, buffer_for_tokenization + bp, fp - bp);
                    else {
                        strncpy(tkin->lexeme, buffer_for_tokenization + bp, TWIN_BUFFER_SIZE - bp);
                        strncpy(tkin->lexeme + TWIN_BUFFER_SIZE - bp, buffer_for_tokenization, fp);
                    }

                    tkin->lexeme[(fp - bp + TWIN_BUFFER_SIZE) % TWIN_BUFFER_SIZE] = '\0';

                    if(searchSymbol(tkin->lexeme, keyword_ht))
                        tkin->type = getEnumValue(tkin->lexeme,keyword_ht);
                    else
                        tkin->type = ID;

                    tkin->lno = line_number;
                    bp = fp;
                    return tkin;
                }
            }
            break;

            case 12:
            {
                if(lookahead == '*')
                    state = 14;
                
                else {
                    state = 13;
                    fp = (fp + TWIN_BUFFER_SIZE - 1) % TWIN_BUFFER_SIZE; // Retract 1
                    bp = fp;

                    tkin->type = MUL;
                    tkin->lno = line_number;
                    tkin->lexeme[0] = '*';
                    tkin->lexeme[1] = '\0';
                    return tkin;
                }
            }
            break;

            case 14:
            {
                if(lookahead == '*')
                    state = 15;
                
                else if(lookahead == '\n')
                    line_number++;

                else if(lookahead == '\0'){
                    free(tkin);
                    return NULL;
                }

                else {
                    // Do nothing
                }
            }
            break;

            case 15:
            {
                // Commentmark is no longer lexeme, we need to restart now, instead of returning.
                if(lookahead == '*') {
                    state = 0;
                    bp = fp;
                    continue;
                }  
                
                else if(lookahead == '\n') {
                    line_number++;
                    state = 14;
                }

                else {
                    state = 14;
                }
            }
            break;
            
            case 20:
            {
                if(lookahead == '=') {
                    state = 21;

                    tkin->type = LE;
                    tkin->lno = line_number;
                    tkin->lexeme[0] = '<';
                    tkin->lexeme[1] = '=';
                    tkin->lexeme[2] = '\0';
                    bp = fp;
                    return tkin;
                }

                else if(lookahead == '<') {
                    state = 22; // No longer final; 
                }

                else {
                    state = 23;
                    fp = (fp + TWIN_BUFFER_SIZE - 1) % TWIN_BUFFER_SIZE; // Retract 1
                    bp = fp;

                    tkin->type = LT;
                    tkin->lno = line_number;
                    tkin->lexeme[0] = '<';
                    tkin->lexeme[1] = '\0';
                    return tkin;
                }
            }
            break;

            case 24:
            {
                if(lookahead == '=') {
                    state = 25;

                    tkin->type = GE;
                    tkin->lno = line_number;
                    tkin->lexeme[0] = '>';
                    tkin->lexeme[1] = '=';
                    tkin->lexeme[2] = '\0';
                    bp = fp;
                    return tkin;
                }

                else if(lookahead == '>') {
                    state = 26; // No longer final; 
                }

                else {
                    state = 27;
                    fp = (fp + TWIN_BUFFER_SIZE - 1) % TWIN_BUFFER_SIZE; // Retract 1
                    bp = fp;

                    tkin->type = GT;
                    tkin->lno = line_number;
                    tkin->lexeme[0] = '>';
                    tkin->lexeme[1] = '\0';
                    return tkin;
                }
            }
            break;

            case 34:
            {
                if(lookahead == '=') {
                    state = 35;

                    tkin->type = NE;
                    tkin->lno = line_number;
                    tkin->lexeme[0] = '!';
                    tkin->lexeme[1] = '=';
                    tkin->lexeme[2] = '\0';
                    bp = fp;
                    return tkin;
                }
                
                else {   
                    state = 0;
                    fp = (fp + TWIN_BUFFER_SIZE - 1) % TWIN_BUFFER_SIZE; // Retract 1
                    print_lexical_error(bp, fp);
                    bp = fp;
                    continue;
                }
            }
            break;

            case 36:
            {
                if(lookahead == '=') {
                    state = 37;

                    tkin->type = EQ;
                    tkin->lno = line_number;
                    tkin->lexeme[0] = '=';
                    tkin->lexeme[1] = '=';
                    tkin->lexeme[2] = '\0';
                    bp = fp;
                    return tkin;
                }
                else {   
                    state = 0;
                    fp = (fp + TWIN_BUFFER_SIZE - 1) % TWIN_BUFFER_SIZE; // Retract 1
                    print_lexical_error(bp, fp);
                    bp = fp;
                }
            }
            break;

            case 38:
            {
                if(lookahead == '=') {
                    state = 39;

                    tkin->type = ASSIGNOP;
                    tkin->lno = line_number;
                    tkin->lexeme[0] = ':';
                    tkin->lexeme[1] = '=';
                    tkin->lexeme[2] = '\0';
                    bp = fp;
                    return tkin;
                }

                else {
                    state = 40;
                    fp = (fp + TWIN_BUFFER_SIZE - 1) % TWIN_BUFFER_SIZE; // Retract 1
                    
                    tkin->type = COLON;
                    tkin->lno = line_number;
                    tkin->lexeme[0] = ':';
                    tkin->lexeme[1] = '\0';
                    bp = fp;
                    return tkin;
                }
            }
            break;

            case 41:
            {
                if(lookahead == '.') {
                    state = 42;

                    tkin->type = RANGEOP;
                    tkin->lno = line_number;
                    tkin->lexeme[0] = '.';
                    tkin->lexeme[1] = '.';
                    tkin->lexeme[2] = '\0';
                    bp = fp;
                    return tkin;
                }
                else {   
                    state = 0;
                    fp = (fp + TWIN_BUFFER_SIZE - 1) % TWIN_BUFFER_SIZE; // Retract 1
                    print_lexical_error(bp, fp);
                    bp = fp;
                    continue;
                }    
            }
            break;

            case 22:
            {
                if(lookahead == '<') {
                    state = 43;

                    tkin->type = DRIVERDEF;
                    tkin->lno = line_number;
                    tkin->lexeme[0] = '<';
                    tkin->lexeme[1] = '<';
                    tkin->lexeme[2] = '<';
                    tkin->lexeme[3] = '\0';
                    bp = fp;
                    return tkin;
                }

                else {
                    state = 44;
                    fp = (fp + TWIN_BUFFER_SIZE - 1) % TWIN_BUFFER_SIZE; // Retract 1
                    bp = fp;
                    
                    tkin->type = DEF;
                    tkin->lno = line_number;
                    tkin->lexeme[0] = '<';
                    tkin->lexeme[1] = '<';
                    tkin->lexeme[2] = '\0';
                    return tkin;
                }
            }
            break;

            case 26:
            {
                if(lookahead == '>') {
                    state = 45;

                    tkin->type = DRIVERENDDEF;
                    tkin->lno = line_number;
                    tkin->lexeme[0] = '>';
                    tkin->lexeme[1] = '>';
                    tkin->lexeme[2] = '>';
                    tkin->lexeme[3] = '\0';
                    bp = fp;
                    return tkin;
                }

                else {
                    state = 46;
                    fp = (fp + TWIN_BUFFER_SIZE - 1) % TWIN_BUFFER_SIZE; // Retract 1
                    bp = fp;
                    
                    tkin->type = ENDDEF;
                    tkin->lno = line_number;
                    tkin->lexeme[0] = '>';
                    tkin->lexeme[1] = '>';
                    tkin->lexeme[2] = '\0';
                    return tkin;
                }
            }
            break;
        }
    }
}

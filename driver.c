#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "hash.h"
#include "lexer/lexer.h"

struct hashTable *ht;

int main(int argc, char *argv[]) {

    // Create and poplate hash table for keywords
    ht = createHashTable(31);

    FILE *fp = fopen("keywords.txt", "r");

    if(fp == NULL) {
        printf("Can't open file");
        exit(1);
    }

    char keyword[21];
    int i = 0;
    while(!feof(fp)) {
        fscanf(fp, " %s", keyword);
        addKeyword(keyword, i++);
    }
    fclose(fp);

    // Populate token name
    char tokenName[57][21]; // Sync with terminals.txt

    fp = fopen("terminals.txt", "r");

    if(fp == NULL) {
        printf("Can't open file");
        exit(1);
    }

    i = 0;
    while(!feof(fp)) {
        fscanf(fp, " %s", tokenName[i++]);
    }
    fclose(fp);

    //printHashTable();

    //removeComments("abc.txt", "abc1.txt");

    if (argc == 2)
        fp = fopen(argv[1], "r");
    else
        fp = fopen("test_cases_1000.txt", "r");
    //fp = fopen("abc2.txt", "r");
    
    // Print all tokens.
    tokenInfo *tk = getNextToken(fp);
    while(tk!=NULL) {

        printf("LNo %6d ", tk->lno);
        printf("%20s ", tokenName[tk->type]);
        printf("%20s ", tk->lexeme);

        if(tk->type==NUM)
            printf("%20d\n", tk->value.num);

        else if(tk->type ==RNUM)
            printf("%20f\n", tk->value.rnum);

        else
            printf("%20s\n", "");

        free(tk);
        tk = getNextToken(fp);
        i++;

        if(i == 125000){
            printf("Interrupted");
            break;
        }
    }

}

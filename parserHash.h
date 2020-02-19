// since the structure of the nodes in the hash table here and the one used in lexer is different,
// I created this another set of hash definitions
// May think of abstracting out the minimum functionality of just the hash part so that redefinition isn't required.
// For now, this should do.
#ifndef PARSERHASH_H
#define PARSERHASH_H
#include <stdbool.h>
#include "parserDef.h"

// struct definitions
// Mapping Table (Symbol -> Enum)
struct mappingTableNode{
    gSymbol symEnum; // to store the Enum index value
    char *symbol; // stores the symbol string
    struct mappingTableNode *next; // points to next node in the linked list (Separate chaining is used for handling collisions)
}; //114 symbols (57 each of terminals and non terminals)
typedef struct mappingTableNode mappingTableNode;

struct mappingTableEntry {
    int size; // size of the linked list pointed by 'first'
    struct mappingTableNode *first; // points to the first node in the linked list
};

struct mappingTable {
    int size; // no. of slots in the mapping table
    int hashed; // no. of symbols hashed into the mapping table (this is incremented when we add a symbol to the mapping table)
    struct mappingTableEntry *entries; // points to the array of elements of type mappingTableEntry
};

void addSymbol(char *symbol, int index); // add 'symbol' to mapping table with index as 'index'
bool searchSymbol(char *symbol); // check if 'symbol' is present in mapping table or not
gSymbol getEnumValue(char *symbol); // get the Enum value (viz. stored index) of the 'symbol'
int mappingHashFunction(char *symbol); // returns the slot value in the mapping table where 'symbol' is to be hashed
struct mappingTable* createMappingTable(int size); // creates an empty mapping table and returns a pointer to the structure
void printMappingTable(); // prints the mapping table
void fillMappingTable(char *tokenPath, char *nonTerminalsPath); // fills the mapping table with the symbols read from tokenPath and nonTerminalsPath
char *ripOffX(char *symbol); // takes string "X(...)", returns a string "..."


// Inverse Mapping Table (Enum -> Symbol)
// Inverse Mapping Table is an array of pointers to linked lists of mappingTableNode(s)
// We only need to change the functions since now hash is on Enum value rather than string
// All the structs can be reused
void addEnum(gSymbol enum_val, char *symbol); // adds the key value pair ('enum_val', 'symbol') to the inverse mapping table 'imt'
bool searchEnum(gSymbol enum_val); // check if 'enum_val' is present in the inverse mapping table or not
char* getSymbol(gSymbol enum_val); // get the 'symbol' corresponding to Enum value 'enum_val' [NOTE: the returned string is malloced in the function so it can be used without harming the string already in the linked list node]
int enumHashFunction(gSymbol enum_val); // returns the slot value in the inverse mapping table where 'enum_val' is to be hashed [NOTE: identity function is used as hash, collision free guaranteed]
struct mappingTable* createInverseMappingTable(int size); // creates an empty inverse mapping table and returns a pointer to the structure
void printInverseMappingTable(); // prints the inverse mapping table
void fillInverseMappingTable(char *tokenPath, char *nonTerminalsPath); // fills the inverse mapping table with the (enum value, symbol) pairs read from tokenPath and nonTerminalsPath

#endif
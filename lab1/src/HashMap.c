#include "HashMap.h"
#include <string.h>
char *reserved_words_list[37] = {
    "auto",      "break",    "case",     "char",   "const",   "continue",
    "default",   "do",       "double",   "else",   "enum",    "extern",
    "float",     "for",      "goto",     "if",     "inline",  "int",
    "long",      "register", "restrict", "return", "short",   "signed",
    "sizeof",    "static",   "struct",   "switch", "typedef", "union",
    "unsigned",  "void",     "volatile", "while",  "_Bool",   "_Complex",
    "_Imaginary"};

const int M = 467;
const int B = 227;

unsigned int getHashValue(const char *word) {
    size_t len = strlen(word);
    unsigned int value = 0;
    for (size_t i = 0; i < len; i++)
    {
        value = (value * B + word[i]) % M;
    }
    return value;
}
void reserveMapInit(char *hash_map[]) {
    for (int i = 0; i < 37; i++)
    {
        hash_map[getHashValue(reserved_words_list[i])] = reserved_words_list[i];
    }
}

int isReservedWord(const char *word, char **hash_map) {
    unsigned int hash_value = getHashValue(word);
    if (hash_map[hash_value] == NULL)
    {
        return 0;
    }
    else
    { return (int)(strcmp(word, hash_map[hash_value]) == 0); }
}
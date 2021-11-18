void reserveMapInit(char *hash_map[]);

unsigned int getHashValue(const char *word);

int isReservedWord(const char *word, char **hash_map);
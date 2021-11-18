#include <stdio.h>
char *yytext;
int yyleng;
long unsigned int line_num = 1;
long unsigned int char_num = 0;
long unsigned int num_before_current_line = 0;
long unsigned int identifier_num = 0;
long unsigned int reserved_words_num = 0;
long unsigned int numeric_constants_num = 0;
long unsigned int char_constants_num = 0;
long unsigned int string_literals_num = 0;
long unsigned int punctuator_num = 0;
enum _token_type {
    IDENTIFIER,
    INTEGER,
    FLOAT,
    CHAR_CONST,
    STRING,
    RESERVED,
    LINE_COMMENT,
    BLOCK_COMMENT,
    PREPROCESS,
    PUNCTUATOR,
    NEW_LINE
};
void print(int token_type) {
    switch (token_type)
    {
    case IDENTIFIER:
        printf("[Identifier, Value: %s]\n", yytext);
        identifier_num++;
        break;
    case INTEGER:
        printf("[Integer, Value: %s]\n", yytext);
        numeric_constants_num++;
        break;
    case FLOAT:
        printf("[Float, Value: %s]\n", yytext);
        numeric_constants_num++;
        break;
    case CHAR_CONST:
        printf("[Character constant, Value: %s]\n", yytext);
        char_constants_num++;
        break;
    case STRING:
        printf("[String, Value: %s]\n", yytext);
        string_literals_num++;
        break;
    case RESERVED:
        printf("[Reserved word, Value: %s]\n", yytext);
        reserved_words_num++;
        break;
    case LINE_COMMENT:
        break;
    case BLOCK_COMMENT: {
        for (int i = 0; i < yyleng; ++i)
        {
            if (yytext[i] == '\n')
            {
                line_num++;
                printf("Line %ld:\n", line_num);
            }
        }
    }
    break;
    case PREPROCESS:
        break;
    case PUNCTUATOR:
        printf("[Punctuation, Value: %s]\n", yytext);
        punctuator_num++;
        break;
    case NEW_LINE:
        line_num++;
        printf("Line %ld:\n", line_num);
        break;
    default:
        break;
    }
    char_num += yyleng;
}
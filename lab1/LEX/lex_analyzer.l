%{
#include <stdio.h>
long unsigned int line_num = 1;
long unsigned int char_num = 0;
long unsigned int num_before_current_line = 0;
long unsigned int identifier_num = 0;
long unsigned int reserved_words_num = 0;
long unsigned int numeric_constants_num = 0;
long unsigned int char_constants_num = 0;
long unsigned int string_literals_num = 0;
long unsigned int punctuator_num = 0;
enum _token_type{
IDENTIFIER=1,
INTEGER,
FLOAT,
CHAR_CONST,
STRING,
LINE_COMMENT,
BLOCK_COMMENT,
PREPROCESS,
RESERVED,
PUNCTUATOR,
NEW_LINE,
SPACE
};
%}
IDENTIFIER ([_a-zA-Z][_a-zA-Z0-9]*)
INTEGER (0x[0-9a-fA-F]+|[0-9]+)([lL]|[lL]{2}|[uU]|[lL]{1,2}[uU]|[uU][lL]{1,2})?
FLOAT (([0-9]+\.([0-9]+)?|\.[0-9]+)([eE]([+-])?[0-9]+)?|[0-9]+[eE]([+-])?[0-9]+|(0x[0-9a-fA-F]+\.([0-9a-fA-F]+)?|0x\.[0-9a-fA-F]+|[0-9a-fA-F]+)[pP]([+-])?[0-9]+)([fF]|[lL])?
CHAR_CONST (L)?\'(\\.|[^\\\'])\'
STRING (L)?\"(\\.|[^\\\"])+\"
LINE_COMMENT ("//"[^\n]*)
BLOCK_COMMENT ("/*"(\n|.)*"*/")
PREPROCESS ("#"[^\n]*)
RESERVED auto|break|case|char|const|continue|default|do|double|else|enum|extern|float|for|goto|if|inline|int|long|register|restrict|return|short|signed|sizeof|static|struct|switch|typedef|union|unsigned|void|volatile|while|_Bool|_Complex|_Imaginary
PUNCTUATOR \.([\.]{2})?|\-([\>\=\-])?|([\+\&\|]){2}|[\+\&\|](\=)?|[\=\!\^\*](\=)?|\/(\=)?|\>\>\=|>([\>\=])?|\<\<\=|\<([\<\=\:\%])?|\:(\>)?|\#{1,2}|\%\:\%\:|\%([\:\=\>])|[\+\&\|]|[\[\]\(\)\{\}\~\?\;\,]
%%
[\n]    {return NEW_LINE;}
[ \t\r\a]+  {return SPACE;}
{RESERVED} {return RESERVED;}
{IDENTIFIER}    {return IDENTIFIER;}
{INTEGER}   {return INTEGER;}
{FLOAT} {return FLOAT;}
{CHAR_CONST}    {return CHAR_CONST;}
{STRING}    {return STRING;}
{LINE_COMMENT}    {return LINE_COMMENT;}
{BLOCK_COMMENT}    {return BLOCK_COMMENT;}
{PREPROCESS}    {return PREPROCESS;}
{PUNCTUATOR}    {return PUNCTUATOR;}
<<EOF>> { return 0; }
%%
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
    case SPACE:
        break;
    default:
        break;
    }
    char_num += yyleng;
}

int main() {
	int token_type;
	while (token_type = yylex()) {
		print(token_type);
	}
	    printf("\n\nTotal:\nLines: %lu\nCharators: %lu\nIdentifiers: %lu\nReserved "
           "Words: %lu\nNumeric Constants: %lu\nCharactor Constants: "
           "%lu\nString Literals: %lu\nPunctuators: %lu\n",
           line_num, char_num, identifier_num, reserved_words_num,
           numeric_constants_num, char_constants_num, string_literals_num,
           punctuator_num);
	return 0;
}

int yywrap() {
	return 1;
}
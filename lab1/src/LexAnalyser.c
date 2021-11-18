#include "HashMap.h"
#include "List.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int BUFFER_SIZE = 1 << 10;

long unsigned int line_num = 1;
long unsigned int char_num = 0;
long unsigned int num_before_current_line = 0;
long unsigned int identifier_num = 0;
long unsigned int reserved_words_num = 0;
long unsigned int numeric_constants_num = 0;
long unsigned int char_constants_num = 0;
long unsigned int string_literals_num = 0;
long unsigned int punctuator_num = 0;

struct List *identifier_list;
char buffer[2][(1 << 10) + 1];

enum statement {
    //初始状态
    ORIGIN,

    //标识符
    IDENTIFIER,

    //数字常量
    CONST_INT_DEC,
    CONST_INT_ZERO,
    CONST_INT_HEX,
    CONST_INT_OCT,
    CONST_INT_POSTFIX,
    CONST_FLOAT_2,
    CONST_FLOAT_3,
    CONST_FLOAT_4,
    CONST_FLOAT_5,
    CONST_FLOAT_POSTFIX,

    //字符常量
    CONST_CHAR,

    //字符串
    STRING,

    //空白符
    SPACE,

    //符号
    PUNCTUATOR,
    POINT_1,
    POINT_2,
    POINT_3,
    MINUS,
    PLUS_AND_OR,
    NOT_XOR_EQ_MULT,
    DIVIDE,
    GE_1,
    GE_2,
    LE_1,
    LE_2,
    COLON,
    NUMBER_SIGH,
    MOD_1,
    MOD_2,
    MOD_3,

    //注释
    COMMENT_LINE,
    COMMENT_BLOCK,

    //预处理
    PREPROCESS,

    //错误处理
    ERROR_NUM_POSTFIX,
    ERROR_INT,
    ERROR_FLOAT,
    ERROR_PUNCTUATOR,
};

_Bool isIdentifier(const char *forward) {
    return (isalpha(*forward) != 0 || *forward == '_' ||
            isdigit(*forward) != 0);
}

_Bool isOctal(const char *forward) {
    if (isdigit(*forward) == 0 || *forward == '8' || *forward == '9')
    {
        return 0;
    }
    return 1;
}

_Bool isHexdecimal(const char *forward) {
    if (isdigit(*forward) == 0 &&
        (*forward < 'a' && *forward > 'F' || *forward > 'f' || *forward < 'A'))
    {
        return 0;
    }
    return 1;
}

char *copyStr(const char *lex_begin, const char *forward) {
    char *copy;
    int which = 0;
    if (forward - lex_begin < 0 || forward - lex_begin + 2 > (BUFFER_SIZE))
    {
        int len = 0;
        if (lex_begin - &buffer[0][0] <= BUFFER_SIZE)
        {
            which = 0;
        }
        else if (lex_begin - &buffer[1][0] <= BUFFER_SIZE)
        { which = 1; }
        else
        {
            printf("Error occurred at copyStr!\n");
            exit(0);
        }
        len = BUFFER_SIZE - (lex_begin - &buffer[which][0]);
        len += forward - &buffer[1 - which][0] + 2;
        copy = (char *)malloc(sizeof(char) * len);
        int i = 0;
        while (i < BUFFER_SIZE - (lex_begin - &buffer[which][0]))
        {
            copy[i] = lex_begin[i];
            i++;
        }
        for (int j = 0; j < forward - &buffer[1 - which][0] + 1; ++j)
        {
            copy[i++] = buffer[1 - which][j];
        }
        copy[i] = '\0';
    }
    else
    {
        copy = (char *)malloc(sizeof(char) * (forward - lex_begin + 2));
        int i = 0;
        while (i <= (int)(forward - lex_begin))
        {
            copy[i] = lex_begin[i];
            i++;
        }
        copy[i] = '\0';
    }
    return copy;
}

void LexAnalyser(FILE *source) {
    printf("Line %lu:\n", line_num);

    char **reserved_words = malloc(sizeof(char *) * 467);
    reserveMapInit(reserved_words);

    struct List *identifier_list;
    identifier_list = ListInit();

    enum statement stmt = ORIGIN;
    int which = 0;
    size_t len = 0;
    _Bool reached_end = 0;
    len = fread(buffer[which], sizeof(char), BUFFER_SIZE, source);
    buffer[which][len] = -1;
    if (len != BUFFER_SIZE)
    {
        reached_end = 1;
    }
    char *lex_begin = &buffer[which][0];
    char *forward = lex_begin;

    while (1)
    {
        _Bool exit_flag = 0;
        enum statement number_stmt;
        if (*forward == -1)
        {
            if (!reached_end)
            {
                which = 1 - which;
                len = fread(buffer[which], sizeof(char), BUFFER_SIZE, source);
                if (len != BUFFER_SIZE)
                {
                    reached_end = 1;
                }
                buffer[which][len] = -1;
                forward = &buffer[which][0];
            }
            else
            { exit_flag = 1; }
        }

        switch (stmt)
        {
        case ORIGIN:
            if (isalpha(*forward) != 0 || *forward == '_')
            {
                stmt = IDENTIFIER;
                forward++;
                char_num++;
            }
            else if (isdigit(*forward) != 0 && *forward != '0')
            {
                stmt = CONST_INT_DEC;
                forward++;
                char_num++;
            }
            else if (*forward == '0')
            {
                stmt = CONST_INT_ZERO;
                forward++;
                char_num++;
            }
            else if (isspace(*forward) != 0)
            {
                if (*forward == '\n')
                {
                    line_num++;
                    num_before_current_line = char_num;
                    printf("Line %lu:\n", line_num);
                }
                stmt = SPACE;
                forward++;
                char_num++;
            }
            else if (*forward == '[' || *forward == ']' || *forward == '(' ||
                     *forward == ')' || *forward == '{' || *forward == '}' ||
                     *forward == '~' || *forward == '?' || *forward == ';' ||
                     *forward == ',')
            {
                printf("[Punctuator: %c]\n", *(forward++));
                punctuator_num++;
                char_num++;
                lex_begin = forward;
            }
            else if (*forward == '.')
            {
                stmt = POINT_1;
                forward++;
                char_num++;
            }
            else if (*forward == '-')
            {
                stmt = MINUS;
                forward++;
                char_num++;
            }
            else if (*(forward) == '+' || *(forward) == '&' ||
                     *(forward) == '|')
            {
                stmt = PLUS_AND_OR;
                forward++;
                char_num++;
            }
            else if (*forward == '=' || *forward == '!' || *forward == '^' ||
                     *forward == '*')
            {
                stmt = NOT_XOR_EQ_MULT;
                forward++;
                char_num++;
            }
            else if (*forward == '/')
            {
                stmt = DIVIDE;
                forward++;
                char_num++;
            }
            else if (*forward == '>')
            {
                stmt = GE_1;
                forward++;
                char_num++;
            }
            else if (*forward == '<')
            {
                stmt = LE_1;
                forward++;
                char_num++;
            }
            else if (*forward == ':')
            {
                stmt = COLON;
                forward++;
                char_num++;
            }
            else if (*forward == '#')
            {
                stmt = NUMBER_SIGH;
                forward++;
                char_num++;
            }
            else if (*forward == '%')
            {
                stmt = MOD_1;
                forward++;
                char_num++;
            }
            else if (*forward == '"')
            {
                stmt = STRING;
                forward++;
                char_num++;
            }
            else if (*forward == '\'')
            {
                stmt = CONST_CHAR;
                forward++;
                char_num++;
            }
            break;
        case PUNCTUATOR: {
            char *punctuator = copyStr(lex_begin, forward);
            printf("[Punctuator: %s]\n", punctuator);
            free(punctuator);
            stmt = ORIGIN;
            forward++;
            char_num++;
            punctuator_num++;
            lex_begin = forward;
            break;
        }
        case IDENTIFIER:
            if (isIdentifier(forward))
            {
                stmt = IDENTIFIER;
                forward++;
                char_num++;
            }
            else
            {
                if (*forward == '"' || *forward == '\'')
                {
                    if (forward == &buffer[which][0] &&
                        buffer[1 - which][BUFFER_SIZE - 1] == 'L')
                    {
                        if (*forward == '"')
                        {
                            stmt = STRING;
                        }
                        else
                        { stmt = CONST_CHAR; }
                        break;
                    }
                    else if (*(forward - 1) == 'L')
                    {
                        if (*forward == '"')
                        {
                            stmt = STRING;
                        }
                        else
                        { stmt = CONST_CHAR; }
                        break;
                    }
                }
                stmt = ORIGIN;
                char *idf = copyStr(lex_begin, forward);
                if (forward != &buffer[which][0])
                {
                    idf = copyStr(lex_begin, forward - 1);
                }
                else
                {
                    idf =
                        copyStr(lex_begin, &buffer[1 - which][BUFFER_SIZE - 1]);
                }
                if (isReservedWord(idf, reserved_words))
                {
                    printf("[Reserved Word: %s]\n", idf);
                    free(idf);
                    reserved_words_num++;
                }
                else
                {
                    int sequence = ListPush(identifier_list, idf);
                    printf("[Identifier, Value: %s, Sequence: %d]\n", idf,
                           sequence);
                    identifier_num++;
                }
                lex_begin = forward;
            }
            break;
        case CONST_INT_DEC:
            if (isdigit(*forward) != 0)
            {
                stmt = CONST_INT_DEC;
                forward++;
                char_num++;
            }
            else if (isdigit(*forward) == 0 && isHexdecimal(forward) != 0)
            { stmt = ERROR_INT; }
            else if (*forward == '.')
            {
                number_stmt = stmt;
                stmt = CONST_FLOAT_2;
                forward++;
                char_num++;
            }
            else if (*forward == 'E' || *forward == 'e')
            {
                number_stmt = stmt;
                stmt = CONST_FLOAT_3;
                forward++;
                char_num++;
            }
            else if (*forward == 'l' || *forward == 'L' || *forward == 'u' ||
                     *forward == 'U')
            {
                number_stmt = stmt;
                stmt = CONST_INT_POSTFIX;
            }
            else
            {
                stmt = ORIGIN;
                char *integer_constant_dec;
                if (forward != &buffer[which][0])
                {
                    integer_constant_dec = copyStr(lex_begin, forward - 1);
                }
                else
                {
                    integer_constant_dec =
                        copyStr(lex_begin, &buffer[1 - which][BUFFER_SIZE - 1]);
                }
                printf("[Integer Constant (Decimal), value: %s]\n",
                       integer_constant_dec);
                free(integer_constant_dec);
                numeric_constants_num++;
                lex_begin = forward;
            }
            break;
        case CONST_INT_ZERO:
            if (*forward == 'x' || *forward == 'X')
            {
                stmt = CONST_INT_HEX;
                forward++;
                char_num++;
            }
            else if (isOctal(forward))
            {
                stmt = CONST_INT_OCT;
                forward++;
                char_num++;
            }
            else if (isOctal(forward) == 0 && isdigit(*forward) != 0)
            { stmt = ERROR_INT; }
            else if (*forward == 'l' || *forward == 'L' || *forward == 'u' ||
                     *forward == 'U')
            {
                number_stmt = CONST_INT_DEC;
                stmt = CONST_INT_POSTFIX;
            }
            else if (*forward == '0')
            {
                forward++;
                char_num++;
            }
            else
            {
                stmt = ORIGIN;
                char *integer_constant_dec;
                if (forward != &buffer[which][0])
                {
                    integer_constant_dec = copyStr(lex_begin, forward - 1);
                }
                else
                {
                    integer_constant_dec =
                        copyStr(lex_begin, &buffer[1 - which][BUFFER_SIZE - 1]);
                }
                printf("[Integer Constant (Decimal), value: %s]\n",
                       integer_constant_dec);
                free(integer_constant_dec);
                numeric_constants_num++;
                lex_begin = forward;
            }
            break;
        case CONST_INT_OCT:
            if (isOctal(forward) != 0)
            {
                stmt = CONST_INT_OCT;
                forward++;
                char_num++;
            }
            else if (*forward == '.')
            {
                number_stmt = stmt;
                stmt = CONST_FLOAT_2;
                forward++;
                char_num++;
            }
            else if (*forward == 'E' || *forward == 'e')
            {
                number_stmt = stmt;
                stmt = CONST_FLOAT_3;
                forward++;
                char_num++;
            }
            else if (*forward == 'l' || *forward == 'L' || *forward == 'u' ||
                     *forward == 'U')
            {
                number_stmt = stmt;
                stmt = CONST_INT_POSTFIX;
            }
            else if (isOctal(forward) == 0 && isdigit(*forward) != 0)
            { stmt = ERROR_INT; }
            else
            {
                stmt = ORIGIN;
                char *integer_constant_oct;
                if (forward != &buffer[which][0])
                {
                    integer_constant_oct = copyStr(lex_begin, forward - 1);
                }
                else
                {
                    integer_constant_oct =
                        copyStr(lex_begin, &buffer[1 - which][BUFFER_SIZE - 1]);
                }
                printf("[Integer Constant (Octal), value: %s]\n",
                       integer_constant_oct);
                free(integer_constant_oct);
                numeric_constants_num++;
                lex_begin = forward;
            }
            break;
        case CONST_INT_HEX:
            if (isHexdecimal(forward) != 0)
            {
                stmt = CONST_INT_HEX;
                forward++;
                char_num++;
            }
            else if (*forward == '.')
            {
                number_stmt = stmt;
                stmt = CONST_FLOAT_2;
                forward++;
                char_num++;
            }
            else if (*forward == 'P' || *forward == 'p')
            {
                number_stmt = stmt;
                stmt = CONST_FLOAT_3;
                forward++;
                char_num++;
            }
            else if (*forward == 'l' || *forward == 'L' || *forward == 'u' ||
                     *forward == 'U')
            {
                number_stmt = stmt;
                stmt = CONST_INT_POSTFIX;
            }
            else
            {
                stmt = ORIGIN;
                char *integer_constant_hex;
                if (forward != &buffer[which][0])
                {
                    integer_constant_hex = copyStr(lex_begin, forward - 1);
                }
                else
                {
                    integer_constant_hex =
                        copyStr(lex_begin, &buffer[1 - which][BUFFER_SIZE - 1]);
                }
                printf("[Integer Constant (Hexdecimal), value: %s]\n",
                       integer_constant_hex);
                free(integer_constant_hex);
                numeric_constants_num++;
                lex_begin = forward;
            }
            break;
        case CONST_INT_POSTFIX: {
            char *ci_postfix[7] = {"l", "ll", "u", "ul", "lu", "ull", "llu"};
            char *tmp_f = forward;
            while (*forward == 'u' || *forward == 'l' || *forward == 'U' ||
                   *forward == 'L')
            {
                forward++;
                char_num++;
            }
            char *integer_constant_postfix;
            if (forward != &buffer[which][0])
            {
                integer_constant_postfix = copyStr(tmp_f, forward - 1);
            }
            else
            {
                integer_constant_postfix =
                    copyStr(tmp_f, &buffer[1 - which][BUFFER_SIZE - 1]);
            }
            size_t len = strlen(integer_constant_postfix);
            for (int i = 0; i < len; i++)
            {
                *(integer_constant_postfix + i) =
                    tolower(*(integer_constant_postfix + i));
            }
            for (int i = 0; i < 7; i++)
            {
                if (strcmp(ci_postfix[i], integer_constant_postfix) == 0)
                {
                    stmt = ORIGIN;
                    char *integer_constant;
                    if (forward != &buffer[which][0])
                    {
                        integer_constant = copyStr(lex_begin, forward - 1);
                    }
                    else
                    {
                        integer_constant = copyStr(
                            lex_begin, &buffer[1 - which][BUFFER_SIZE - 1]);
                    }
                    switch (number_stmt)
                    {
                    case CONST_INT_DEC:
                        printf("[Integer Constant (Decimal), value: %s]\n",
                               integer_constant);
                        break;
                    case CONST_INT_OCT:
                        printf("[Integer Constant (Octal), value: %s]\n",
                               integer_constant);
                        break;
                    case CONST_INT_HEX:
                        printf("[Integer Constant (Hexdecimal), value: %s]\n",
                               integer_constant);
                        break;
                    default:
                        break;
                    }
                    free(integer_constant);
                    numeric_constants_num++;
                    break;
                }
                stmt = ERROR_NUM_POSTFIX;
            }
            free(integer_constant_postfix);
            break;
        }
        case CONST_FLOAT_2:
            if (isdigit(*forward) != 0 ||
                isHexdecimal(forward) != 0 && number_stmt == CONST_INT_HEX)
            {
                stmt = CONST_FLOAT_2;
                forward++;
                char_num++;
            }
            else if ((*forward == 'e' || *forward == 'E') &&
                     (number_stmt == CONST_INT_DEC ||
                      number_stmt == CONST_INT_OCT))
            {
                stmt = CONST_FLOAT_3;
                forward++;
                char_num++;
            }
            else if ((*forward == 'p' || *forward == 'P') &&
                     number_stmt == CONST_INT_HEX)
            {
                stmt = CONST_FLOAT_3;
                forward++;
                char_num++;
            }
            else if (*forward == 'f' || *forward == 'F' || *forward == 'l' ||
                     *forward == 'L')
            { stmt = CONST_FLOAT_POSTFIX; }
            else
            { stmt = ERROR_FLOAT; }
            break;
        case CONST_FLOAT_3:
            if (*forward == '+' || *forward == '-')
            {
                stmt = CONST_FLOAT_4;
                forward++;
                char_num++;
            }
            else if (isdigit(*forward) ||
                     isHexdecimal(forward) != 0 && number_stmt == CONST_INT_HEX)
            {
                stmt = CONST_FLOAT_5;
                forward++;
                char_num++;
            }
            else
            { stmt = ERROR_FLOAT; }
            break;
        case CONST_FLOAT_4:
            if (isdigit(*forward) ||
                isHexdecimal(forward) != 0 && number_stmt == CONST_INT_HEX)
            {
                stmt = CONST_FLOAT_5;
                forward++;
                char_num++;
            }
            else
            { stmt = ERROR_FLOAT; }
            break;
        case CONST_FLOAT_5:
            if (isdigit(*forward) ||
                isHexdecimal(forward) != 0 && number_stmt == CONST_INT_HEX)
            {
                stmt = CONST_FLOAT_5;
                forward++;
                char_num++;
            }
            else if (*forward == 'f' || *forward == 'F' || *forward == 'l' ||
                     *forward == 'L')
            { stmt = CONST_FLOAT_POSTFIX; }
            else if (number_stmt == CONST_INT_DEC ||
                     number_stmt == CONST_INT_OCT)
            {
                stmt = ORIGIN;
                char *float_constant_dec;
                if (forward != &buffer[which][0])
                {
                    float_constant_dec = copyStr(lex_begin, forward - 1);
                }
                else
                {
                    float_constant_dec =
                        copyStr(lex_begin, &buffer[1 - which][BUFFER_SIZE - 1]);
                }
                printf("[Float Constant (Decimal), value: %s]\n",
                       float_constant_dec);
                free(float_constant_dec);
                numeric_constants_num++;
                lex_begin = forward;
            }
            else if (number_stmt == CONST_INT_HEX)
            {
                stmt = ORIGIN;
                char *float_constant_hex;
                if (forward != &buffer[which][0])
                {
                    float_constant_hex = copyStr(lex_begin, forward - 1);
                }
                else
                {
                    float_constant_hex =
                        copyStr(lex_begin, &buffer[1 - which][BUFFER_SIZE - 1]);
                }
                printf("[Float Constant (Hexdecimal), value: %s]\n",
                       float_constant_hex);
                free(float_constant_hex);
                numeric_constants_num++;
                lex_begin = forward;
            }
            break;
        case CONST_FLOAT_POSTFIX:
            if (number_stmt == CONST_INT_DEC || number_stmt == CONST_INT_OCT)
            {
                stmt = ORIGIN;
                char *float_constant_dec = copyStr(lex_begin, forward);
                printf("[Float Constant (Decimal), value: %s]\n",
                       float_constant_dec);
                free(float_constant_dec);
                numeric_constants_num++;
                forward++;
                char_num++;
                lex_begin = forward;
            }
            else if (number_stmt == CONST_INT_HEX)
            {
                stmt = ORIGIN;
                char *float_constant_hex = copyStr(lex_begin, forward);
                printf("[Float Constant (Hexdecimal), value: %s]\n",
                       float_constant_hex);
                free(float_constant_hex);
                numeric_constants_num++;
                forward++;
                char_num++;
                lex_begin = forward;
            }
            break;
        case STRING:
            if (*forward == '"' && *(forward - 1) != '\\' ||
                *forward == '"' && *(forward - 1) == '\\' &&
                    *(forward - 2) == '\\')
            {
                stmt = ORIGIN;
                char *string_literals = copyStr(lex_begin, forward);
                printf("[String Literals: %s]\n", string_literals);
                free(string_literals);
                string_literals_num++;
                forward++;
                char_num++;
                lex_begin = forward;
            }
            else
            {
                forward++;
                char_num++;
            }
            break;
        case CONST_CHAR:
            if (*forward == '\'' && *(forward - 1) != '\\' ||
                *forward == '\'' && *(forward - 1) == '\\' &&
                    *(forward - 2) == '\\')
            {
                stmt = ORIGIN;
                char *const_char = copyStr(lex_begin, forward);
                printf("[Charactor Constant: %s]\n", const_char);
                free(const_char);
                char_constants_num++;
                forward++;
                char_num++;
                lex_begin = forward;
            }
            else
            {
                forward++;
                char_num++;
            }
            break;
        case COMMENT_LINE: {
            while (*forward != '\n')
            {
                forward++;
                char_num++;
            }
            stmt = ORIGIN;
            // char *comment;
            // if (forward != &buffer[which][0])
            // {
            //     comment = copyStr(lex_begin, forward - 1);
            // }
            // else
            // {
            //     comment =
            //         copyStr(lex_begin, &buffer[1 - which][BUFFER_SIZE - 1]);
            // }
            // printf("[Comment: %s]\n", comment);
            // free(comment);
            lex_begin = forward;
            break;
        }
        case COMMENT_BLOCK: {
            if (!(*forward == '/' && *(forward - 1) == '*'))
            {
                forward++;
                char_num++;
                if (*forward == '\n')
                {
                    line_num++;
                    num_before_current_line = char_num;
                    printf("Line %lu:\n", line_num);
                }
                break;
            }
            stmt = ORIGIN;
            // char *comment;
            // if (forward != &buffer[which][0])
            // {
            //     comment = copyStr(lex_begin, forward - 1);
            // }
            // else
            // {
            //     comment =
            //         copyStr(lex_begin, &buffer[1 - which][BUFFER_SIZE - 1]);
            // }
            // printf("[Comment: %s]\n", comment);
            // free(comment);
            forward++;
            lex_begin = forward;
            break;
        }
        case PREPROCESS: {
            while (*forward != '\n')
            {
                forward++;
                char_num++;
            }
            stmt = ORIGIN;
            char *preprocess;
            if (forward != &buffer[which][0])
            {
                preprocess = copyStr(lex_begin, forward - 1);
            }
            else
            {
                preprocess =
                    copyStr(lex_begin, &buffer[1 - which][BUFFER_SIZE - 1]);
            }
            printf("[Preprocess Line: %s]\n", preprocess);
            free(preprocess);
            lex_begin = forward;
            break;
        }
        case SPACE:
            if (isspace(*forward) != 0)
            {
                if (*forward == '\n')
                {
                    line_num++;
                    num_before_current_line = char_num;
                    printf("Line %lu:\n", line_num);
                }
                forward++;
                char_num++;
            }
            else
            {
                stmt = ORIGIN;
                lex_begin = forward;
            }
            break;
        case MINUS:
            if (*forward == '=' || *forward == '>')
            {
                stmt = PUNCTUATOR;
            }
            else
            {
                printf("[Punctuator: -]\n");
                punctuator_num++;
                stmt = ORIGIN;
                lex_begin = forward;
            }
            break;
        case PLUS_AND_OR:
            if (*forward == '=')
            {
                stmt = PUNCTUATOR;
            }
            else if (forward == &buffer[which][0])
            {
                if (*forward == buffer[1 - which][BUFFER_SIZE - 1])
                {
                    stmt = PUNCTUATOR;
                }
                else
                {
                    printf("[Punctuator: %c]\n",
                           buffer[1 - which][BUFFER_SIZE - 1]);
                    punctuator_num++;
                    stmt = ORIGIN;
                    lex_begin = forward;
                }
            }
            else
            {
                if (*forward == *(forward - 1))
                {
                    stmt = PUNCTUATOR;
                }
                else
                {
                    printf("[Punctuator: %c]\n", *(forward - 1));
                    punctuator_num++;
                    stmt = ORIGIN;
                    lex_begin = forward;
                }
            }
            break;
        case NOT_XOR_EQ_MULT:
            if (*forward == '=')
            {
                stmt = PUNCTUATOR;
            }
            else
            {
                if (forward == &buffer[which][0])
                {
                    printf("[Punctuator: %c]\n",
                           buffer[1 - which][BUFFER_SIZE - 1]);
                }
                else
                { printf("[Punctuator: %c]\n", *(forward - 1)); }
                punctuator_num++;
                stmt = ORIGIN;
                lex_begin = forward;
            }
            break;
        case DIVIDE:
            if (*forward == '=')
            {
                stmt = PUNCTUATOR;
            }
            else if (*forward == '/')
            { stmt = COMMENT_LINE; }
            else if (*forward == '*')
            { stmt = COMMENT_BLOCK; }
            else
            {
                printf("[Punctuator: /]\n");
                punctuator_num++;
                stmt = ORIGIN;
                lex_begin = forward;
            }
            break;
        case GE_1:
            if (*forward == '>')
            {
                stmt = GE_2;
                forward++;
                char_num++;
            }
            else
            {
                if (forward == &buffer[which][0])
                {
                    printf("[Punctuator: %c]\n",
                           buffer[1 - which][BUFFER_SIZE - 1]);
                }
                else
                { printf("[Punctuator: %c]\n", *(forward - 1)); }
                punctuator_num++;
                stmt = ORIGIN;
                lex_begin = forward;
            }
            break;
        case GE_2:
            if (*forward == '=')
            {
                stmt = PUNCTUATOR;
            }
            else
            {
                if (forward == &buffer[which][0])
                {
                    forward = &buffer[1 - which][BUFFER_SIZE - 1];
                }
                else
                {
                    forward--;
                    char_num--;
                }
                stmt = PUNCTUATOR;
            }
        case LE_1:
            if (*forward == '=' || *forward == ':' || *forward == '%')
            {
                stmt = PUNCTUATOR;
            }
            else if (*forward == '<')
            {
                stmt = LE_2;
                forward++;
                char_num++;
            }
            else
            {
                if (forward == &buffer[which][0])
                {
                    printf("[Punctuator: %c]\n",
                           buffer[1 - which][BUFFER_SIZE - 1]);
                }
                else
                { printf("[Punctuator: %c]\n", *(forward - 1)); }
                punctuator_num++;
                stmt = ORIGIN;
                lex_begin = forward;
            }
            break;
        case LE_2:
            if (*forward == '=')
            {
                stmt = PUNCTUATOR;
            }
            else
            {
                if (forward == &buffer[which][0])
                {
                    forward = &buffer[1 - which][BUFFER_SIZE - 1];
                }
                else
                {
                    forward--;
                    char_num--;
                }
                stmt = PUNCTUATOR;
            }
            break;
        case COLON:
            if (*forward == '>')
            {
                stmt = PUNCTUATOR;
            }
            else
            {
                printf("[Punctuator: :]\n");
                punctuator_num++;
                stmt = ORIGIN;
                lex_begin = forward;
            }
            break;
        case NUMBER_SIGH:
            if (*forward == '#')
            {
                stmt = PUNCTUATOR;
            }
            else
            { stmt = PREPROCESS; }
            break;
        case MOD_1:
            if (*forward == ':')
            {
                stmt = MOD_2;
                forward++;
                char_num++;
            }
            else if (*forward == '>' || *forward == '=')
            { stmt = PUNCTUATOR; }
            else
            {
                printf("[Punctuator: %%]\n");
                punctuator_num++;
                stmt = ORIGIN;
                lex_begin = forward;
            }
            break;
        case MOD_2:
            if (*forward == '%')
            {
                stmt = MOD_3;
                forward++;
                char_num++;
            }
            else
            { stmt = ERROR_PUNCTUATOR; }
            break;
        case MOD_3:
            if (*forward == ':')
            {
                stmt = PUNCTUATOR;
            }
            else
            { stmt = ERROR_PUNCTUATOR; }
            break;
        case POINT_1:
            if (*forward == '.')
            {
                forward++;
                char_num++;
                stmt = POINT_2;
            }
            else
            {
                stmt = ORIGIN;
                printf("[Punctuator: %c]\n", *(forward++));
                punctuator_num++;
                char_num++;
                lex_begin = forward;
            }
            break;
        case POINT_2:
            if (*forward == '.')
            {
                stmt = PUNCTUATOR;
            }
            else
            { stmt = ERROR_PUNCTUATOR; }
            break;
        case ERROR_NUM_POSTFIX: {
            char *integer_constant;
            if (forward != &buffer[which][0])
            {
                integer_constant = copyStr(lex_begin, forward - 1);
            }
            else
            {
                integer_constant =
                    copyStr(lex_begin, &buffer[1 - which][BUFFER_SIZE - 1]);
            }
            printf(
                "***\nError:%ld:%ld: \nNumeric constant with invalid post fix: "
                "%s\n***\n",
                line_num, char_num - num_before_current_line, integer_constant);
            free(integer_constant);
            lex_begin = forward;
            stmt = ORIGIN;
        }
        break;
        case ERROR_INT: {
            if (isdigit(*forward) != 0 || *forward == 'u' || *forward == 'U' ||
                *forward == 'L' || *forward == 'l')
            {
                forward++;
                char_num++;
                break;
            }
            char *integer_constant;
            if (forward != &buffer[which][0])
            {
                integer_constant = copyStr(lex_begin, forward - 1);
            }
            else
            {
                integer_constant =
                    copyStr(lex_begin, &buffer[1 - which][BUFFER_SIZE - 1]);
            }
            printf("***\nError:%ld:%ld: \nInvalid integer constant: "
                   "%s\n***\n",
                   line_num, char_num - num_before_current_line,
                   integer_constant);
            free(integer_constant);
            lex_begin = forward;
            stmt = ORIGIN;
            break;
        }
        case ERROR_FLOAT: {
            if (isalpha(*forward) || isdigit(*forward) || *forward == '.' ||
                *forward == '+' || *forward == '-')
            {
                forward++;
                char_num++;
                break;
            }
            char *float_constant;
            if (forward != &buffer[which][0])
            {
                float_constant = copyStr(lex_begin, forward - 1);
            }
            else
            {
                float_constant =
                    copyStr(lex_begin, &buffer[1 - which][BUFFER_SIZE - 1]);
            }
            printf("***\nError:%ld:%ld: \nInvalid numeric constant: "
                   "%s\n***\n",
                   line_num, char_num - num_before_current_line,
                   float_constant);
            free(float_constant);
            lex_begin = forward;
            stmt = ORIGIN;
            break;
        }
        case ERROR_PUNCTUATOR: {
            char *punctuator = copyStr(lex_begin, forward);
            printf("***\nError:%ld:%ld: \nInvalid punctuator: "
                   "%s\n***\n",
                   line_num, char_num - num_before_current_line, punctuator);
            free(punctuator);
            forward++;
            lex_begin = forward;
            stmt = ORIGIN;
            break;
        }
        }
        if (exit_flag)
        {
            if (stmt == STRING)
            {
                printf(
                    "***\nERROR:\nString is unclosed. Missing \'\"\'.\n***\n");
            }
            if (stmt == COMMENT_BLOCK)
            {
                printf("***\nERROR:\nComment is unclosed. Missing */.\n***\n");
            }
            break;
        }
    }
}

int main(int argc, char *argv[]) {
    FILE *source;
    char *s;
    if (2 == argc)
    {

        s = argv[1];
        source = fopen(argv[1], "r");
    }
    else
    {
        printf("Usage: LexAnalyse [source_file.c]\n");
        return 0;
    }
    if (source == NULL)
    {
        printf("Error: Could not open source file.\n");
        return 0;
    }
    LexAnalyser(source);
    fclose(source);
    printf("\n\nTotal:\nLines: %lu\nCharators: %lu\nIdentifiers: %lu\nReserved "
           "Words: %lu\nNumeric Constants: %lu\nCharactor Constants: "
           "%lu\nString Literals: %lu\nPunctuators: %lu\n",
           line_num, char_num, identifier_num, reserved_words_num,
           numeric_constants_num, char_constants_num, string_literals_num,
           punctuator_num);
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "tokens.h"

char *copy_string(const char *src){
    char *copy = malloc(strlen(src)+1);
    if(copy==NULL){
        return NULL;
    }
    strcpy(copy,src);
    return copy;
}


TokenArray *lex(const char *src,FILE *out){
    TokenArray *result = malloc(sizeof(TokenArray));
    if(result==NULL){
        return NULL;
    }
    int capacity=100;
    result->tokens = malloc(capacity*sizeof(Token));
    if(result->tokens==NULL){
        free(result);
        return NULL;
    }
    result->count=0;
    
    int i=0;
    while(src[i]!='\0'){

        /* Skip whitespace */
        if (isspace((unsigned char)src[i]))
        {
            i++;
            continue;
        }

        /* Preporcessor directive */
        if(src[i]=='#'){
            char directive[256];
            int j=0;
            while(src[i]!='\n'&&src[i]!='\0'){
                directive[j++]=src[i++];
            }
            directive[j]='\0';
            result->tokens[result->count].type=TOKEN_PREPROCESSOR;
            result->tokens[result->count].value = copy_string(directive);
            result->count++;
            fprintf(out,"PREPROCESSOR(%s)\n",directive);
            continue;
        }

        /* Identifier or keyword */
        if (isalpha((unsigned char)src[i]) || src[i] == '_')
        {
            char word[100];
            int j = 0;

            while (isalnum((unsigned char)src[i]) || src[i] == '_')
            {
                word[j++] = src[i++];
            }
            word[j] = '\0';
            if (strcmp(word, "int") == 0){
                result->tokens[result->count].type=TOKEN_KEYWORD;
                result->tokens[result->count].value=copy_string(word);
                result->count++;
                fprintf(out, "KEYWORD(int)\n");
            }
            else if (strcmp(word, "void") == 0){
                result->tokens[result->count].type=TOKEN_KEYWORD;
                result->tokens[result->count].value=copy_string(word);
                result->count++;
                fprintf(out, "KEYWORD(void)\n");
            }
            else if (strcmp(word, "return") == 0){
                result->tokens[result->count].type=TOKEN_KEYWORD;
                result->tokens[result->count].value=copy_string(word);
                result->count++;
                fprintf(out, "KEYWORD(return)\n");
            }
            else{
                result->tokens[result->count].type=TOKEN_IDENTIFIER;
                result->tokens[result->count].value=copy_string(word);
                result->count++;
                fprintf(out, "IDENTIFIER(%s)\n", word);
            }

            continue;
        }

        /* Integer constant */
        if (isdigit((unsigned char)src[i]))
        {
            char number[100];
            int j = 0;

            while (isdigit((unsigned char)src[i]))
            {
                number[j++] = src[i++];
            }
            number[j] = '\0';

            result->tokens[result->count].type = TOKEN_CONSTANT;
            result->tokens[result->count].value = copy_string(number);
            result->count++;

            fprintf(out, "CONSTANT(%s)\n", number);
            continue;
        }

        /* Hyphen */
        if(src[i]=='-'){
            if(src[i+1]!='-'){
                result->tokens[result->count].type = TOKEN_HYPHEN;
                result->tokens[result->count].value = NULL;
                fprintf(out,"HYPHEN\n");
                result->count++;
                i++;
            }else{
                result->tokens[result->count].type= TOKEN_DECREMENT;
                result->tokens[result->count].value=NULL;
                result->count++;
                fprintf(out,"DECREMENT\n");
                i+=2;
            }
            continue;
        }

        /* Single-character tokens and operators */
        switch (src[i])
    {
        case '(':
            result->tokens[result->count].type = TOKEN_OPEN_PAREN;
            result->tokens[result->count].value = NULL;

            fprintf(out, "OPEN_PAREN\n");

            result->count++;
            break;

        case ')':
            result->tokens[result->count].type = TOKEN_CLOSE_PAREN;
            result->tokens[result->count].value = NULL;

            fprintf(out, "CLOSE_PAREN\n");

            result->count++;
            break;

        case '{':
            result->tokens[result->count].type = TOKEN_OPEN_BRACE;
            result->tokens[result->count].value = NULL;

            fprintf(out, "OPEN_BRACE\n");

            result->count++;
            break;

        case '}':
            result->tokens[result->count].type = TOKEN_CLOSE_BRACE;
            result->tokens[result->count].value = NULL;

            fprintf(out, "CLOSE_BRACE\n");

            result->count++;
            break;

        case ';':
            result->tokens[result->count].type = TOKEN_SEMICOLON;
            result->tokens[result->count].value = NULL;

            fprintf(out, "SEMICOLON\n");

            result->count++;
            break;

        case '~':
            result->tokens[result->count].type = TOKEN_TILDE;
            result->tokens[result->count].value = NULL;

            fprintf(out, "TILDE\n");

            result->count++;
            break;

        case '!':
            result->tokens[result->count].type = TOKEN_NOT;
            result->tokens[result->count].value = NULL;

            fprintf(out, "NOT\n");

            result->count++;
            break;

        case '^':
            result->tokens[result->count].type = TOKEN_XOR;
            result->tokens[result->count].value = NULL;

            fprintf(out, "XOR\n");

            result->count++;
            break;

        case '&':
            result->tokens[result->count].type = TOKEN_AND;
            result->tokens[result->count].value = NULL;

            fprintf(out, "AND\n");

            result->count++;
            break;

            default:
            fprintf(out,
                    "LEXICAL ERROR: Unexpected character '%c'\n",
                    src[i]);
            free(result->tokens);
            free(result);
            return NULL;
        }
        i++;
    }

    result->tokens[result->count].type = TOKEN_EOF;
    result->tokens[result->count].value = NULL;
    result->count++;

    fprintf(out, "EOF\n");

    return result;
}
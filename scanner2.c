#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    begin,
    end,
    read,
    write,
    id,
    intliteral,
    lparen,
    rparen,
    semicolon,
    comma,
    plusop,
    minusop,
    assignop,
    scan_eof
} token;

char token_buffer[256];

FILE *source;
FILE* output;

int line_number = 1;

void clear_buffer(void);
void buffer_char(int c);
token check_reserved(void);
void lexical_error(int c);
token scanner(void);
void print_token(token t);


void clear_buffer(void) {
    token_buffer[0] = '\0';
}

void buffer_char(int c) {
    int len = strlen(token_buffer);
    token_buffer[len] = (char)c;
    token_buffer[len + 1] = '\0';
}

token check_reserved(void) {
    if(strcmp(token_buffer, "begin") == 0) return begin;
    if(strcmp(token_buffer,"end") == 0) return end;
    if(strcmp(token_buffer,"read") == 0) return read;
    if(strcmp(token_buffer,"write") == 0) return write;
    return id;
}

void lexical_error(int c) {
    printf("Erreur lexicale (ligne %d) : caractere inattendu '%c'\n",
           line_number, c);
   // exit(1);
}

token scanner(void) {
    int in_char, c;

    clear_buffer();

    while ((in_char = fgetc(source)) != EOF) {

        if (in_char == '\n') {
            line_number++;
            continue;
        }

        if (isspace(in_char))
            continue;

        else if (isalpha(in_char)) {// cette bloque verifie si le caractere est une lettre puis il continue a lire les caracteres tant qu'ils sont alphanumeriques ou underscore et les stocke dans le buffer token_buffer, puis ungetc 
            buffer_char(in_char);
            for (c = fgetc(source); isalnum(c) || c == '_'; c = fgetc(source))
                buffer_char(c);
            ungetc(c, source);
            return check_reserved();
        }

        else if (isdigit(in_char)) {
            buffer_char(in_char);
            for (c = fgetc(source); isdigit(c); c = fgetc(source))
                buffer_char(c);
            ungetc(c, source);
            return intliteral;
        }

        else if (in_char == '(') return lparen;
        else if (in_char == ')') return rparen;
        else if (in_char == ';') return semicolon;
        else if (in_char == ',') return comma;
        else if (in_char == '+') return plusop;

        else if (in_char == '-') {
            c = fgetc(source);
            if (c == '-') {
                do {
                    in_char = fgetc(source);
                } while (in_char != '\n' && in_char != EOF);
                if (in_char == '\n') line_number++;
            } else {
                ungetc(c, source);
                return minusop;
            }
        }

        else if (in_char == ':') {
            c = fgetc(source);
            if (c == '=') return assignop;
            else {
                ungetc(c, source);
                lexical_error(in_char);
            }
        }

        else {
            lexical_error(in_char);
        }
    }

    return scan_eof;
}

void print_token(token t) {
    printf("[ligne %d] ", line_number);
    switch (t) {
        case id:         printf("ID(%s)\n", token_buffer); break;
        case intliteral: printf("INT(%s)\n", token_buffer); break;
        case lparen:     printf("LPAREN\n"); break;
        case rparen:     printf("RPAREN\n"); break;
        case semicolon:  printf("SEMICOLON\n"); break;
        case comma:      printf("COMMA\n"); break;
        case plusop:     printf("PLUSOP\n"); break;
        case minusop:    printf("MINUSOP\n"); break;
        case assignop:   printf("ASSIGNOP\n"); break;
        case scan_eof:    printf("SCANEOF\n"); break;
        default:         printf("ERREUR\n"); break;
    }
}

void print_token_to_output(token t) {
    fprintf(output, "[ligne %d] ", line_number);
    switch (t) {
        case id:         fprintf(output, "ID(%s)\n", token_buffer); break;
        case intliteral: fprintf(output, "INT(%s)\n", token_buffer); break;
        case lparen:     fprintf(output, "LPAREN\n"); break;
        case rparen:     fprintf(output, "RPAREN\n"); break;
        case semicolon:  fprintf(output, "SEMICOLON\n"); break;
        case comma:      fprintf(output, "COMMA\n"); break;
        case plusop:     fprintf(output, "PLUSOP\n"); break;
        case minusop:    fprintf(output, "MINUSOP\n"); break;
        case assignop:   fprintf(output, "ASSIGNOP\n"); break;
        case scan_eof:    fprintf(output, "SCANEOF\n"); break;
        default:         fprintf(output, "ERREUR\n"); break;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage : %s fichier_source fichier_output\n", argv[0]);
        return 1;
    }

    source = fopen(argv[1], "r");
    output = fopen(argv[2], "w");
    if (source == NULL) {
        printf("Impossible d'ouvrir le fichier %s\n", argv[1]);
        return 1;
    }
    if (output == NULL) {
        printf("Impossible d'ouvrir le fichier %s\n", argv[2]);
        return 1;
    }

    token t;
    do {
        t = scanner();
        print_token_to_output(t);
    } while (t != scan_eof);

    fclose(source);
    fclose(output);
    return 0;
}
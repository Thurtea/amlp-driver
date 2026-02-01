/*
 * lexer.c - Lexical Analyzer Implementation
 * 
 * This file implements the lexer for tokenizing LPC source code.
 * It handles reading source code, identifying tokens, and managing
 * position tracking for error reporting.
 */

#define _POSIX_C_SOURCE 200809L

#include "lexer.h"
#include <ctype.h>
#include <errno.h>

/* ========== Helper Function Declarations ========== */

static void lexer_skip_whitespace(Lexer *lexer);
static void lexer_skip_comment_impl(Lexer *lexer);
static int lexer_is_at_end(Lexer *lexer);
static char lexer_current_char(Lexer *lexer);
static char lexer_peek_char(Lexer *lexer, int offset);
static void lexer_advance(Lexer *lexer);

static Token lexer_read_identifier(Lexer *lexer);
static Token lexer_read_number(Lexer *lexer);
static Token lexer_read_string(Lexer *lexer, char quote);
static Token lexer_read_operator(Lexer *lexer);

static int is_keyword(const char *value);
static Token make_token(TokenType type, const char *value, int line, int column);


/* ========== Keyword Table ========== */

static const char *keywords[] = {
    "void", "int", "mixed", "string", "object", "mapping",
    "if", "else", "while", "for", "do", "switch", "case", "default", "break", "continue",
    "return", "function", "inherit", "private", "public", "protected", "static",
    "define", "ifdef", "ifndef", "endif", "include",
    "create", "init", "heart_beat", "die", "reset",
    NULL  /* Sentinel value */
};


/* ========== Utility Functions ========== */

/**
 * Checks if a string is an LPC keyword
 */
static int is_keyword(const char *value) {
    for (int i = 0; keywords[i] != NULL; i++) {
        if (strcmp(value, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

/**
 * Creates a token with the given information
 */
static Token make_token(TokenType type, const char *value, int line, int column) {
    Token token;
    token.type = type;
    token.value = value ? strdup(value) : NULL;
    token.line_number = line;
    token.column_number = column;
    return token;
}

/**
 * Check if we're at the end of the input
 */
static int lexer_is_at_end(Lexer *lexer) {
    return lexer->position >= lexer->buffer_size - 1;
}

/**
 * Get the current character without advancing
 */
static char lexer_current_char(Lexer *lexer) {
    if (lexer_is_at_end(lexer)) {
        return '\0';
    }
    return lexer->buffer[lexer->position];
}

/**
 * Peek ahead at a character with an offset
 */
static char lexer_peek_char(Lexer *lexer, int offset) {
    int pos = lexer->position + offset;
    if (pos >= lexer->buffer_size - 1) {
        return '\0';
    }
    return lexer->buffer[pos];
}

/**
 * Advance to the next character
 */
static void lexer_advance(Lexer *lexer) {
    if (lexer->buffer[lexer->position] == '\n') {
        lexer->line_number++;
        lexer->column_number = 1;
    } else {
        lexer->column_number++;
    }
    lexer->position++;
}

/**
 * Skip whitespace characters (spaces, tabs, newlines)
 */
static void lexer_skip_whitespace(Lexer *lexer) {
    while (!lexer_is_at_end(lexer) && isspace(lexer_current_char(lexer))) {
        lexer_advance(lexer);
    }
}

/* Helper to skip comments */
static void lexer_skip_comment_impl(Lexer *lexer) {
    if (lexer_current_char(lexer) == '/' && lexer_peek_char(lexer, 1) == '/') {
        /* Single-line comment */
        while (!lexer_is_at_end(lexer) && lexer_current_char(lexer) != '\n') {
            lexer_advance(lexer);
        }
        if (!lexer_is_at_end(lexer)) {
            lexer_advance(lexer);  /* Skip the newline */
        }
    } else if (lexer_current_char(lexer) == '/' && lexer_peek_char(lexer, 1) == '*') {
        /* Multi-line comment */
        lexer_advance(lexer);  /* Skip forward slash */
        lexer_advance(lexer);  /* Skip asterisk */

        while (!lexer_is_at_end(lexer)) {
            if (lexer_current_char(lexer) == '*' && lexer_peek_char(lexer, 1) == '/') {
                lexer_advance(lexer);  /* Skip asterisk */
                lexer_advance(lexer);  /* Skip forward slash */
                break;
            }
            lexer_advance(lexer);
        }
    }
}

/**
 * Read an identifier (variable name, function name, etc.)
 */
static Token lexer_read_identifier(Lexer *lexer) {
    int start_pos = lexer->position;
    int start_col = lexer->column_number;
    int start_line = lexer->line_number;

    while (!lexer_is_at_end(lexer) && (isalnum(lexer_current_char(lexer)) || lexer_current_char(lexer) == '_')) {
        lexer_advance(lexer);
    }

    int length = lexer->position - start_pos;
    char *value = malloc(length + 1);
    strncpy(value, &lexer->buffer[start_pos], length);
    value[length] = '\0';

    TokenType type = is_keyword(value) ? TOKEN_KEYWORD : TOKEN_IDENTIFIER;
    Token token;
    token.type = type;
    token.value = value;
    token.line_number = start_line;
    token.column_number = start_col;

    return token;
}

/**
 * Read a number (integer or float)
 */
static Token lexer_read_number(Lexer *lexer) {
    int start_pos = lexer->position;
    int start_col = lexer->column_number;
    int start_line = lexer->line_number;
    int is_float = 0;

    /* Read the integer part */
    while (!lexer_is_at_end(lexer) && isdigit(lexer_current_char(lexer))) {
        lexer_advance(lexer);
    }

    /* Check for decimal point */
    if (!lexer_is_at_end(lexer) && lexer_current_char(lexer) == '.' && isdigit(lexer_peek_char(lexer, 1))) {
        is_float = 1;
        lexer_advance(lexer);  /* Skip '.' */
        while (!lexer_is_at_end(lexer) && isdigit(lexer_current_char(lexer))) {
            lexer_advance(lexer);
        }
    }

    /* Check for exponent notation */
    if (!lexer_is_at_end(lexer) && (lexer_current_char(lexer) == 'e' || lexer_current_char(lexer) == 'E')) {
        is_float = 1;
        lexer_advance(lexer);
        if (!lexer_is_at_end(lexer) && (lexer_current_char(lexer) == '+' || lexer_current_char(lexer) == '-')) {
            lexer_advance(lexer);
        }
        while (!lexer_is_at_end(lexer) && isdigit(lexer_current_char(lexer))) {
            lexer_advance(lexer);
        }
    }

    int length = lexer->position - start_pos;
    char *value = malloc(length + 1);
    strncpy(value, &lexer->buffer[start_pos], length);
    value[length] = '\0';

    TokenType type = is_float ? TOKEN_FLOAT : TOKEN_NUMBER;
    Token token;
    token.type = type;
    token.value = value;
    token.line_number = start_line;
    token.column_number = start_col;

    return token;
}

/**
 * Read a string literal
 */
static Token lexer_read_string(Lexer *lexer, char quote) {
    int start_col = lexer->column_number;
    int start_line = lexer->line_number;
    
    lexer_advance(lexer);  /* Skip opening quote */

    int string_start = lexer->position;

    while (!lexer_is_at_end(lexer) && lexer_current_char(lexer) != quote) {
        if (lexer_current_char(lexer) == '\\') {
            lexer_advance(lexer);  /* Skip escape character */
            if (!lexer_is_at_end(lexer)) {
                lexer_advance(lexer);  /* Skip escaped character */
            }
        } else {
            lexer_advance(lexer);
        }
    }

    if (lexer_is_at_end(lexer)) {
        fprintf(stderr, "Unterminated string on line %d, column %d\n", start_line, start_col);
        Token token;
        token.type = TOKEN_ERROR;
        token.value = strdup("");
        token.line_number = start_line;
        token.column_number = start_col;
        return token;
    }

    int length = lexer->position - string_start;
    char *value = malloc(length + 1);
    strncpy(value, &lexer->buffer[string_start], length);
    value[length] = '\0';

    lexer_advance(lexer);  /* Skip closing quote */

    Token token;
    token.type = TOKEN_STRING;
    token.value = value;
    token.line_number = start_line;
    token.column_number = start_col;

    return token;
}

static Token lexer_read_operator(Lexer *lexer) {
    int start_col = lexer->column_number;
    int start_line = lexer->line_number;
    char ch = lexer_current_char(lexer);
    char next = lexer_peek_char(lexer, 1);

    char op_str[3] = {ch, next, '\0'};

    /* Check for multi-character operators */
    if ((ch == '=' && next == '=') ||
        (ch == '!' && next == '=') ||
        (ch == '<' && next == '=') ||
        (ch == '>' && next == '=') ||
        (ch == '+' && next == '+') ||
        (ch == '-' && next == '-') ||
        (ch == '+' && next == '=') ||
        (ch == '-' && next == '=') ||
        (ch == '*' && next == '=') ||
        (ch == '/' && next == '=') ||
        (ch == '&' && next == '&') ||
        (ch == '|' && next == '|')) {
        lexer_advance(lexer);
        lexer_advance(lexer);
        Token token = make_token(TOKEN_OPERATOR, op_str, start_line, start_col);
        return token;
    }

    /* Single character operator */
    op_str[1] = '\0';
    lexer_advance(lexer);
    Token token = make_token(TOKEN_OPERATOR, op_str, start_line, start_col);
    return token;
}


/* ========== Public API ========== */

/**
 * Initialize lexer from file
 */
Lexer* lexer_init(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file '%s': %s\n", filename, strerror(errno));
        return NULL;
    }

    /* Get file size */
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (size < 0) {
        fprintf(stderr, "Error: Cannot determine file size\n");
        fclose(file);
        return NULL;
    }

    /* Allocate buffer */
    char *buffer = malloc(size + 1);
    if (!buffer) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        fclose(file);
        return NULL;
    }

    /* Read file */
    size_t bytes_read = fread(buffer, 1, size, file);
    if (bytes_read != (size_t)size) {
        fprintf(stderr, "Error: Failed to read entire file\n");
        free(buffer);
        fclose(file);
        return NULL;
    }
    buffer[size] = '\0';
    fclose(file);

    /* Initialize lexer - use calloc to zero all memory */
    Lexer *lexer = calloc(1, sizeof(Lexer));
    if (!lexer) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        free(buffer);
        return NULL;
    }

    /* Explicitly initialize all fields */
    lexer->buffer = buffer;
    lexer->buffer_size = size + 1;
    lexer->position = 0;
    lexer->line_number = 1;
    lexer->column_number = 1;
    lexer->token_count = 0;

    return lexer;
}

/**
 * Initialize lexer from string
 */
Lexer* lexer_init_from_string(const char *source) {
    if (!source) {
        fprintf(stderr, "Error: source string is NULL\n");
        return NULL;
    }

    size_t size = strlen(source);
    char *buffer = malloc(size + 1);
    if (!buffer) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return NULL;
    }

    strcpy(buffer, source);

    /* CRITICAL: Use calloc to zero all memory - prevents uninitialized state */
    Lexer *lexer = calloc(1, sizeof(Lexer));
    if (!lexer) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        free(buffer);
        return NULL;
    }

    /* Explicitly initialize all fields (calloc already zeroed memory) */
    lexer->buffer = buffer;
    lexer->buffer_size = size + 1;
    lexer->position = 0;
    lexer->line_number = 1;
    lexer->column_number = 1;
    lexer->token_count = 0;

    return lexer;
}

/**
 * Get next token
 */
Token lexer_get_next_token(Lexer *lexer) {
    if (!lexer) {
        return make_token(TOKEN_ERROR, "NULL lexer", 0, 0);
    }

    while (!lexer_is_at_end(lexer)) {
        /* Skip whitespace */
        lexer_skip_whitespace(lexer);

        if (lexer_is_at_end(lexer)) {
            break;
        }

        char ch = lexer_current_char(lexer);

        /* Skip comments */
        if (ch == '/' && (lexer_peek_char(lexer, 1) == '/' || lexer_peek_char(lexer, 1) == '*')) {
            lexer_skip_comment_impl(lexer);
            continue;
        }

        /* Check for specific delimiters */
        switch (ch) {
            case '(':
                lexer_advance(lexer);
                return make_token(TOKEN_LPAREN, "(", lexer->line_number, lexer->column_number - 1);
            case ')':
                lexer_advance(lexer);
                return make_token(TOKEN_RPAREN, ")", lexer->line_number, lexer->column_number - 1);
            case '{':
                lexer_advance(lexer);
                return make_token(TOKEN_LBRACE, "{", lexer->line_number, lexer->column_number - 1);
            case '}':
                lexer_advance(lexer);
                return make_token(TOKEN_RBRACE, "}", lexer->line_number, lexer->column_number - 1);
            case '[':
                lexer_advance(lexer);
                return make_token(TOKEN_LBRACKET, "[", lexer->line_number, lexer->column_number - 1);
            case ']':
                lexer_advance(lexer);
                return make_token(TOKEN_RBRACKET, "]", lexer->line_number, lexer->column_number - 1);
            case ';':
                lexer_advance(lexer);
                return make_token(TOKEN_SEMICOLON, ";", lexer->line_number, lexer->column_number - 1);
            case ',':
                lexer_advance(lexer);
                return make_token(TOKEN_COMMA, ",", lexer->line_number, lexer->column_number - 1);
            case '.':
                if (isdigit(lexer_peek_char(lexer, 1))) {
                    return lexer_read_number(lexer);
                }
                lexer_advance(lexer);
                return make_token(TOKEN_DOT, ".", lexer->line_number, lexer->column_number - 1);
            case ':':
                lexer_advance(lexer);
                return make_token(TOKEN_COLON, ":", lexer->line_number, lexer->column_number - 1);
            case '"':
            case '\'':
                return lexer_read_string(lexer, ch);
            default:
                if (isalpha(ch) || ch == '_') {
                    return lexer_read_identifier(lexer);
                } else if (isdigit(ch)) {
                    return lexer_read_number(lexer);
                } else {
                    return lexer_read_operator(lexer);
                }
        }
    }

    return make_token(TOKEN_EOF, NULL, lexer->line_number, lexer->column_number);
}

/**
 * Peek at next token without consuming it
 */
Token lexer_peek_token(Lexer *lexer) {
    if (!lexer) {
        return make_token(TOKEN_ERROR, "NULL lexer", 0, 0);
    }

    int saved_pos = lexer->position;
    int saved_line = lexer->line_number;
    int saved_col = lexer->column_number;

    Token token = lexer_get_next_token(lexer);

    lexer->position = saved_pos;
    lexer->line_number = saved_line;
    lexer->column_number = saved_col;

    return token;
}

/**
 * Reset lexer to beginning
 */
void lexer_reset(Lexer *lexer) {
    if (!lexer) return;
    
    lexer->position = 0;
    lexer->line_number = 1;
    lexer->column_number = 1;
    lexer->token_count = 0;
}

/**
 * Free lexer resources
 */
void lexer_free(Lexer *lexer) {
    if (!lexer) return;
    
    if (lexer->buffer) {
        free(lexer->buffer);
    }
    free(lexer);
}

/**
 * Convert token type to string
 */
const char* token_type_to_string(TokenType type) {
    switch (type) {
        case TOKEN_IDENTIFIER:  return "IDENTIFIER";
        case TOKEN_NUMBER:      return "NUMBER";
        case TOKEN_FLOAT:       return "FLOAT";
        case TOKEN_STRING:      return "STRING";
        case TOKEN_KEYWORD:     return "KEYWORD";
        case TOKEN_OPERATOR:    return "OPERATOR";
        case TOKEN_LPAREN:      return "LPAREN";
        case TOKEN_RPAREN:      return "RPAREN";
        case TOKEN_LBRACE:      return "LBRACE";
        case TOKEN_RBRACE:      return "RBRACE";
        case TOKEN_LBRACKET:    return "LBRACKET";
        case TOKEN_RBRACKET:    return "RBRACKET";
        case TOKEN_SEMICOLON:   return "SEMICOLON";
        case TOKEN_COMMA:       return "COMMA";
        case TOKEN_DOT:         return "DOT";
        case TOKEN_COLON:       return "COLON";
        case TOKEN_EOF:         return "EOF";
        case TOKEN_ERROR:       return "ERROR";
        default:                return "UNKNOWN";
    }
}

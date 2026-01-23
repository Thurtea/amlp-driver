/*
 * parser.c - Parser Implementation
 * 
 * This file implements a recursive descent parser for LPC source code.
 * It consumes tokens from the lexer and builds an Abstract Syntax Tree (AST).
 */

#define _POSIX_C_SOURCE 200809L

#include "parser.h"
#include "lexer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

/* ========== Forward Declarations ========== */

static void parser_advance(Parser *parser);
static int parser_match(Parser *parser, TokenType type);
static int parser_check(Parser *parser, TokenType type);
static void parser_expect(Parser *parser, TokenType type);
static void parser_error(Parser *parser, const char *message);
static void parser_synchronize(Parser *parser);

static ASTNode* parser_parse_declaration(Parser *parser);
static ASTNode* parser_parse_statement(Parser *parser);
static ASTNode* parser_parse_expression(Parser *parser);
static ASTNode* parser_parse_assignment(Parser *parser);
static ASTNode* parser_parse_logical_or(Parser *parser);
static ASTNode* parser_parse_logical_and(Parser *parser);
static ASTNode* parser_parse_equality(Parser *parser);
static ASTNode* parser_parse_comparison(Parser *parser);
static ASTNode* parser_parse_addition(Parser *parser);
static ASTNode* parser_parse_multiplication(Parser *parser);
static ASTNode* parser_parse_unary(Parser *parser);
static ASTNode* parser_parse_postfix(Parser *parser);
static ASTNode* parser_parse_primary(Parser *parser);
static ASTNode* parser_parse_block(Parser *parser);
static char* parser_parse_type(Parser *parser);

static ASTNode* ast_node_create(ASTNodeType type, int line, int column);
static ProgramNode* program_node_create(void);
static void program_node_add_declaration(ProgramNode *node, ASTNode *decl);

/* ========== Utility Functions ========== */

/**
 * Advance to the next token
 */
static void parser_advance(Parser *parser) {
    parser->previous_token = parser->current_token;
    parser->current_token = lexer_get_next_token(parser->lexer);
}

/**
 * Check if current token matches type
 */
static int parser_check(Parser *parser, TokenType type) {
    return parser->current_token.type == type;
}

/**
 * Check if current token matches type and advance
 */
static int parser_match(Parser *parser, TokenType type) {
    if (parser_check(parser, type)) {
        parser_advance(parser);
        return 1;
    }
    return 0;
}

/**
 * Expect a specific token type or error
 */
static void parser_expect(Parser *parser, TokenType type) {
    if (!parser_check(parser, type)) {
        char msg[256];
        snprintf(msg, sizeof(msg), "Expected '%s' but found '%s'",
                 token_type_to_string(type),
                 token_type_to_string(parser->current_token.type));
        parser_error(parser, msg);
    }
    parser_advance(parser);
}

/**
 * Report a parse error
 */
static void parser_error(Parser *parser, const char *message) {
    fprintf(stderr, "[Parser Error] Line %d, Column %d: %s\n",
            parser->current_token.line_number,
            parser->current_token.column_number,
            message);
    parser->error_count++;
    parser->error_recovery_mode = 1;
}

/**
 * Attempt to recover from an error by skipping tokens
 */
static void parser_synchronize(Parser *parser) {
    parser_advance(parser);

    while (!parser_check(parser, TOKEN_EOF)) {
        if (parser->previous_token.type == TOKEN_SEMICOLON) {
            parser->error_recovery_mode = 0;
            return;
        }

        switch (parser->current_token.type) {
            case TOKEN_KEYWORD:
                parser->error_recovery_mode = 0;
                return;
            default:
                break;
        }

        parser_advance(parser);
    }
}

/**
 * Create a new AST node
 */
static ASTNode* ast_node_create(ASTNodeType type, int line, int column) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    node->type = type;
    node->line = line;
    node->column = column;
    node->data = NULL;
    return node;
}

/**
 * Create a program node
 */
static ProgramNode* program_node_create(void) {
    ProgramNode *node = malloc(sizeof(ProgramNode));
    if (!node) return NULL;
    node->declarations = malloc(sizeof(ASTNode*) * 10);
    node->declaration_count = 0;
    node->capacity = 10;
    return node;
}

/**
 * Add a declaration to a program node
 */
static void program_node_add_declaration(ProgramNode *prog, ASTNode *decl) {
    if (prog->declaration_count >= prog->capacity) {
        prog->capacity *= 2;
        prog->declarations = realloc(prog->declarations, sizeof(ASTNode*) * prog->capacity);
    }
    prog->declarations[prog->declaration_count++] = decl;
}

/**
 * Parse a type specification
 */
static char* parser_parse_type(Parser *parser) {
    char *type_str = NULL;

    if (parser_match(parser, TOKEN_KEYWORD)) {
        type_str = strdup(parser->previous_token.value);
    } else if (parser_match(parser, TOKEN_IDENTIFIER)) {
        type_str = strdup(parser->previous_token.value);
    } else {
        parser_error(parser, "Expected type specifier");
        return strdup("mixed");
    }

    /* Handle array types (e.g., int[]) */
    if (parser_match(parser, TOKEN_LBRACKET)) {
        parser_expect(parser, TOKEN_RBRACKET);
        char *array_type = malloc(strlen(type_str) + 3);
        sprintf(array_type, "%s[]", type_str);
        free(type_str);
        type_str = array_type;
    }

    return type_str;
}

/* ========== Expression Parsing ========== */

/**
 * Parse primary expressions (literals, identifiers, parenthesized expressions)
 */
static ASTNode* parser_parse_primary(Parser *parser) {
    ASTNode *node = NULL;
    int line = parser->current_token.line_number;
    int column = parser->current_token.column_number;

    if (parser_match(parser, TOKEN_NUMBER)) {
        node = ast_node_create(NODE_LITERAL_NUMBER, line, column);
        LiteralNumberNode *num_node = malloc(sizeof(LiteralNumberNode));
        num_node->int_value = strtol(parser->previous_token.value, NULL, 10);
        num_node->is_float = 0;
        node->data = num_node;
    } else if (parser_match(parser, TOKEN_FLOAT)) {
        node = ast_node_create(NODE_LITERAL_NUMBER, line, column);
        LiteralNumberNode *num_node = malloc(sizeof(LiteralNumberNode));
        num_node->float_value = strtod(parser->previous_token.value, NULL);
        num_node->is_float = 1;
        node->data = num_node;
    } else if (parser_match(parser, TOKEN_STRING)) {
        node = ast_node_create(NODE_LITERAL_STRING, line, column);
        LiteralStringNode *str_node = malloc(sizeof(LiteralStringNode));
        str_node->value = strdup(parser->previous_token.value);
        node->data = str_node;
    } else if (parser_match(parser, TOKEN_IDENTIFIER)) {
        node = ast_node_create(NODE_IDENTIFIER, line, column);
        IdentifierNode *id_node = malloc(sizeof(IdentifierNode));
        id_node->name = strdup(parser->previous_token.value);
        node->data = id_node;
    } else if (parser_match(parser, TOKEN_LPAREN)) {
        node = parser_parse_expression(parser);
        parser_expect(parser, TOKEN_RPAREN);
    } else {
        char msg[256];
        snprintf(msg, sizeof(msg), "Unexpected token: %s", parser->current_token.value);
        parser_error(parser, msg);
        node = ast_node_create(NODE_IDENTIFIER, line, column);
    }

    return node;
}

/**
 * Parse postfix expressions (function calls, array access, member access)
 */
static ASTNode* parser_parse_postfix(Parser *parser) {
    ASTNode *node = parser_parse_primary(parser);

    while (1) {
        if (parser_match(parser, TOKEN_LPAREN)) {
            /* Function call */
            int line = parser->previous_token.line_number;
            int column = parser->previous_token.column_number;
            ASTNode *call_node = ast_node_create(NODE_FUNCTION_CALL, line, column);
            FunctionCallNode *fcall = malloc(sizeof(FunctionCallNode));
            
            if (node->type == NODE_IDENTIFIER) {
                IdentifierNode *id = (IdentifierNode*)node->data;
                fcall->function_name = strdup(id->name);
                free(id);
                free(node);
            } else {
                fcall->function_name = strdup("unknown");
            }

            fcall->arguments = malloc(sizeof(ASTNode*) * 10);
            fcall->argument_count = 0;
            fcall->capacity = 10;

            if (!parser_check(parser, TOKEN_RPAREN)) {
                do {
                    ASTNode *arg = parser_parse_assignment(parser);
                    if (fcall->argument_count >= fcall->capacity) {
                        fcall->capacity *= 2;
                        fcall->arguments = realloc(fcall->arguments, sizeof(ASTNode*) * fcall->capacity);
                    }
                    fcall->arguments[fcall->argument_count++] = arg;
                } while (parser_match(parser, TOKEN_COMMA));
            }

            parser_expect(parser, TOKEN_RPAREN);
            call_node->data = fcall;
            node = call_node;
        } else if (parser_match(parser, TOKEN_LBRACKET)) {
            /* Array access */
            int line = parser->previous_token.line_number;
            int column = parser->previous_token.column_number;
            ASTNode *access_node = ast_node_create(NODE_ARRAY_ACCESS, line, column);
            ArrayAccessNode *arr_access = malloc(sizeof(ArrayAccessNode));
            arr_access->array = node;
            arr_access->index = parser_parse_expression(parser);
            parser_expect(parser, TOKEN_RBRACKET);
            access_node->data = arr_access;
            node = access_node;
        } else if (parser_match(parser, TOKEN_DOT)) {
            /* Member access */
            int line = parser->previous_token.line_number;
            int column = parser->previous_token.column_number;
            parser_expect(parser, TOKEN_IDENTIFIER);
            ASTNode *member_node = ast_node_create(NODE_MEMBER_ACCESS, line, column);
            MemberAccessNode *member = malloc(sizeof(MemberAccessNode));
            member->object = node;
            member->member = strdup(parser->previous_token.value);
            member_node->data = member;
            node = member_node;
        } else {
            break;
        }
    }

    return node;
}

/**
 * Parse unary expressions (prefix ++, --, -, !, etc.)
 */
static ASTNode* parser_parse_unary(Parser *parser) {
    if (parser->current_token.type == TOKEN_OPERATOR) {
        const char *op_str = parser->current_token.value;
        if (strcmp(op_str, "-") == 0 || strcmp(op_str, "!") == 0 || 
            strcmp(op_str, "~") == 0 || strcmp(op_str, "++") == 0 || 
            strcmp(op_str, "--") == 0) {
            
            int line = parser->current_token.line_number;
            int column = parser->current_token.column_number;
            parser_advance(parser);
            
            ASTNode *unary_node = ast_node_create(NODE_UNARY_OP, line, column);
            UnaryOpNode *unary = malloc(sizeof(UnaryOpNode));
            unary->operator = strdup(op_str);
            unary->operand = parser_parse_unary(parser);
            unary->is_prefix = 1;
            unary_node->data = unary;
            return unary_node;
        }
    }

    return parser_parse_postfix(parser);
}

/**
 * Parse multiplication and division expressions
 */
static ASTNode* parser_parse_multiplication(Parser *parser) {
    ASTNode *node = parser_parse_unary(parser);

    while (parser->current_token.type == TOKEN_OPERATOR) {
        const char *op = parser->current_token.value;
        if (strcmp(op, "*") != 0 && strcmp(op, "/") != 0 && strcmp(op, "%") != 0) {
            break;
        }

        int line = parser->current_token.line_number;
        int column = parser->current_token.column_number;
        parser_advance(parser);

        ASTNode *binop_node = ast_node_create(NODE_BINARY_OP, line, column);
        BinaryOpNode *binop = malloc(sizeof(BinaryOpNode));
        binop->left = node;
        binop->operator = strdup(op);
        binop->right = parser_parse_unary(parser);
        binop_node->data = binop;
        node = binop_node;
    }

    return node;
}

/**
 * Parse addition and subtraction expressions
 */
static ASTNode* parser_parse_addition(Parser *parser) {
    ASTNode *node = parser_parse_multiplication(parser);

    while (parser->current_token.type == TOKEN_OPERATOR) {
        const char *op = parser->current_token.value;
        if (strcmp(op, "+") != 0 && strcmp(op, "-") != 0) {
            break;
        }

        int line = parser->current_token.line_number;
        int column = parser->current_token.column_number;
        parser_advance(parser);

        ASTNode *binop_node = ast_node_create(NODE_BINARY_OP, line, column);
        BinaryOpNode *binop = malloc(sizeof(BinaryOpNode));
        binop->left = node;
        binop->operator = strdup(op);
        binop->right = parser_parse_multiplication(parser);
        binop_node->data = binop;
        node = binop_node;
    }

    return node;
}

/**
 * Parse comparison expressions (<, >, <=, >=)
 */
static ASTNode* parser_parse_comparison(Parser *parser) {
    ASTNode *node = parser_parse_addition(parser);

    while (parser->current_token.type == TOKEN_OPERATOR) {
        const char *op = parser->current_token.value;
        if (strcmp(op, "<") != 0 && strcmp(op, ">") != 0 && 
            strcmp(op, "<=") != 0 && strcmp(op, ">=") != 0) {
            break;
        }

        int line = parser->current_token.line_number;
        int column = parser->current_token.column_number;
        parser_advance(parser);

        ASTNode *binop_node = ast_node_create(NODE_BINARY_OP, line, column);
        BinaryOpNode *binop = malloc(sizeof(BinaryOpNode));
        binop->left = node;
        binop->operator = strdup(op);
        binop->right = parser_parse_addition(parser);
        binop_node->data = binop;
        node = binop_node;
    }

    return node;
}

/**
 * Parse equality expressions (==, !=)
 */
static ASTNode* parser_parse_equality(Parser *parser) {
    ASTNode *node = parser_parse_comparison(parser);

    while (parser->current_token.type == TOKEN_OPERATOR) {
        const char *op = parser->current_token.value;
        if (strcmp(op, "==") != 0 && strcmp(op, "!=") != 0) {
            break;
        }

        int line = parser->current_token.line_number;
        int column = parser->current_token.column_number;
        parser_advance(parser);

        ASTNode *binop_node = ast_node_create(NODE_BINARY_OP, line, column);
        BinaryOpNode *binop = malloc(sizeof(BinaryOpNode));
        binop->left = node;
        binop->operator = strdup(op);
        binop->right = parser_parse_comparison(parser);
        binop_node->data = binop;
        node = binop_node;
    }

    return node;
}

/**
 * Parse logical AND expressions (&&)
 */
static ASTNode* parser_parse_logical_and(Parser *parser) {
    ASTNode *node = parser_parse_equality(parser);

    while (parser->current_token.type == TOKEN_OPERATOR && strcmp(parser->current_token.value, "&&") == 0) {
        int line = parser->current_token.line_number;
        int column = parser->current_token.column_number;
        parser_advance(parser);

        ASTNode *binop_node = ast_node_create(NODE_BINARY_OP, line, column);
        BinaryOpNode *binop = malloc(sizeof(BinaryOpNode));
        binop->left = node;
        binop->operator = strdup("&&");
        binop->right = parser_parse_equality(parser);
        binop_node->data = binop;
        node = binop_node;
    }

    return node;
}

/**
 * Parse logical OR expressions (||)
 */
static ASTNode* parser_parse_logical_or(Parser *parser) {
    ASTNode *node = parser_parse_logical_and(parser);

    while (parser->current_token.type == TOKEN_OPERATOR && strcmp(parser->current_token.value, "||") == 0) {
        int line = parser->current_token.line_number;
        int column = parser->current_token.column_number;
        parser_advance(parser);

        ASTNode *binop_node = ast_node_create(NODE_BINARY_OP, line, column);
        BinaryOpNode *binop = malloc(sizeof(BinaryOpNode));
        binop->left = node;
        binop->operator = strdup("||");
        binop->right = parser_parse_logical_and(parser);
        binop_node->data = binop;
        node = binop_node;
    }

    return node;
}

/**
 * Parse assignment expressions (=, +=, -=, etc.)
 */
static ASTNode* parser_parse_assignment(Parser *parser) {
    ASTNode *node = parser_parse_logical_or(parser);

    if (parser->current_token.type == TOKEN_OPERATOR) {
        const char *op = parser->current_token.value;
        if (strcmp(op, "=") == 0 || strcmp(op, "+=") == 0 || strcmp(op, "-=") == 0 ||
            strcmp(op, "*=") == 0 || strcmp(op, "/=") == 0) {
            
            int line = parser->current_token.line_number;
            int column = parser->current_token.column_number;
            parser_advance(parser);

            ASTNode *assign_node = ast_node_create(NODE_ASSIGNMENT, line, column);
            AssignmentNode *assign = malloc(sizeof(AssignmentNode));
            assign->target = node;
            assign->operator = strdup(op);
            assign->value = parser_parse_assignment(parser);
            assign_node->data = assign;
            return assign_node;
        }
    }

    return node;
}

/**
 * Parse complete expressions
 */
static ASTNode* parser_parse_expression(Parser *parser) {
    return parser_parse_assignment(parser);
}

/* ========== Statement Parsing ========== */

/**
 * Parse a block statement
 */
static ASTNode* parser_parse_block(Parser *parser) {
    int line = parser->current_token.line_number;
    int column = parser->current_token.column_number;
    parser_expect(parser, TOKEN_LBRACE);

    ASTNode *block_node = ast_node_create(NODE_BLOCK, line, column);
    BlockNode *block = malloc(sizeof(BlockNode));
    block->statements = malloc(sizeof(ASTNode*) * 20);
    block->statement_count = 0;
    block->capacity = 20;
    block_node->data = block;

    while (!parser_check(parser, TOKEN_RBRACE) && !parser_check(parser, TOKEN_EOF)) {
        ASTNode *stmt = parser_parse_statement(parser);
        if (stmt) {
            if (block->statement_count >= block->capacity) {
                block->capacity *= 2;
                block->statements = realloc(block->statements, sizeof(ASTNode*) * block->capacity);
            }
            block->statements[block->statement_count++] = stmt;
        }
    }

    parser_expect(parser, TOKEN_RBRACE);
    return block_node;
}

/**
 * Parse a statement
 */
static ASTNode* parser_parse_statement(Parser *parser) {
    /* If statement */
    if (parser_match(parser, TOKEN_KEYWORD) && strcmp(parser->previous_token.value, "if") == 0) {
        int line = parser->previous_token.line_number;
        int column = parser->previous_token.column_number;
        parser_expect(parser, TOKEN_LPAREN);
        ASTNode *if_node = ast_node_create(NODE_IF_STATEMENT, line, column);
        IfStatementNode *if_stmt = malloc(sizeof(IfStatementNode));
        if_stmt->condition = parser_parse_expression(parser);
        parser_expect(parser, TOKEN_RPAREN);
        if_stmt->then_statement = parser_parse_statement(parser);
        
        if_stmt->else_statement = NULL;
        if (parser_match(parser, TOKEN_KEYWORD) && strcmp(parser->previous_token.value, "else") == 0) {
            if_stmt->else_statement = parser_parse_statement(parser);
        }
        
        if_node->data = if_stmt;
        return if_node;
    }

    /* While loop */
    if (parser_match(parser, TOKEN_KEYWORD) && strcmp(parser->previous_token.value, "while") == 0) {
        int line = parser->previous_token.line_number;
        int column = parser->previous_token.column_number;
        parser_expect(parser, TOKEN_LPAREN);
        ASTNode *while_node = ast_node_create(NODE_WHILE_LOOP, line, column);
        WhileLoopNode *while_stmt = malloc(sizeof(WhileLoopNode));
        while_stmt->condition = parser_parse_expression(parser);
        parser_expect(parser, TOKEN_RPAREN);
        while_stmt->body = parser_parse_statement(parser);
        while_node->data = while_stmt;
        return while_node;
    }

    /* Return statement */
    if (parser_match(parser, TOKEN_KEYWORD) && strcmp(parser->previous_token.value, "return") == 0) {
        int line = parser->previous_token.line_number;
        int column = parser->previous_token.column_number;
        ASTNode *ret_node = ast_node_create(NODE_RETURN_STATEMENT, line, column);
        ReturnStatementNode *ret_stmt = malloc(sizeof(ReturnStatementNode));
        
        if (!parser_check(parser, TOKEN_SEMICOLON)) {
            ret_stmt->value = parser_parse_expression(parser);
        } else {
            ret_stmt->value = NULL;
        }
        
        parser_expect(parser, TOKEN_SEMICOLON);
        ret_node->data = ret_stmt;
        return ret_node;
    }

    /* Break statement */
    if (parser_match(parser, TOKEN_KEYWORD) && strcmp(parser->previous_token.value, "break") == 0) {
        int line = parser->previous_token.line_number;
        int column = parser->previous_token.column_number;
        parser_expect(parser, TOKEN_SEMICOLON);
        return ast_node_create(NODE_BREAK_STATEMENT, line, column);
    }

    /* Continue statement */
    if (parser_match(parser, TOKEN_KEYWORD) && strcmp(parser->previous_token.value, "continue") == 0) {
        int line = parser->previous_token.line_number;
        int column = parser->previous_token.column_number;
        parser_expect(parser, TOKEN_SEMICOLON);
        return ast_node_create(NODE_CONTINUE_STATEMENT, line, column);
    }

    /* Block statement */
    if (parser_check(parser, TOKEN_LBRACE)) {
        return parser_parse_block(parser);
    }

    /* Expression statement */
    ASTNode *expr = parser_parse_expression(parser);
    parser_expect(parser, TOKEN_SEMICOLON);
    
    ASTNode *expr_stmt = ast_node_create(NODE_EXPRESSION_STATEMENT, expr->line, expr->column);
    expr_stmt->data = expr;
    return expr_stmt;
}

/* ========== Declaration Parsing ========== */

/**
 * Parse a declaration (function or variable)
 */
static ASTNode* parser_parse_declaration(Parser *parser) {
    char *type = parser_parse_type(parser);
    int line = parser->previous_token.line_number;
    int column = parser->previous_token.column_number;

    if (!parser_match(parser, TOKEN_IDENTIFIER) && !parser_match(parser, TOKEN_KEYWORD)) {
        parser_error(parser, "Expected identifier after type");
        free(type);
        return NULL;
    }

    char *name = strdup(parser->previous_token.value);

    /* Function declaration */
    if (parser_match(parser, TOKEN_LPAREN)) {
        ASTNode *func_node = ast_node_create(NODE_FUNCTION_DECL, line, column);
        FunctionDeclNode *func = malloc(sizeof(FunctionDeclNode));
        func->return_type = type;
        func->name = name;
        func->parameters = NULL;
        func->parameter_count = 0;
        func->is_private = 0;
        func->is_static = 0;

        /* Parse parameters (allow empty list) */
        if (!parser_check(parser, TOKEN_RPAREN)) {
            func->parameters = malloc(sizeof(struct {char *type; char *name;}) * 10);
            do {
                char *param_type = parser_parse_type(parser);
                if (!parser_match(parser, TOKEN_IDENTIFIER)) {
                    parser_error(parser, "Expected parameter name");
                    break;
                }
                func->parameters[func->parameter_count].type = param_type;
                func->parameters[func->parameter_count].name = strdup(parser->previous_token.value);
                func->parameter_count++;
            } while (parser_match(parser, TOKEN_COMMA));
        }

        parser_expect(parser, TOKEN_RPAREN);
        func->body = parser_parse_block(parser);
        func_node->data = func;
        return func_node;
    }

    /* Variable declaration */
    ASTNode *var_node = ast_node_create(NODE_VARIABLE_DECL, line, column);
    VariableDeclNode *var = malloc(sizeof(VariableDeclNode));
    var->type = type;
    var->name = name;
    var->is_private = 0;
    var->is_static = 0;
    var->initializer = NULL;

    if (parser_match(parser, TOKEN_OPERATOR) && strcmp(parser->previous_token.value, "=") == 0) {
        var->initializer = parser_parse_expression(parser);
    }

    parser_expect(parser, TOKEN_SEMICOLON);
    var_node->data = var;
    return var_node;
}

/* ========== Public API ========== */

/**
 * Initialize parser
 */
Parser* parser_init(Lexer *lexer) {
    if (!lexer) {
        fprintf(stderr, "Error: Cannot initialize parser with NULL lexer\n");
        return NULL;
    }

    Parser *parser = malloc(sizeof(Parser));
    if (!parser) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return NULL;
    }

    parser->lexer = lexer;
    parser->current_token = lexer_get_next_token(lexer);
    parser->error_count = 0;
    parser->error_recovery_mode = 0;

    return parser;
}

/**
 * Parse the program
 */
ASTNode* parser_parse(Parser *parser) {
    if (!parser) {
        fprintf(stderr, "Error: Cannot parse with NULL parser\n");
        return NULL;
    }

    ProgramNode *program = program_node_create();
    ASTNode *prog_node = ast_node_create(NODE_PROGRAM, 1, 1);
    prog_node->data = program;

    while (!parser_check(parser, TOKEN_EOF)) {
        ASTNode *decl = parser_parse_declaration(parser);
        if (decl) {
            program_node_add_declaration(program, decl);
        } else if (parser->error_recovery_mode) {
            parser_synchronize(parser);
        } else {
            parser_advance(parser);
        }
    }

    if (parser->error_count > 0) {
        fprintf(stderr, "[Parser] Completed with %d error(s)\n", parser->error_count);
    }

    return prog_node;
}

/**
 * Free parser resources
 */
void parser_free(Parser *parser) {
    if (!parser) return;
    free(parser);
}

/**
 * Recursively free AST nodes
 */
void ast_node_free(ASTNode *node) {
    if (!node) return;

    switch (node->type) {
        case NODE_PROGRAM: {
            ProgramNode *prog = (ProgramNode*)node->data;
            for (int i = 0; i < prog->declaration_count; i++) {
                ast_node_free(prog->declarations[i]);
            }
            free(prog->declarations);
            free(prog);
            break;
        }
        case NODE_BLOCK: {
            BlockNode *block = (BlockNode*)node->data;
            for (int i = 0; i < block->statement_count; i++) {
                ast_node_free(block->statements[i]);
            }
            free(block->statements);
            free(block);
            break;
        }
        case NODE_FUNCTION_CALL: {
            FunctionCallNode *call = (FunctionCallNode*)node->data;
            free(call->function_name);
            for (int i = 0; i < call->argument_count; i++) {
                ast_node_free(call->arguments[i]);
            }
            free(call->arguments);
            free(call);
            break;
        }
        case NODE_BINARY_OP: {
            BinaryOpNode *binop = (BinaryOpNode*)node->data;
            ast_node_free(binop->left);
            ast_node_free(binop->right);
            free(binop->operator);
            free(binop);
            break;
        }
        case NODE_LITERAL_STRING: {
            LiteralStringNode *str = (LiteralStringNode*)node->data;
            free(str->value);
            free(str);
            break;
        }
        case NODE_IDENTIFIER: {
            IdentifierNode *id = (IdentifierNode*)node->data;
            free(id->name);
            free(id);
            break;
        }
        default:
            if (node->data) free(node->data);
            break;
    }

    free(node);
}

/**
 * Convert AST node type to string
 */
const char* ast_node_to_string(ASTNodeType type) {
    switch (type) {
        case NODE_PROGRAM:          return "PROGRAM";
        case NODE_FUNCTION_DECL:    return "FUNCTION_DECL";
        case NODE_VARIABLE_DECL:    return "VARIABLE_DECL";
        case NODE_BLOCK:            return "BLOCK";
        case NODE_IF_STATEMENT:     return "IF_STATEMENT";
        case NODE_WHILE_LOOP:       return "WHILE_LOOP";
        case NODE_RETURN_STATEMENT: return "RETURN_STATEMENT";
        case NODE_BINARY_OP:        return "BINARY_OP";
        case NODE_UNARY_OP:         return "UNARY_OP";
        case NODE_ASSIGNMENT:       return "ASSIGNMENT";
        case NODE_FUNCTION_CALL:    return "FUNCTION_CALL";
        case NODE_LITERAL_NUMBER:   return "LITERAL_NUMBER";
        case NODE_LITERAL_STRING:   return "LITERAL_STRING";
        case NODE_IDENTIFIER:       return "IDENTIFIER";
        default:                    return "UNKNOWN";
    }
}

/**
 * Print AST in readable format
 */
void parser_print_ast(ASTNode *node, int indent) {
    if (!node) return;

    for (int i = 0; i < indent; i++) printf("  ");
    printf("[%s]\n", ast_node_to_string(node->type));

    switch (node->type) {
        case NODE_PROGRAM: {
            ProgramNode *prog = (ProgramNode*)node->data;
            for (int i = 0; i < prog->declaration_count; i++) {
                parser_print_ast(prog->declarations[i], indent + 1);
            }
            break;
        }
        case NODE_FUNCTION_CALL: {
            FunctionCallNode *call = (FunctionCallNode*)node->data;
            for (int i = 0; i < indent + 1; i++) printf("  ");
            printf("Function: %s\n", call->function_name);
            for (int i = 0; i < call->argument_count; i++) {
                parser_print_ast(call->arguments[i], indent + 1);
            }
            break;
        }
        case NODE_IDENTIFIER: {
            IdentifierNode *id = (IdentifierNode*)node->data;
            for (int i = 0; i < indent + 1; i++) printf("  ");
            printf("Name: %s\n", id->name);
            break;
        }
        default:
            break;
    }
}

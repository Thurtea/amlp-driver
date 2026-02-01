#ifndef PARSER_STATE_H
#define PARSER_STATE_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* Magic number for state validation */
#define PARSER_STATE_MAGIC 0x50415253  /* "PARS" */

/* State validation macros */
#define PARSER_STATE_INIT(state) \
    do { \
        (state)->magic = PARSER_STATE_MAGIC; \
        (state)->initialized = 1; \
    } while(0)

#define PARSER_STATE_CHECK(state) \
    do { \
        if ((state)->magic != PARSER_STATE_MAGIC) { \
            fprintf(stderr, "[PARSER] FATAL: State corruption detected (magic=0x%x, expected=0x%x)\n", \
                    (state)->magic, PARSER_STATE_MAGIC); \
            abort(); \
        } \
        if (!(state)->initialized) { \
            fprintf(stderr, "[PARSER] FATAL: Using uninitialized parser state\n"); \
            abort(); \
        } \
    } while(0)

#define PARSER_STATE_CLEAR(state) \
    do { \
        (state)->magic = 0; \
        (state)->initialized = 0; \
    } while(0)

#endif /* PARSER_STATE_H */

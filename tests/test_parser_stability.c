/*
 * Parser Stability Test Harness
 * Tests parser determinism by compiling identical source repeatedly
 * and checking for state corruption between runs.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../src/compiler.h"
#include "../src/lexer.h"
#include "../src/parser.h"
#include "../src/program.h"

/* Test configurations */
#define TEST_ITERATIONS 100
#define MAX_TEST_SOURCES 10

/* Test source samples - progressively more complex */
static const char *test_sources[] = {
    /* Minimal empty object */
    "void create() { }",

    /* With variable */
    "int x;\nvoid create() { x = 5; }",

    /* With string variable */
    "string name;\nvoid create() { name = \"test\"; }",

    /* With function parameter */
    "void setup_player(string n) { }",

    /* Multiple functions */
    "void create() { }\nvoid setup_player(string n, string p) { }",

    /* With efun call */
    "void create() { write(\"hello\"); }",

    /* With conditional */
    "void create() { if (1) { } }",

    /* With variable and function */
    "string pname;\nvoid create() { }\nvoid setup_player(string n) { pname = n; }",

    NULL
};

typedef struct {
    int iteration;
    int success;
    char error_msg[512];
    int token_count;
    int line_count;
} CompileResult;

/* Compile source and capture detailed results */
static CompileResult compile_and_analyze(const char *source, const char *test_name, int iteration) {
    CompileResult result = {0};
    result.iteration = iteration;

    /* Count lines in source */
    const char *p = source;
    result.line_count = 1;
    while (*p) {
        if (*p == '\n') result.line_count++;
        p++;
    }

    /* Count tokens via separate lexer */
    Lexer *token_counter = lexer_init_from_string(source);
    if (token_counter) {
        while (1) {
            Token tok = lexer_get_next_token(token_counter);
            if (tok.value) free(tok.value);
            if (tok.type == TOKEN_EOF) break;
            result.token_count++;
        }
        lexer_free(token_counter);
    }

    /* Attempt compilation */
    char filename[128];
    snprintf(filename, sizeof(filename), "test_%s_iter_%d.lpc", test_name, iteration);

    Program *prog = compiler_compile_string(source, filename);

    if (prog) {
        result.success = 1;
        /* Verify program structure */
        if (!prog->bytecode) {
            snprintf(result.error_msg, sizeof(result.error_msg),
                     "Program compiled but has no bytecode");
            result.success = 0;
        }
        program_free(prog);
    } else {
        result.success = 0;
        snprintf(result.error_msg, sizeof(result.error_msg),
                 "Compilation failed (check stderr)");
    }

    return result;
}

/* Test determinism by compiling same source N times */
static int test_determinism(const char *source, const char *test_name, int iterations) {
    printf("\n=== Testing: %s ===\n", test_name);
    printf("Source (%zu bytes):\n%s\n\n", strlen(source), source);

    CompileResult results[iterations];
    int success_count = 0;
    int failure_count = 0;
    int first_failure = -1;

    for (int i = 0; i < iterations; i++) {
        results[i] = compile_and_analyze(source, test_name, i);

        if (results[i].success) {
            success_count++;
        } else {
            failure_count++;
            if (first_failure == -1) first_failure = i;
        }

        /* Print progress every 10 iterations */
        if ((i + 1) % 10 == 0) {
            printf("  Progress: %d/%d compiled (%.1f%% success)\n",
                   i + 1, iterations,
                   100.0 * success_count / (i + 1));
        }
    }

    printf("\n--- Results ---\n");
    printf("  Total iterations: %d\n", iterations);
    printf("  Successful: %d (%.1f%%)\n", success_count, 100.0 * success_count / iterations);
    printf("  Failed: %d (%.1f%%)\n", failure_count, 100.0 * failure_count / iterations);

    if (failure_count > 0) {
        printf("\n--- First Failure (iteration %d) ---\n", first_failure);
        printf("  Error: %s\n", results[first_failure].error_msg);
        printf("  Tokens expected: %d\n", results[0].token_count);
        printf("  Tokens counted: %d\n", results[first_failure].token_count);

        /* Show pattern of failures */
        printf("\n--- Failure Pattern ---\n  ");
        for (int i = 0; i < iterations && i < 100; i++) {
            printf("%c", results[i].success ? '.' : 'X');
            if ((i + 1) % 50 == 0) printf("\n  ");
        }
        printf("\n");
    }

    /* Determinism check: all should succeed or all should fail */
    int is_deterministic = (success_count == iterations || failure_count == iterations);
    if (is_deterministic) {
        printf("\n✓ DETERMINISTIC: All %d iterations had same outcome\n", iterations);
    } else {
        printf("\n✗ NON-DETERMINISTIC: Mixed success/failure indicates state corruption\n");
    }

    return is_deterministic ? 0 : 1;
}

/* Test parser state isolation between compilations */
static int test_state_isolation() {
    printf("\n\n=== Testing State Isolation ===\n");
    printf("Compiling different sources in sequence to check for state bleed\n\n");

    int failures = 0;

    /* Compile each test source twice in sequence */
    for (int i = 0; test_sources[i] != NULL; i++) {
        char test_name[64];
        snprintf(test_name, sizeof(test_name), "source_%d", i);

        /* First compilation */
        Program *prog1 = compiler_compile_string(test_sources[i], test_name);
        if (!prog1) {
            printf("  [%s] First compile: FAILED\n", test_name);
            failures++;
            continue;
        }
        program_free(prog1);

        /* Second compilation of same source */
        Program *prog2 = compiler_compile_string(test_sources[i], test_name);
        if (!prog2) {
            printf("  [%s] Second compile: FAILED (state corruption likely)\n", test_name);
            failures++;
            continue;
        }
        program_free(prog2);

        printf("  [%s] Both compiles: OK\n", test_name);
    }

    printf("\n--- State Isolation Results ---\n");
    int num_sources = 0;
    while (test_sources[num_sources] != NULL) num_sources++;
    printf("  Test sources: %d\n", num_sources);
    printf("  Failures: %d\n", failures);

    return failures;
}

/* Main test runner */
int main(int argc, char **argv) {
    printf("Parser Stability Test Harness\n");
    printf("==============================\n");
    printf("PID: %d\n", getpid());
    printf("Iterations per test: %d\n", TEST_ITERATIONS);

    int total_failures = 0;

    /* Test minimal source determinism */
    total_failures += test_determinism(
        test_sources[0],
        "minimal",
        TEST_ITERATIONS
    );

    /* Test with variable */
    total_failures += test_determinism(
        test_sources[1],
        "with_variable",
        TEST_ITERATIONS
    );

    /* Test state isolation */
    total_failures += test_state_isolation();

    printf("\n\n=== FINAL SUMMARY ===\n");
    if (total_failures == 0) {
        printf("✓ ALL TESTS PASSED - Parser is stable and deterministic\n");
        return 0;
    } else {
        printf("✗ FAILURES DETECTED - Parser has state management issues\n");
        printf("  Total test failures: %d\n", total_failures);
        printf("\nCheck /tmp/amlp_tokens_*.log and /tmp/amlp_last_loaded_source.lpc for details\n");
        return 1;
    }
}

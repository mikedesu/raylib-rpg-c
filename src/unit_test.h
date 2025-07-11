#pragma once
#include <stdio.h>
#include <stdlib.h>

static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

#define ASSERT(cond, msg)                                                                                                                                                          \
    do {                                                                                                                                                                           \
        if (!(cond)) {                                                                                                                                                             \
            printf("[FAIL] %s:%d: %s\n", __FILE__, __LINE__, msg);                                                                                                                 \
            tests_failed++;                                                                                                                                                        \
            return;                                                                                                                                                                \
        }                                                                                                                                                                          \
    } while (0)

#define TEST(name)                                                                                                                                                                 \
    static void name(void);                                                                                                                                                        \
    static void run_##name(void) {                                                                                                                                                 \
        tests_run++;                                                                                                                                                               \
        name();                                                                                                                                                                    \
        tests_passed++;                                                                                                                                                            \
    }                                                                                                                                                                              \
    static void name(void)

static void unit_test_summary(void) {
    printf("Tests:  %d\n\033[32;1mPassed\033[0m: %d\n\033[31;1mFailed\033[0m: %d\n", tests_run, tests_passed, tests_failed);
    if (tests_failed > 0) {
        exit(EXIT_FAILURE);
    }
}

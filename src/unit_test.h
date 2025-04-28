#pragma once
#include <stdio.h>
#include <stdlib.h>

static int tests_run = 0;
static int tests_failed = 0;

#define ASSERT(cond, msg)                                                                                                                                      \
    do {                                                                                                                                                       \
        tests_run++;                                                                                                                                           \
        if (!(cond)) {                                                                                                                                         \
            printf("[FAIL] %s:%d: %s\n", __FILE__, __LINE__, msg);                                                                                             \
            tests_failed++;                                                                                                                                    \
            return;                                                                                                                                            \
        }                                                                                                                                                      \
    } while (0)

#define TEST(name) static void name(void)

static void unit_test_summary(void) {
    printf("Tests run: %d, Failures: %d\n", tests_run, tests_failed);
    if (tests_failed > 0) exit(EXIT_FAILURE);
}

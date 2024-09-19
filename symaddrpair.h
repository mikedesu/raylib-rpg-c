#pragma once

typedef struct symaddrpair_t {
    const char* name;
    void (**addr)();
} symaddrpair_t;

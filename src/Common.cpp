#include "Common.h"

void Common::Error(const char* message) {
    fprintf(stderr, "\033[0;31m %s \033[0m\n", message);
}

jmp_buf* Common::GetEnv() {
    return &env;
}

#pragma once

#include "Headings.h"

class Common {
private:
    static inline jmp_buf env;

public:
    static jmp_buf *GetEnv();

    static void Error(const char* message);
};
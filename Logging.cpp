#include "Logging.h"
#include <stdio.h>
#include <stdarg.h>

Logging::Logging(){
}

Logging::~Logging(){
}

void Logging::log(const char *st,...)
{
    va_list args;
    va_start(args,st);
    vprintf(st,args);
    va_end(args);
}


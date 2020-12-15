#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef enum {
    ok, eof, bad
} input_val;

void        skip_str();
void        help();
input_val   input_int(int32_t* val);
input_val   input_str(char* val);

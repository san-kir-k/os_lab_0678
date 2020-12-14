#pragma once
#include <stdio.h>
#include <stdlib.h>

typedef enum input_val input_val;
enum input_val {
    ok, eof, bad
};

void        skip_str();
void        help();
input_val   input_int(int32_t* val);
input_val   input_str(char* val);


#ifndef JSON_WRITER_HEADER
#define JSON_WRITER_HEADER
#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include <stdio.h>

void writeJSON(FILE * output, Program * program);
#endif

#ifndef JSON_WRITER_HEADER
#define JSON_WRITER_HEADER
#include "../../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include <stdio.h>
#include "../../domain-specific/SimulationState.h"

void writeJSON(FILE * out, SimulationState * state);
#endif
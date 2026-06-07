#ifndef ENGINE_EXECUTOR_HEADER
#define ENGINE_EXECUTOR_HEADER

#include "../../support/type/CompilerState.h"
#include "../domain-specific/SimulationState.h"
#include "../../support/type/CompilationStatus.h"
SimulationState * executeEngine(CompilerState * compilerState);

#endif
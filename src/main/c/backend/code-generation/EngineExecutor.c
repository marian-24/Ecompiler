#include "EngineExecutor.h"
#include "../domain-specific/SimulationEngine.h"
#include "../../support/logging/Logger.h"

static Logger * _logger = NULL;

SimulationState * executeEngine(CompilerState * compilerState) {

    _logger = createLogger("EngineExecutor");
    if (compilerState->succeeded != SUCCEEDED) {
        logError(_logger, "Semantic analysis failed. Simulation aborted.");
        return NULL;
    }

    Program * program = compilerState->abstractSyntaxtTree;

    if (program == NULL) {
        logError(_logger,"AST is empty. Simulation aborted.");
        return NULL;
    }

    compilerState->simulationState = initSimulation(program);

    StatementList * current = program->statements;
    while (current != NULL) {
        Statement * stmt = current->statement;
        if (stmt->type == STATEMENT_SIMULATE) {
            logDebugging(_logger, "Executing simulation for ecosystem '%s'", stmt->simulateStatement->ecosystemName);
            runSimulation(compilerState->simulationState, stmt->simulateStatement);
        }
        current = current->next;
    }

    logDebugging(_logger,"Simulation phase completed.");
    return compilerState->simulationState;
}
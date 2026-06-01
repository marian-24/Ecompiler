#ifndef SIMULATION_ENGINE_HEADER
#define SIMULATION_ENGINE_HEADER

#include "SimulationState.h"
#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../support/type/ModuleDestructor.h"
#include "../../support/logging/Logger.h"

/*initialize modules internal state */
ModuleDestructor initializeSimulationEngineModule();

/*
  Reads the AST and builds the initial SimulationState, Must be called before runSimulation.
*/
SimulationState * initSimulation(Program * ast);

/*
Runs the simulation for the number of generations specified in cmd
Applies reproduction, encounters, environmental changes, and mortality
 */
void runSimulation(SimulationState * state, SimulateStatement * cmd);

/*
  Executes a single statement in the context of the current simulation state
 Used for immediate statements: for each, while, log, if, attribute assignment
 */
void executeStatement(SimulationState * state, Statement * statement);

/*
  Returns the current population of a species in a region.
  Returns -1 if the ecosystem, region or species is not found.
 */
int getPopulation(SimulationState * state, const char * ecosystemName,
                  const char * regionName, const char * speciesName);

/*
Frees all memory owned by the SimulationState.
 */
void destroySimulationState(SimulationState * state);

#endif
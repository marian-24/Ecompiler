#ifndef SIMULATION_STATE_HEADER
#define SIMULATION_STATE_HEADER

#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../support/type/TokenLabel.h"
#include <stdlib.h>
#include <string.h>

/*
  Represents a single living individual of a species inside a region.
  energy is per-individual and mutable 
  age is incremented each generation; death occurs when age >= species.lifespan.
 */
typedef struct Individual {
    char * speciesName;
    double energy;
    int age;
    struct Individual * next;
} Individual;

/*
  Runtime copy of a SpeciesDefinition. 
  Shared across all individuals of this species, do not mutate after setup.
 */
typedef struct RuntimeSpecies {
    char * name;
    int lifespan;
    double reproductionRate;
    int speed;
    double initialEnergy;
    TokenLabel reproductiveStrategy;
    TokenLabel diet;
    TokenLabel habitat;
    EnvTolerance envTolerance;
    struct RuntimeSpecies * next;
} RuntimeSpecies;

/*
  Runtime state of a region. Environmental attributes
  are mutable, on generation and every random blocks can modify them.
 */
typedef struct RuntimeRegion {
    char * name;
    int temperature;
    int humidity;
    int altitude;
    int carryingCapacity;
    TokenLabel habitat;
    Individual * individuals;
    struct RuntimeRegion * next;
} RuntimeRegion;

/*
  Runtime state of an ecosystem.
 Holds the live regions and the species registered in this ecosystem.
 */
typedef struct RuntimeEcosystem {
    char * name;
    RuntimeRegion * regions;
    RuntimeSpecies * species;
    struct RuntimeEcosystem * next;
} RuntimeEcosystem;

/*
  Global simulation state, Owns all runtime structures
  programStatements is used to find
  on encounter / on generation / every random blocks at runtime
 */
typedef struct {
    RuntimeEcosystem * ecosystems;
    int currentGeneration;
    unsigned int randomSeed;
    StatementList * programStatements;  // read-only, owned by the AST
} SimulationState;

#endif
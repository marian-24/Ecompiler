#ifndef SIMULATION_STATE_HEADER
#define SIMULATION_STATE_HEADER

#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../support/type/TokenLabel.h"
#include <stdlib.h>
#include <string.h>

typedef struct Individual {
    char * speciesName;
    double energy;
    int age;
    struct Individual * next;
} Individual;

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

typedef struct RuntimeEcosystem {
    char * name;
    RuntimeRegion * regions;
    RuntimeSpecies * species;
    struct RuntimeEcosystem * next;
} RuntimeEcosystem;

typedef struct PopulationRecord {
    int    generation;
    char * ecosystemName;
    char * regionName;
    char * speciesName;
    int    count;
    struct PopulationRecord * next;
} PopulationRecord;

typedef struct ExtinctionRecord {
    int generation;
    char * ecosystemName;
    char * regionName;
    char * speciesName;
    struct ExtinctionRecord * next;
} ExtinctionRecord;

typedef struct EncounterRecord {
    int generation;
    char * ecosystemName;
    char * regionName;
    char * speciesA;
    char * speciesB;
    struct EncounterRecord * next;
} EncounterRecord;

typedef struct EnvironmentalChangeRecord {
    int generation;
    char * ecosystemName;
    char * regionName;
    char * attribute;
    double oldValue;
    double newValue;
    struct EnvironmentalChangeRecord * next;
} EnvironmentalChangeRecord;

typedef struct {
    RuntimeEcosystem * ecosystems;
    int currentGeneration;
    unsigned int randomSeed;
    StatementList * programStatements;  // read-only, owned by the AST
    PopulationRecord * history;       
    PopulationRecord * historyTail;   // para evitar recorrer toda la lista  

    ExtinctionRecord * extinctions;
    ExtinctionRecord * extinctionsTail;

    EncounterRecord * encounters;
    EncounterRecord * encountersTail;

    EnvironmentalChangeRecord * envChanges;
    EnvironmentalChangeRecord * envChangesTail;
} SimulationState;

#endif
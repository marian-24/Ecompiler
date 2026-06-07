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


typedef enum {
    EXTINCTION_CAUSE_AGE,       
    EXTINCTION_CAUSE_ENERGY,   
    EXTINCTION_CAUSE_HABITAT,   
    EXTINCTION_CAUSE_REMOVED    
} ExtinctionCause;


typedef struct ExtinctionRecord {
    int             generation;
    char *          ecosystemName;
    char *          regionName;
    char *          speciesName;
    ExtinctionCause cause;
    struct ExtinctionRecord * next;
} ExtinctionRecord;

typedef struct EncounterRecord {
    int    generation;
    char * ecosystemName;
    char * regionName;
    char * speciesA;
    double energyABefore;
    double energyAAfter;    
    int    speciesARemoved;
    char * speciesB;
    double energyBBefore;
    double energyBAfter;     
    int    speciesBRemoved;
    struct EncounterRecord * next;
} EncounterRecord;

typedef struct EnvironmentRecord {
    int    generation;
    char * ecosystemName;
    char * regionName;
    int    temperature;
    int    humidity;
    int    altitude;
    struct EnvironmentRecord * next;
} EnvironmentRecord;

typedef struct {
    RuntimeEcosystem  * ecosystems;
    int                 currentGeneration;
    unsigned int        randomSeed;
    StatementList     * programStatements;   

    PopulationRecord  * history;             
    PopulationRecord  * historyTail;             

    ExtinctionRecord  * extinctionHistory;   
    ExtinctionRecord  * extinctionHistoryTail;   

    EncounterRecord   * encounterHistory;    
    EncounterRecord   * encounterHistoryTail;    

    EnvironmentRecord * environmentHistory; 
    EnvironmentRecord * environmentHistoryTail; 

} SimulationState;

#endif
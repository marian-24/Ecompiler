#ifndef HTML_WRITER_H
#define HTML_WRITER_H
#include "../../domain-specific/SimulationState.h"
void writeHTML(FILE * out, SimulationState * state);

typedef struct ExtinctionSummary {
    char * species;
    char * ecosystem;
    char * region;

    int count;
    
    ExtinctionCause cause;

    struct ExtinctionSummary * next;
} ExtinctionSummary;

typedef struct EncounterSummary {
    char * speciesA;
    char * speciesB;

    int count;
    int removalsA;
    int removalsB;

    struct EncounterSummary * next;
} EncounterSummary;


#endif
#include "JsonWriter.h"
#include <stdio.h>
#include <string.h>

/* PRIVATE */
static void _writePopulationHistory(FILE * out, PopulationRecord * history);
static void _writeEcosystems(FILE * out, RuntimeEcosystem * eco);
static void _writeExtinctionHistory(FILE * out,ExtinctionRecord * rec);
static void _writeEncounterHistory(FILE * out, EncounterRecord * rec);
static const char * _extinctionCauseToString(ExtinctionCause cause);


static void _writePopulationHistory(FILE * out, PopulationRecord * history) {
    while(history != NULL) {

        fprintf(out,
            "    {\n"
            "      \"generation\": %d,\n"
            "      \"ecosystem\": \"%s\",\n"
            "      \"region\": \"%s\",\n"
            "      \"species\": \"%s\",\n"
            "      \"count\": %d\n"
            "    }%s\n",

            history->generation,
            history->ecosystemName,
            history->regionName,
            history->speciesName,
            history->count,

            history->next == NULL ? "" : ","
        );
        history = history->next;
    }
}

static void _writeEcosystems(FILE * out, RuntimeEcosystem * eco) {
    while (eco != NULL) {

        fprintf(out,
            "    {\n"
            "      \"name\": \"%s\",\n",
            eco->name
        );

        fprintf(out,
            "      \"regions\": ["
        );

        RuntimeRegion * r = eco->regions;
        while (r) {
            fprintf(out, "\"%s\"%s", r->name, r->next ? ", " : "");
            r = r->next;
        }

        fprintf(out,
            "],\n"
            "      \"species\": ["
        );

        RuntimeSpecies * s = eco->species;
        while (s) {
            fprintf(out,"\"%s\"%s", s->name, s->next ? ", " : "");
            s = s->next;
        }

        fprintf(out,
            "]\n"
            "    }%s\n",
            eco->next ? "," : ""
        );
        eco = eco->next;
    }
}

static void _writeEncounterHistory(FILE * out, EncounterRecord * rec) {
    while (rec != NULL) {
        fprintf(out,
            "    {\n"
            "      \"generation\": %d,\n"
            "      \"ecosystem\": \"%s\",\n"
            "      \"region\": \"%s\",\n"

            "      \"speciesA\": {\n"
            "        \"name\": \"%s\",\n"
            "        \"energyBefore\": %.2f,\n"
            "        \"energyAfter\": %.2f,\n"
            "        \"removed\": %s\n"
            "      },\n"

            "      \"speciesB\": {\n"
            "        \"name\": \"%s\",\n"
            "        \"energyBefore\": %.2f,\n"
            "        \"energyAfter\": %.2f,\n"
            "        \"removed\": %s\n"
            "      }\n"

            "    }%s\n",

            rec->generation,
            rec->ecosystemName,
            rec->regionName,

            rec->speciesA,
            rec->energyABefore,
            rec->energyAAfter,
            rec->speciesARemoved ? "true" : "false",

            rec->speciesB,
            rec->energyBBefore,
            rec->energyBAfter,
            rec->speciesBRemoved ? "true" : "false",

            rec->next ? "," : ""
        );

        rec = rec->next;
    }
}

static const char * _extinctionCauseToString(ExtinctionCause cause) {
    switch (cause) {
        case EXTINCTION_CAUSE_AGE: return "AGE";
        case EXTINCTION_CAUSE_ENERGY: return "ENERGY";
        case EXTINCTION_CAUSE_HABITAT: return "HABITAT";
        case EXTINCTION_CAUSE_REMOVED: return "REMOVED";
        default: return "UNKNOWN";
    }
}

static void _writeExtinctionHistory(FILE * out,ExtinctionRecord * rec) {
    while (rec != NULL) {

        fprintf(out,
            "    {\n"
            "      \"generation\": %d,\n"
            "      \"ecosystem\": \"%s\",\n"
            "      \"region\": \"%s\",\n"
            "      \"species\": \"%s\",\n"
            "      \"cause\": \"%s\"\n"
            "    }%s\n",

            rec->generation,
            rec->ecosystemName,
            rec->regionName,
            rec->speciesName,
            _extinctionCauseToString(rec->cause),

            rec->next ? "," : ""
        );

        rec = rec->next;
    }
}

static void _writeEnvironmentHistory(FILE * out, EnvironmentRecord * rec) {
    while (rec != NULL) {

        fprintf(out,
            "    {\n"
            "      \"generation\": %d,\n"
            "      \"ecosystem\": \"%s\",\n"
            "      \"region\": \"%s\",\n"

            "      \"environment\": {\n"
            "        \"temperature\": %d,\n"
            "        \"humidity\": %d,\n"
            "        \"altitude\": %d\n"
            "      }\n"

            "    }%s\n",

            rec->generation,
            rec->ecosystemName,
            rec->regionName,

            rec->temperature,
            rec->humidity,
            rec->altitude,

            rec->next ? "," : ""
        );

        rec = rec->next;
    }
}


/* PUBLIC */
void writeJSON(FILE * out, SimulationState * state) {

    fprintf(out, "{\n");

    fprintf(out,
        "  \"generationCount\": %d,\n",
        state->currentGeneration
    );

    /* Ecosistemas */
    fprintf(out,
        "  \"ecosystems\": [\n"
    );
    _writeEcosystems(
        out,
        state->ecosystems
    );
    fprintf(out,
        "  ],\n"
    );

    /* Evolución de poblaciones */
    fprintf(out,
        "  \"populationHistory\": [\n"
    );
    _writePopulationHistory(
        out,
        state->history
    );
    fprintf(out,
        "  ],\n"
    );

    /* Evolución de ambientes */
    fprintf(out,
        "  \"environmentHistory\": [\n"
    );
    _writeEnvironmentHistory(
        out,
        state->environmentHistory
    );
    fprintf(out,
        "  ],\n"
    );

    /* Encuentros */
    fprintf(out,
        "  \"encounters\": [\n"
    );
    _writeEncounterHistory(
        out,
        state->encounterHistory
    );
    fprintf(out,
        "  ],\n"
    );

    /* Extinciones */
    fprintf(out,
        "  \"extinctions\": [\n"
    );
    _writeExtinctionHistory(
        out,
        state->extinctionHistory
    );
    fprintf(out,
        "  ]\n"
    );

    fprintf(out, "}\n");
}
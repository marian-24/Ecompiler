#include "JsonWriter.h"
#include <stdio.h>
#include <string.h>

/* PRIVATE */
static void _writeSpecies(FILE * out, SpeciesDefinition * species, int last);
static void _writeSpeciesAttributes(FILE * out, SpeciesAttributeList * list);
static void _writeRegion(FILE * out, RegionDefinition * region, int last);
static void _writeEvents(FILE * out, StatementList * statements);
static void _writeEvent(FILE * out, Statement * statement, int last);
static const char * _tokenLabelToString(TokenLabel label);

static void _writeSpecies(FILE * out, SpeciesDefinition * species, int last){
    fprintf(out, "    {\n      \"name\": \"%s\",\n      ", species->name);
    _writeSpeciesAttributes(out, species->attributes);
    fprintf(out, "\n    }%s\n", last ? "" : ",");
}

static void _writeSpeciesAttributes(FILE * out, SpeciesAttributeList * list) {
    while (list != NULL) {
        SpeciesAttribute * attr = list->attribute;
        switch (attr->type) {
            case ATTR_LIFESPAN:
                fprintf(out, "\"lifespan\": %d", attr->lifespanValue);
                break;
            case ATTR_REPRODUCTION_RATE:
                fprintf(out, "\"reproductionRate\": %.2f", attr->reproductionRateValue);
                break;
            case ATTR_SPEED:
                fprintf(out, "\"speed\": %d", attr->speedValue);
                break;
            case ATTR_DIET:
                fprintf(out, "\"diet\": \"%s\"", _tokenLabelToString(attr->dietValue));
                break;
            case ATTR_HABITAT:
                fprintf(out, "\"habitat\": \"%s\"", _tokenLabelToString(attr->habitatValue));
                break;
            case ATTR_REPRODUCTIVE_STRATEGY:
                fprintf(out, "\"reproductionStrategy\": \"%s\"", _tokenLabelToString(attr->reproductiveStrategy));
                break;
            case ATTR_ENERGY:
                fprintf(out, "\"energy\": %.2f", attr->energyValue);
                break;
            case ATTR_ENVIRONMENTAL_TOLERANCE:
                fprintf(out, "\"environmentalTolerance\": { "
                    "\"temperature\": [%d, %d], "
                    "\"humidity\": [%d, %d], "
                    "\"altitude\": [%d, %d] }",
                    attr->envTolerance.temperature.min,
                    attr->envTolerance.temperature.max,
                    attr->envTolerance.humidity.min,
                    attr->envTolerance.humidity.max,
                    attr->envTolerance.altitude.min,
                    attr->envTolerance.altitude.max);
                break;
        }
        if (list->next != NULL) fprintf(out, ",\n      ");
        list = list->next;
    }
}

static void _writeRegion(FILE * out, RegionDefinition * region, int last) {
    fprintf(out, "    {\n");
    fprintf(out, "      \"name\": \"%s\",\n", region->name);
    fprintf(out, "      \"temperature\": %d,\n", region->temperature);
    fprintf(out, "      \"humidity\": %d,\n", region->humidity);
    fprintf(out, "      \"altitude\": %d,\n", region->altitude);
    fprintf(out, "      \"carryingCapacity\": %d,\n", region->carryingCapacity);
    fprintf(out, "      \"habitat\": \"%s\"\n", _tokenLabelToString(region->habitat));
    fprintf(out, "    }%s\n", last ? "" : ",");
}

static void _writeEvent(FILE * out, Statement * stmt, int last) {
    switch (stmt->type) {
        case STATEMENT_ADD:
            fprintf(out, "    { \"type\": \"ADD\", \"amount\": %d, \"species\": \"%s\", \"ecosystem\": \"%s\", \"region\": \"%s\" }",
                stmt->addStatement->amount,
                stmt->addStatement->speciesName,
                stmt->addStatement->ecosystemName,
                stmt->addStatement->regionName);
            break;
        case STATEMENT_REMOVE:
            fprintf(out, "    { \"type\": \"REMOVE\", \"species\": \"%s\" }",
                stmt->removeStatement->speciesName);
            break;
        case STATEMENT_MOVE:
            fprintf(out, "    { \"type\": \"MOVE\", \"species\": \"%s\", \"ecosystem\": \"%s\", \"region\": \"%s\" }",
                stmt->moveStatement->speciesName,
                stmt->moveStatement->ecosystemName,
                stmt->moveStatement->regionName);
            break;
        case STATEMENT_ON_ENCOUNTER:
            fprintf(out, "    { \"type\": \"ON_ENCOUNTER\", \"speciesA\": \"%s\", \"speciesB\": \"%s\", \"ecosystem\": \"%s\", \"region\": \"%s\" }",
                stmt->onEncounterBlock->speciesA,
                stmt->onEncounterBlock->speciesB,
                stmt->onEncounterBlock->ecosystemName,
                stmt->onEncounterBlock->regionName);
            break;
        case STATEMENT_ON_GENERATION:
            fprintf(out, "    { \"type\": \"ON_GENERATION\", \"generation\": %d, \"ecosystem\": \"%s\", \"region\": \"%s\" }",
                stmt->onGenerationBlock->generationNumber,
                stmt->onGenerationBlock->ecosystemName,
                stmt->onGenerationBlock->regionName);
            break;
        case STATEMENT_SIMULATE:
            fprintf(out, "    { \"type\": \"SIMULATE\", \"ecosystem\": \"%s\", \"generations\": %d, \"hasSeed\": %s, \"seed\": %d }",
                stmt->simulateStatement->ecosystemName,
                stmt->simulateStatement->generations,
                stmt->simulateStatement->hasSeed ? "true" : "false",
                stmt->simulateStatement->seedValue);
            break;
        case STATEMENT_LOG:
            switch (stmt->logStatement->type) {
                case LOG_POPULATION:
                    fprintf(out, "    { \"type\": \"LOG_POPULATION\", \"species\": \"%s\", \"ecosystem\": \"%s\", \"region\": \"%s\" }",
                        stmt->logStatement->speciesName,
                        stmt->logStatement->ecosystemName,
                        stmt->logStatement->regionName);
                    break;
                case LOG_STATE:
                    fprintf(out, "    { \"type\": \"LOG_STATE\", \"ecosystem\": \"%s\" }",
                        stmt->logStatement->ecosystemName);
                    break;
                case LOG_GENERATION:
                    fprintf(out, "    { \"type\": \"LOG_GENERATION\" }");
                    break;
            }
            break;
        default:
            break;
    }
    fprintf(out, "%s\n", last ? "" : ",");
}

static void _writeEvents(FILE * out, StatementList * list) {
    /* primero contamos cuántos eventos hay para saber cuál es el último */
    int count = 0;
    StatementList * aux = list;
    while (aux != NULL) { count++; aux = aux->next; }

    int i = 0;
    while (list != NULL) {
        _writeEvent(out, list->statement, i == count - 1);
        i++;
        list = list->next;
    }
}


static const char * _tokenLabelToString(TokenLabel label) {
    switch (label) {
        case HERBIVORE:    return "HERBIVORE";
        case CARNIVORE:    return "CARNIVORE";
        case OMNIVORE:     return "OMNIVORE";
        case DECOMPOSER:   return "DECOMPOSER";
        case TERRESTRIAL:  return "TERRESTRIAL";
        case AQUATIC:      return "AQUATIC";
        case AMPHIBIOUS:   return "AMPHIBIOUS";
        case MIXED:        return "MIXED";
        case R_SELECTED:   return "R_SELECTED";
        case K_SELECTED:   return "K_SELECTED";
        case R_STRATEGY:   return "R_STRATEGY";
        case K_STRATEGY:   return "K_STRATEGY";
        default:           return "UNKNOWN";
    }
}


static void printStatementsAndCount(StatementList * list, int * countSpecies, int * countRegions, char ** ecosystemName, int * generations, int * hasSeed, int * seed) {
    while (list != NULL) {

        if (list->statement->type == STATEMENT_SPECIES) (*countSpecies)++;
        else if (list->statement->type == STATEMENT_REGION) (*countRegions)++;

        else if (list->statement->type == STATEMENT_SIMULATE) {
            ecosystemName = list->statement->simulateStatement->ecosystemName;
            generations   = list->statement->simulateStatement->generations;
            hasSeed       = list->statement->simulateStatement->hasSeed;
            seed          = list->statement->simulateStatement->seedValue;
        }
        list = list->next;
    }

}

/* PUBLIC */
void writeJson(FILE * out, Program * program) {
    /* buscamos el simulate para obtener ecosistema y generaciones */
    char * ecosystemName = "unknown";
    int generations = 0, hasSeed = 0, seed = 0;

    StatementList * aux = program->statements;
    int countSpecies = 0, countRegions = 0;

    printStatementsAndCount(aux, &countSpecies, &countRegions, &ecosystemName, &generations, &hasSeed, &seed);
    fprintf(out, "{\n  \"simulation\": {\n");
    fprintf(out, "    \"ecosystem\": \"%s\",\n", ecosystemName);
    fprintf(out, "    \"generations\": %d,\n", generations);
    if (hasSeed) fprintf(out, "    \"seed\": %d,\n", seed);

    /* species */
    fprintf(out, "    \"species\": [\n");
    StatementList * list = program->statements;
    int si = 0;
    while (list != NULL) {
        if (list->statement->type == STATEMENT_SPECIES) {
            _writeSpecies(out, list->statement->speciesDefinition, si == countSpecies - 1);
            si++;
        }
        list = list->next;
    }
    fprintf(out, "    ],\n");

    /* regions */
    fprintf(out, "    \"regions\": [\n");
    int ri = 0;
    list = program->statements;
    while (list != NULL) {
        if (list->statement->type == STATEMENT_REGION) {
            _writeRegion(out, list->statement->regionDefinition, ri == countRegions - 1);
            ri++;
        }
        list = list->next;
    }
    fprintf(out, "    ],\n");

    /* events */
    fprintf(out, "    \"events\": [\n");
    _writeEvents(out, program->statements);
    fprintf(out, "    ]\n");

    fprintf(out, "  }\n}\n");
}
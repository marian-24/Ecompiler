#include "SimulationEngine.h"
#include "../../frontend/syntactic-analysis/BisonParser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


static Logger * _logger = NULL;


typedef struct {
    const char *     speciesName; /* especie a la que pertenece el individuo  */
    Individual *     individual;  /* puntero al individuo en proceso           */
    int              removed;     /* 1 si el individuo fue removido/liberado   */
    RuntimeRegion *  region;      /* región en la que vive el individuo        */
    RuntimeEcosystem * ecosystem; /* ecosistema contenedor                     */
} ExecCtx;

static ExecCtx _ctxA = { NULL, NULL, 0, NULL, NULL };
static ExecCtx _ctxB = { NULL, NULL, 0, NULL, NULL };


/* lookup */
static RuntimeEcosystem * _findEcosystem(SimulationState *, const char *);
static RuntimeRegion *    _findRegion(RuntimeEcosystem *, const char *);
static RuntimeSpecies *   _findSpecies(SimulationState *, const char *);

/* conteo y selección de individuos */
static int          _countInRegion(RuntimeRegion *, const char *);
static int          _totalInRegion(RuntimeRegion *);
static Individual * _pickRandom(RuntimeRegion *, const char *);

/* gestión de individuos */
static void _addToRegion(RuntimeRegion *, const char *, int, double);
static void _removeIndividual(RuntimeRegion *, Individual *);

/* compatibilidad de hábitat */
static int _habitatCompatible(TokenLabel speciesHabitat, TokenLabel regionHabitat);

/* evaluación de expresiones y condiciones */
static double _evalExpr(SimulationState *, Expression *);
static int    _evalCond(SimulationState *, Condition *);
static void   _applyOp(double *, AssignmentOperatorType, double);

/* ejecución de statements */
static void _execList(SimulationState *, StatementList *);
static void _execStmt(SimulationState *, Statement *);

/* pasos de simulación por generación */
static void _applyOnGeneration(SimulationState *, RuntimeEcosystem *, RuntimeRegion *);
static void _applyEveryRandom(SimulationState *, RuntimeEcosystem *, RuntimeRegion *);
static void _applyEncounters(SimulationState *, RuntimeEcosystem *, RuntimeRegion *);
static void _applyReproduction(SimulationState *, RuntimeEcosystem *, RuntimeRegion *);
static void _applyMortality(SimulationState *, RuntimeEcosystem *, RuntimeRegion *);

/* constructores de runtime desde AST */
static RuntimeSpecies * _buildSpecies(SpeciesDefinition *);
static RuntimeRegion *  _buildRegion(RegionDefinition *);


static void _shutdownSimulationEngineModule(void) {
    if (_logger != NULL) {
        logDebugging(_logger, "Destroying module: SimulationEngine...");
        destroyLogger(_logger);
        _logger = NULL;
    }
}

ModuleDestructor initializeSimulationEngineModule(void) {
    _logger = createLogger("SimulationEngine");
    return _shutdownSimulationEngineModule;
}


static RuntimeEcosystem * _findEcosystem(SimulationState * state, const char * name) {
    RuntimeEcosystem * eco = state->ecosystems;
    while (eco) {
        if (strcmp(eco->name, name) == 0) return eco;
        eco = eco->next;
    }
    return NULL;
}

static RuntimeRegion * _findRegion(RuntimeEcosystem * eco, const char * name) {
    RuntimeRegion * r = eco->regions;
    while (r) {
        if (strcmp(r->name, name) == 0) return r;
        r = r->next;
    }
    return NULL;
}


static RuntimeSpecies * _findSpecies(SimulationState * state, const char * name) {
    RuntimeEcosystem * eco = state->ecosystems;
    while (eco) {
        RuntimeSpecies * sp = eco->species;
        while (sp) {
            if (strcmp(sp->name, name) == 0) return sp;
            sp = sp->next;
        }
        eco = eco->next;
    }
    return NULL;
}


static int _countInRegion(RuntimeRegion * region, const char * speciesName) {
    int n = 0;
    for (Individual * i = region->individuals; i; i = i->next)
        if (strcmp(i->speciesName, speciesName) == 0) n++;
    return n;
}

static int _totalInRegion(RuntimeRegion * region) {
    int n = 0;
    for (Individual * i = region->individuals; i; i = i->next) n++;
    return n;
}


static Individual * _pickRandom(RuntimeRegion * region, const char * speciesName) {
    int count = _countInRegion(region, speciesName);
    if (count == 0) return NULL;
    int target = rand() % count;
    int i = 0;
    for (Individual * ind = region->individuals; ind; ind = ind->next) {
        if (strcmp(ind->speciesName, speciesName) == 0) {
            if (i == target) return ind;
            i++;
        }
    }
    return NULL; /* inalcanzable */
}


static void _addToRegion(RuntimeRegion * region, const char * speciesName,
                          int count, double initialEnergy) {
    for (int i = 0; i < count; i++) {
        Individual * ind   = calloc(1, sizeof(Individual));
        ind->speciesName   = strdup(speciesName);
        ind->energy        = initialEnergy;
        ind->age           = 0;
        ind->next          = region->individuals;
        region->individuals = ind;
    }
}


static void _removeIndividual(RuntimeRegion * region, Individual * target) {
    Individual * prev = NULL;
    Individual * cur  = region->individuals;
    while (cur) {
        if (cur == target) {
            if (prev) prev->next  = cur->next;
            else       region->individuals = cur->next;
            free(cur->speciesName);
            free(cur);
            return;
        }
        prev = cur;
        cur  = cur->next;
    }
}


static int _habitatCompatible(TokenLabel speciesHabitat, TokenLabel regionHabitat) {
    if (speciesHabitat == AMPHIBIOUS) return 1;
    if (regionHabitat  == MIXED)      return 1;
    return speciesHabitat == regionHabitat;
}


static void _applyOp(double * target, AssignmentOperatorType op, double value) {
    switch (op) {
        case ASSIGN_SIMPLE: *target  = value; break;
        case ASSIGN_ADD:    *target += value; break;
        case ASSIGN_SUB:    *target -= value; break;
        case ASSIGN_MUL:    *target *= value; break;
        case ASSIGN_DIV:
            if (value != 0.0) *target /= value;
            else logError(_logger, "Division por cero en asignación de atributo.");
            break;
    }
}


static double _evalExpr(SimulationState * state, Expression * expr) {
    if (!expr) return 0.0;

    switch (expr->type) {
        case EXPR_INTEGER:  return (double) expr->intValue;
        case EXPR_FLOAT:    return expr->floatValue;
        case EXPR_BOOLEAN:  return (double) expr->boolValue;
        case EXPR_STRING:   return 0.0; /* no numérico; sólo válido en log */
        case EXPR_IDENTIFIER:
            /* variables locales no implementadas en esta versión */
            logWarning(_logger, "Identificador '%s' sin contexto de variable local.", expr->identifier);
            return 0.0;

        case EXPR_ATTRIBUTE_ACCESS: {
            const char * obj  = expr->attributeAccess.objectName;
            const char * attr = expr->attributeAccess.attributeName;

            /* energy es por-individuo: buscar en contexto activo */
            if (strcmp(attr, "energy") == 0) {
                if (_ctxA.speciesName && strcmp(_ctxA.speciesName, obj) == 0 && _ctxA.individual && !_ctxA.removed)
                    return _ctxA.individual->energy;
                if (_ctxB.speciesName && strcmp(_ctxB.speciesName, obj) == 0 && _ctxB.individual && !_ctxB.removed)
                    return _ctxB.individual->energy;
                logWarning(_logger, "Acceso a '%s.energy' sin individuo en contexto.", obj);
                return 0.0;
            }

            /* atributos de especie */
            RuntimeSpecies * sp = _findSpecies(state, obj);
            if (!sp) {
                logWarning(_logger, "Acceso a atributo de objeto desconocido '%s'.", obj);
                return 0.0;
            }
            if (strcmp(attr, "speed")            == 0) return (double) sp->speed;
            if (strcmp(attr, "lifespan")         == 0) return (double) sp->lifespan;
            if (strcmp(attr, "reproductionRate") == 0) return sp->reproductionRate;

            logWarning(_logger, "Atributo desconocido '%s' en '%s'.", attr, obj);
            return 0.0;
        }

        case EXPR_ECOSYSTEM_ACCESS:
            /* acceso a región como valor no numérico (e.g. Patagonia.Bosque) */
            return 0.0;

        case EXPR_POPULATION_OF: {
            RuntimeEcosystem * eco = _findEcosystem(state, expr->populationOf.ecosystemName);
            if (!eco) return 0.0;
            RuntimeRegion * region = _findRegion(eco, expr->populationOf.regionName);
            if (!region) return 0.0;
            return (double) _countInRegion(region, expr->populationOf.speciesName);
        }

        case EXPR_RANDOM_RANGE: {
            int range = expr->randomRange.max - expr->randomRange.min + 1;
            if (range <= 0) return (double) expr->randomRange.min;
            return (double)(expr->randomRange.min + rand() % range);
        }

        case EXPR_ADD: return _evalExpr(state, expr->binary.left) + _evalExpr(state, expr->binary.right);
        case EXPR_SUB: return _evalExpr(state, expr->binary.left) - _evalExpr(state, expr->binary.right);
        case EXPR_MUL: return _evalExpr(state, expr->binary.left) * _evalExpr(state, expr->binary.right);
        case EXPR_DIV: {
            double r = _evalExpr(state, expr->binary.right);
            if (r == 0.0) { logError(_logger, "División por cero en expresión."); return 0.0; }
            return _evalExpr(state, expr->binary.left) / r;
        }
        default:
            return 0.0;
    }
}

static int _evalCond(SimulationState * state, Condition * cond) {
    if (!cond) return 0;
    switch (cond->type) {
        case COND_LT:  return _evalExpr(state, cond->binary.left) <  _evalExpr(state, cond->binary.right);
        case COND_GT:  return _evalExpr(state, cond->binary.left) >  _evalExpr(state, cond->binary.right);
        case COND_EQ:  return _evalExpr(state, cond->binary.left) == _evalExpr(state, cond->binary.right);
        case COND_NEQ: return _evalExpr(state, cond->binary.left) != _evalExpr(state, cond->binary.right);
        case COND_LTE: return _evalExpr(state, cond->binary.left) <= _evalExpr(state, cond->binary.right);
        case COND_GTE: return _evalExpr(state, cond->binary.left) >= _evalExpr(state, cond->binary.right);
        case COND_AND: return _evalCond(state, cond->logical.left) && _evalCond(state, cond->logical.right);
        case COND_OR:  return _evalCond(state, cond->logical.left) || _evalCond(state, cond->logical.right);
        case COND_NOT: return !_evalCond(state, cond->operand);
        case COND_IN: {
            RuntimeEcosystem * eco = _findEcosystem(state, cond->inOperator.ecosystemName);
            if (!eco) return 0;
            RuntimeRegion * region = _findRegion(eco, cond->inOperator.regionName);
            if (!region) return 0;
            return _countInRegion(region, cond->inOperator.speciesName) > 0;
        }
        default:
            return 0;
    }
}


static void _execList(SimulationState * state, StatementList * list) {
    while (list) {
        _execStmt(state, list->statement);
        list = list->next;
    }
}

static void _execStmt(SimulationState * state, Statement * stmt) {
    if (!stmt) return;

    switch (stmt->type) {

        case STATEMENT_SPECIES:
        case STATEMENT_REGION:
        case STATEMENT_ECOSYSTEM:
            break;

        case STATEMENT_ON_ENCOUNTER:
        case STATEMENT_ON_GENERATION:
        case STATEMENT_EVERY_RANDOM:
            break;

        case STATEMENT_SIMULATE:
            break;

        case STATEMENT_BEHAVIOR:
            break;

        case STATEMENT_ADD: {
            AddStatement * add = stmt->addStatement;
            RuntimeEcosystem * eco = _findEcosystem(state, add->ecosystemName);
            if (!eco) {
                logError(_logger, "add: ecosistema '%s' no encontrado.", add->ecosystemName);
                break;
            }
            RuntimeRegion * region = _findRegion(eco, add->regionName);
            if (!region) {
                logError(_logger, "add: región '%s' no encontrada en '%s'.", add->regionName, add->ecosystemName);
                break;
            }
            /* Respetar carryingCapacity */
            int spare = region->carryingCapacity - _totalInRegion(region);
            int toAdd = (add->amount < spare) ? add->amount : spare;
            if (toAdd <= 0) {
                logWarning(_logger, "add: región '%s' llena, no se agregan individuos.", region->name);
                break;
            }
            if (toAdd < add->amount) {
                logWarning(_logger, "add: capacidad reducida, agregando %d (pedidos: %d).", toAdd, add->amount);
            }
            RuntimeSpecies * sp = _findSpecies(state, add->speciesName);
            double energy = sp ? sp->initialEnergy : 100.0;
            _addToRegion(region, add->speciesName, toAdd, energy);
            logDebugging(_logger, "add: %d %s → %s.%s", toAdd, add->speciesName, add->ecosystemName, add->regionName);
            break;
        }

        case STATEMENT_REMOVE: {
            
            RemoveStatement * rem = stmt->removeStatement;
            if (_ctxA.speciesName && strcmp(_ctxA.speciesName, rem->speciesName) == 0
                && _ctxA.individual && !_ctxA.removed) {
                _removeIndividual(_ctxA.region, _ctxA.individual);
                _ctxA.individual = NULL;
                _ctxA.removed    = 1;
            } else if (_ctxB.speciesName && strcmp(_ctxB.speciesName, rem->speciesName) == 0
                       && _ctxB.individual && !_ctxB.removed) {
                _removeIndividual(_ctxB.region, _ctxB.individual);
                _ctxB.individual = NULL;
                _ctxB.removed    = 1;
            } else {
                logWarning(_logger, "remove '%s': no hay individuo en contexto.", rem->speciesName);
            }
            break;
        }

        case STATEMENT_MOVE: {
          
            MoveStatement * mv = stmt->moveStatement;
            RuntimeEcosystem * dstEco = _findEcosystem(state, mv->ecosystemName);
            if (!dstEco) {
                logError(_logger, "move: ecosistema destino '%s' no encontrado.", mv->ecosystemName);
                break;
            }
            RuntimeRegion * dst = _findRegion(dstEco, mv->regionName);
            if (!dst) {
                logError(_logger, "move: región destino '%s' no encontrada.", mv->regionName);
                break;
            }

            /* Identificar qué individuo mover */
            Individual *    ind = NULL;
            RuntimeRegion * src = NULL;
            if (_ctxA.speciesName && strcmp(_ctxA.speciesName, mv->speciesName) == 0 && !_ctxA.removed) {
                ind = _ctxA.individual; src = _ctxA.region;
            } else if (_ctxB.speciesName && strcmp(_ctxB.speciesName, mv->speciesName) == 0 && !_ctxB.removed) {
                ind = _ctxB.individual; src = _ctxB.region;
            }

            if (!ind || !src) {
                logWarning(_logger, "move '%s': sin individuo en contexto.", mv->speciesName);
                break;
            }

            /* Desconectar del origen */
            Individual * prev = NULL, * cur = src->individuals;
            while (cur && cur != ind) { prev = cur; cur = cur->next; }
            if (!cur) { logWarning(_logger, "move: individuo no hallado en la región origen."); break; }
            if (prev) prev->next        = cur->next;
            else       src->individuals = cur->next;

            /* Conectar al destino si hay capacidad */
            if (_totalInRegion(dst) < dst->carryingCapacity) {
                ind->next        = dst->individuals;
                dst->individuals = ind;
                /* Actualizar el contexto para que apunte a la nueva región */
                if (_ctxA.individual == ind) _ctxA.region = dst;
                if (_ctxB.individual == ind) _ctxB.region = dst;
                logDebugging(_logger, "move: %s → %s.%s", mv->speciesName, mv->ecosystemName, mv->regionName);
            } else {
                /* Destino lleno: el individuo se pierde */
                free(ind->speciesName);
                free(ind);
                if (_ctxA.individual == ind) { _ctxA.individual = NULL; _ctxA.removed = 1; }
                if (_ctxB.individual == ind) { _ctxB.individual = NULL; _ctxB.removed = 1; }
                logWarning(_logger, "move: destino '%s' lleno, individuo perdido.", dst->name);
            }
            break;
        }

        case STATEMENT_ATTRIBUTE_ASSIGNMENT: {
            AttributeAssignment * aa = stmt->attributeAssignment;
            double val = _evalExpr(state, aa->value);
            const char * obj  = aa->objectName;
            const char * attr = aa->attributeName;

            
            if (strlen(obj) == 0) {
                RuntimeRegion * region = _ctxA.region
                                       ? _ctxA.region
                                       : (_ctxB.region ? _ctxB.region : NULL);
                if (!region) {
                    logWarning(_logger, "Asignación de '%s': sin región en contexto.", attr);
                    break;
                }
                if (strcmp(attr, "temperature") == 0) {
                    double t = region->temperature; _applyOp(&t, aa->op, val); region->temperature = (int) t;
                } else if (strcmp(attr, "humidity") == 0) {
                    double h = region->humidity;    _applyOp(&h, aa->op, val); region->humidity    = (int) h;
                } else {
                    logWarning(_logger, "Atributo de región desconocido: '%s'.", attr);
                }
                break;
            }

            /*
             * energy → por-individuo: modificar el individuo en contexto.
             */
            if (strcmp(attr, "energy") == 0) {
                if (_ctxA.speciesName && strcmp(_ctxA.speciesName, obj) == 0 && _ctxA.individual && !_ctxA.removed) {
                    _applyOp(&_ctxA.individual->energy, aa->op, val);
                } else if (_ctxB.speciesName && strcmp(_ctxB.speciesName, obj) == 0 && _ctxB.individual && !_ctxB.removed) {
                    _applyOp(&_ctxB.individual->energy, aa->op, val);
                } else {
                    logWarning(_logger, "Asignación de energy a '%s': sin individuo en contexto.", obj);
                }
                break;
            }

           
            RuntimeSpecies * sp = _findSpecies(state, obj);
            if (sp) {
                if (strcmp(attr, "lifespan") == 0) {
                    double ls = sp->lifespan; _applyOp(&ls, aa->op, val); sp->lifespan = (int) ls;
                } else if (strcmp(attr, "speed") == 0) {
                    double s = sp->speed; _applyOp(&s, aa->op, val); sp->speed = (int) s;
                } else if (strcmp(attr, "reproductionRate") == 0) {
                    _applyOp(&sp->reproductionRate, aa->op, val);
                } else {
                    logWarning(_logger, "Atributo desconocido '%s' en especie '%s'.", attr, obj);
                }
                break;
            }

            logWarning(_logger, "Asignación: objeto '%s' no es especie ni región conocida.", obj);
            break;
        }

        case STATEMENT_IF: {
            IfStatement * ifs = stmt->ifStatement;
            if (_evalCond(state, ifs->condition)) _execList(state, ifs->thenBody);
            else if (ifs->elseBody)               _execList(state, ifs->elseBody);
            break;
        }

        case STATEMENT_WHILE: {
            WhileStatement * ws = stmt->whileStatement;
            int guard = 0;
            while (_evalCond(state, ws->condition) && guard < 10000) {
                _execList(state, ws->body);
                guard++;
            }
            if (guard >= 10000)
                logWarning(_logger, "while: límite de 10000 iteraciones alcanzado.");
            break;
        }

        case STATEMENT_FOR_EACH: {
            
            ForEachStatement * fe = stmt->forEachStatement;
            RuntimeEcosystem * eco = _findEcosystem(state, fe->ecosystemName);
            if (!eco) {
                logError(_logger, "for each: ecosistema '%s' no encontrado.", fe->ecosystemName);
                break;
            }
            RuntimeRegion * region = _findRegion(eco, fe->regionName);
            if (!region) {
                logError(_logger, "for each: región '%s' no encontrada.", fe->regionName);
                break;
            }

            /* Apilar contexto actual y configurar el nuevo */
            ExecCtx savedA = _ctxA;
            ExecCtx savedB = _ctxB;
            _ctxB = (ExecCtx){ NULL, NULL, 0, NULL, NULL };

            Individual * ind = region->individuals;
            while (ind) {
                Individual * nxt = ind->next; /* guardar antes de posible remoción */
                if (strcmp(ind->speciesName, fe->speciesName) == 0) {
                    _ctxA = (ExecCtx){ fe->speciesName, ind, 0, region, eco };
                    _execList(state, fe->body);
                }
                ind = nxt;
            }

            /* Restaurar contexto previo */
            _ctxA = savedA;
            _ctxB = savedB;
            break;
        }

        case STATEMENT_LOG: {
            LogStatement * ls = stmt->logStatement;
            switch (ls->type) {

                case LOG_POPULATION: {
                    RuntimeEcosystem * eco = _findEcosystem(state, ls->ecosystemName);
                    int pop = 0;
                    if (eco) {
                        RuntimeRegion * region = _findRegion(eco, ls->regionName);
                        if (region) pop = _countInRegion(region, ls->speciesName);
                    }
                    printf("population of %s in %s.%s = %d\n",
                           ls->speciesName, ls->ecosystemName, ls->regionName, pop);
                    fflush(stdout);
                    break;
                }

                case LOG_STATE: {
                    RuntimeEcosystem * eco = _findEcosystem(state, ls->ecosystemName);
                    printf("=== state of %s (generation %d) ===\n",
                           ls->ecosystemName, state->currentGeneration);
                    if (eco) {
                        for (RuntimeRegion * region = eco->regions; region; region = region->next) {
                            printf("  region %s: temperature=%d, humidity=%d, altitude=%d, capacity=%d\n",
                                   region->name, region->temperature, region->humidity,
                                   region->altitude, region->carryingCapacity);
                            for (RuntimeSpecies * sp = eco->species; sp; sp = sp->next) {
                                int n = _countInRegion(region, sp->name);
                                printf("    %s: %d individual%s\n", sp->name, n, n == 1 ? "" : "s");
                            }
                        }
                    }
                    fflush(stdout);
                    break;
                }

                case LOG_GENERATION:
                    printf("generation = %d\n", state->currentGeneration);
                    fflush(stdout);
                    break;
            }
            break;
        }

        default:
            logWarning(_logger, "executeStatement: tipo de statement desconocido: %d.", stmt->type);
            break;
    }
}

void executeStatement(SimulationState * state, Statement * statement) {
    _execStmt(state, statement);
}

static void _applyOnGeneration(SimulationState * state,
                                RuntimeEcosystem * eco, RuntimeRegion * region) {
    for (StatementList * sl = state->programStatements; sl; sl = sl->next) {
        Statement * s = sl->statement;
        if (s->type != STATEMENT_ON_GENERATION) continue;
        OnGenerationBlock * b = s->onGenerationBlock;
        if (b->generationNumber != state->currentGeneration) continue;
        if (strcmp(b->ecosystemName, eco->name)    != 0) continue;
        if (strcmp(b->regionName,    region->name) != 0) continue;

        ExecCtx savedA = _ctxA, savedB = _ctxB;
        _ctxA = (ExecCtx){ NULL, NULL, 0, region, eco };
        _ctxB = (ExecCtx){ NULL, NULL, 0, NULL,   NULL };
        _execList(state, b->body);
        _ctxA = savedA;
        _ctxB = savedB;
        logDebugging(_logger, "on generation %d disparado en %s.%s",
                     b->generationNumber, eco->name, region->name);
    }
}


static void _applyEveryRandom(SimulationState * state,
                               RuntimeEcosystem * eco, RuntimeRegion * region) {
    for (StatementList * sl = state->programStatements; sl; sl = sl->next) {
        Statement * s = sl->statement;
        if (s->type != STATEMENT_EVERY_RANDOM) continue;
        EveryRandomBlock * b = s->everyRandomBlock;
        if (strcmp(b->ecosystemName, eco->name)    != 0) continue;
        if (strcmp(b->regionName,    region->name) != 0) continue;

        if ((rand() % 2) == 0) { /* 50 % de probabilidad */
            ExecCtx savedA = _ctxA, savedB = _ctxB;
            _ctxA = (ExecCtx){ NULL, NULL, 0, region, eco };
            _ctxB = (ExecCtx){ NULL, NULL, 0, NULL,   NULL };
            _execList(state, b->body);
            _ctxA = savedA;
            _ctxB = savedB;
            logDebugging(_logger, "every-random disparado en %s.%s", eco->name, region->name);
        }
    }
}


static void _applyEncounters(SimulationState * state,
                              RuntimeEcosystem * eco, RuntimeRegion * region) {
    for (StatementList * sl = state->programStatements; sl; sl = sl->next) {
        Statement * s = sl->statement;
        if (s->type != STATEMENT_ON_ENCOUNTER) continue;
        OnEncounterBlock * b = s->onEncounterBlock;
        if (strcmp(b->ecosystemName, eco->name)    != 0) continue;
        if (strcmp(b->regionName,    region->name) != 0) continue;

        /* Iterar sobre cada individuo de speciesA */
        Individual * indA = region->individuals;
        while (indA) {
            Individual * nxt = indA->next; /* guardado antes del posible remove */
            if (strcmp(indA->speciesName, b->speciesA) == 0) {
                Individual * indB = _pickRandom(region, b->speciesB);
                if (indB) {
                    ExecCtx savedA = _ctxA, savedB = _ctxB;
                    _ctxA = (ExecCtx){ b->speciesA, indA, 0, region, eco };
                    _ctxB = (ExecCtx){ b->speciesB, indB, 0, region, eco };
                    _execList(state, b->body);
                    /* Restaurar; indA/indB pueden haber sido eliminados */
                    _ctxA = savedA;
                    _ctxB = savedB;
                }
            }
            indA = nxt;
        }
    }
}


static void _applyReproduction(SimulationState * state,
                                RuntimeEcosystem * eco, RuntimeRegion * region) {
    for (RuntimeSpecies * sp = eco->species; sp; sp = sp->next) {
        int count = _countInRegion(region, sp->name);
        int total = _totalInRegion(region);
        int minPair = (sp->reproductiveStrategy == R_SELECTED ||
                       sp->reproductiveStrategy == R_STRATEGY) ? 1 : 2;

        if (count >= minPair && total < region->carryingCapacity) {
            double r = (double) rand() / ((double) RAND_MAX + 1.0);
            if (r < sp->reproductionRate) {
                _addToRegion(region, sp->name, 1, sp->initialEnergy);
                logDebugging(_logger, "reproducción: nuevo %s nacido en %s.%s",
                             sp->name, eco->name, region->name);
            }
        }
    }
}


static void _applyMortality(SimulationState * state,
                             RuntimeEcosystem * eco, RuntimeRegion * region) {
    Individual * ind  = region->individuals;
    Individual * prev = NULL;

    while (ind) {
        Individual * nxt  = ind->next;
        int          dead = 0;

        ind->age++;

        RuntimeSpecies * sp = _findSpecies(state, ind->speciesName);
        if (sp) {
            if (ind->age    >= sp->lifespan) dead = 1;
            if (ind->energy <= 0.0)          dead = 1;
            if (!dead && !_habitatCompatible(sp->habitat, region->habitat)
                      && (rand() % 2) == 0)  dead = 1;
        }

        if (dead) {
            logDebugging(_logger, "mortalidad: %s murió en %s.%s (age=%d, energy=%.1f)",
                         ind->speciesName, eco->name, region->name, ind->age, ind->energy);
            if (prev) prev->next        = nxt;
            else       region->individuals = nxt;
            free(ind->speciesName);
            free(ind);
        } else {
            prev = ind;
        }
        ind = nxt;
    }
}


static RuntimeSpecies * _buildSpecies(SpeciesDefinition * def) {
    RuntimeSpecies * rs = calloc(1, sizeof(RuntimeSpecies));
    rs->name = strdup(def->name);

    /* Valores por defecto razonables */
    rs->lifespan             = 10;
    rs->reproductionRate     = 0.3;
    rs->speed                = 5;
    rs->initialEnergy        = 100.0;
    rs->reproductiveStrategy = K_SELECTED;
    rs->diet                 = HERBIVORE;
    rs->habitat              = TERRESTRIAL;
    /* envTolerance queda en 0 (calloc) */

    for (SpeciesAttributeList * al = def->attributes; al; al = al->next) {
        SpeciesAttribute * a = al->attribute;
        switch (a->type) {
            case ATTR_LIFESPAN:              rs->lifespan             = a->lifespanValue;          break;
            case ATTR_REPRODUCTION_RATE:     rs->reproductionRate     = a->reproductionRateValue;  break;
            case ATTR_SPEED:                 rs->speed                = a->speedValue;              break;
            case ATTR_ENERGY:               rs->initialEnergy         = a->energyValue;            break;
            case ATTR_REPRODUCTIVE_STRATEGY: rs->reproductiveStrategy = a->reproductiveStrategy;   break;
            case ATTR_DIET:                  rs->diet                 = a->dietValue;               break;
            case ATTR_HABITAT:               rs->habitat              = a->habitatValue;            break;
            case ATTR_ENVIRONMENTAL_TOLERANCE: rs->envTolerance       = a->envTolerance;            break;
        }
    }
    return rs;
}

static RuntimeRegion * _buildRegion(RegionDefinition * def) {
    RuntimeRegion * rr = calloc(1, sizeof(RuntimeRegion));
    rr->name             = strdup(def->name);
    rr->temperature      = def->temperature;
    rr->humidity         = def->humidity;
    rr->altitude         = def->altitude;
    rr->carryingCapacity = def->carryingCapacity;
    rr->habitat          = def->habitat;
    rr->individuals      = NULL;
    return rr;
}


SimulationState * initSimulation(Program * ast) {
    SimulationState * state = calloc(1, sizeof(SimulationState));
    state->currentGeneration = 0;
    state->randomSeed        = (unsigned int) time(NULL);
    state->programStatements = ast->statements; /* referencia de solo-lectura */

    for (StatementList * sl = ast->statements; sl; sl = sl->next) {
        Statement * s = sl->statement;
        if (s->type != STATEMENT_ECOSYSTEM) continue;

        EcosystemDefinition * edef = s->ecosystemDefinition;
        RuntimeEcosystem * eco = calloc(1, sizeof(RuntimeEcosystem));
        eco->name = strdup(edef->name);

        for (EcosystemMemberList * ml = edef->members; ml; ml = ml->next) {
            EcosystemMember * mem = ml->member;
            if (mem->type == MEMBER_REGION) {
                /* Buscar la definición de región en el programa */
                for (StatementList * search = ast->statements; search; search = search->next) {
                    if (search->statement->type == STATEMENT_REGION &&
                        strcmp(search->statement->regionDefinition->name, mem->name) == 0) {
                        RuntimeRegion * rr = _buildRegion(search->statement->regionDefinition);
                        rr->next     = eco->regions;
                        eco->regions = rr;
                        break;
                    }
                }
            } else if (mem->type == MEMBER_SPECIES) {
                /* Buscar la definición de especie en el programa */
                for (StatementList * search = ast->statements; search; search = search->next) {
                    if (search->statement->type == STATEMENT_SPECIES &&
                        strcmp(search->statement->speciesDefinition->name, mem->name) == 0) {
                        RuntimeSpecies * rs = _buildSpecies(search->statement->speciesDefinition);
                        rs->next      = eco->species;
                        eco->species  = rs;
                        break;
                    }
                }
            }
        }

        eco->next         = state->ecosystems;
        state->ecosystems = eco;
    }

    logDebugging(_logger, "SimulationState inicializado.");
    return state;
}


void runSimulation(SimulationState * state, SimulateStatement * cmd) {
    if (cmd->hasSeed) srand((unsigned int) cmd->seedValue);
    else              srand(state->randomSeed);

    RuntimeEcosystem * eco = _findEcosystem(state, cmd->ecosystemName);
    if (!eco) {
        logError(_logger, "simulate: ecosistema '%s' no encontrado.", cmd->ecosystemName);
        return;
    }

    logDebugging(_logger, "simulate: iniciando %d generaciones para '%s'",
                 cmd->generations, cmd->ecosystemName);

    for (int g = 0; g < cmd->generations; g++) {
        state->currentGeneration++;

        for (RuntimeRegion * region = eco->regions; region; region = region->next) {
            _applyOnGeneration(state, eco, region);
            _applyEveryRandom(state,  eco, region);
            _applyEncounters(state,   eco, region);
            _applyReproduction(state, eco, region);
            _applyMortality(state,    eco, region);
        }

        for (RuntimeRegion * region = eco->regions; region; region = region->next) {
            for (RuntimeSpecies * sp = eco->species; sp; sp = sp->next) {
                PopulationRecord * rec = calloc(1, sizeof(PopulationRecord));
                rec->generation    = state->currentGeneration;
                rec->ecosystemName = strdup(eco->name);
                rec->regionName    = strdup(region->name);
                rec->speciesName   = strdup(sp->name);
                rec->count         = _countInRegion(region, sp->name);
                rec->next          = state->history;
                state->history     = rec;
            }
        }
    }

    logDebugging(_logger, "simulate: finalizado (generación = %d).", state->currentGeneration);
}


int getPopulation(SimulationState * state, const char * ecosystemName,
                  const char * regionName, const char * speciesName) {
    RuntimeEcosystem * eco = _findEcosystem(state, ecosystemName);
    if (!eco) return -1;
    RuntimeRegion * region = _findRegion(eco, regionName);
    if (!region) return -1;
    return _countInRegion(region, speciesName);
}


void destroySimulationState(SimulationState * state) {
    if (!state) return;

    RuntimeEcosystem * eco = state->ecosystems;
    while (eco) {
        RuntimeEcosystem * nextEco = eco->next;

        /* Liberar individuos y regiones */
        RuntimeRegion * region = eco->regions;
        while (region) {
            RuntimeRegion * nextReg = region->next;
            Individual * ind = region->individuals;
            while (ind) {
                Individual * nextInd = ind->next;
                free(ind->speciesName);
                free(ind);
                ind = nextInd;
            }
            free(region->name);
            free(region);
            region = nextReg;
        }

        /* Liberar especies */
        RuntimeSpecies * sp = eco->species;
        while (sp) {
            RuntimeSpecies * nextSp = sp->next;
            free(sp->name);
            free(sp);
            sp = nextSp;
        }

        free(eco->name);
        free(eco);
        eco = nextEco;
    }

    /* Liberar historial de snapshots */
    PopulationRecord * rec = state->history;
    while (rec) {
        PopulationRecord * nextRec = rec->next;
        free(rec->ecosystemName);
        free(rec->regionName);
        free(rec->speciesName);
        free(rec);
        rec = nextRec;
    }

    free(state);
}
#include "HtmlWriter.h"
#include "../../../support/logging/Logger.h"
#include "../../../support/type/CompilerState.h"
#include "../../../frontend/syntactic-analysis/BisonParser.h"

static void _writePopulationChart(FILE * out, SimulationState * state);
static void _writeSummaryTable(FILE * out, SimulationState * state);
static void _writeSpeciesSummary(FILE * out, SimulationState * state);
static void _writeEncounterTable(FILE * out, SimulationState * state);
static void _writeExtinctionTable(FILE * out, SimulationState * state);
static void _writeEnvironmentTable(FILE * out, SimulationState * state);

static void _writeSpeciesTraitsTable(FILE * out, SimulationState * state);
static void _writeToleranceTable(FILE * out, SimulationState * state);
static void _writeExtinctionCauseChart(FILE * out, SimulationState * state);
static void _writeCapacityChart(FILE * out, SimulationState * state);
static void _writeEnergyBalanceChart(FILE * out, SimulationState * state);

//Helpers
static int _countExtinctions(ExtinctionRecord * history);
static int _countEncounters(EncounterRecord * history);
static int _countSpecies(RuntimeEcosystem * ecosystems);
static int _countRegions(RuntimeEcosystem * ecosystems);
static int _countEcosystems(RuntimeEcosystem * ecosystems);
static int _lastPopulation(SimulationState * state, const char * ecosystem, const char * species);
static const char * _causeToString(ExtinctionCause cause);
static int _populationAtGeneration(SimulationState * state, const char * species, int generation);
static ExtinctionSummary *_findExtinctionSummary(ExtinctionSummary * list,const char * species,const char * eco,const char * region);
static EncounterSummary *_findEncounterSummary(EncounterSummary * list,const char * a,const char * b);

static void _htmlBegin(FILE * out, const char * title);
static void _htmlEnd(FILE * out);
static int _environmentChanged(EnvironmentRecord * a,EnvironmentRecord * b);

// Helpers de las nuevas secciones
static const char * _strategyToString(TokenLabel strategy);
static const char * _dietToString(TokenLabel diet);
static const char * _habitatToString(TokenLabel habitat);
static int _totalPopulationAtGeneration(SimulationState * state, int generation);
static int _totalCarryingCapacity(RuntimeEcosystem * ecosystems);

// Acumulador de energía por especie para el balance de encuentros
typedef struct EnergyAccum {
    const char *         species;
    double               netDelta;   
    struct EnergyAccum * next;
} EnergyAccum;
static EnergyAccum * _findEnergyAccum(EnergyAccum * list, const char * species);
// para el chart
static const char * COLORS[] = {
    "#c36ae6",
    "#6d32da",
    "#1f7cba",
    "#1aa855",
    "#2512f3",
    "#0c453a",
    "#00458a",
    "#b65908"
};
static const char * CHART_JS ="<script src='https://cdn.jsdelivr.net/npm/chart.js'></script>";
static const char * HTML_STYLE =
    "<style>"
    "body{ font-family:Arial,sans-serif;margin:40px; }"
    "h1,h2{ color: #3c0651; }"
    "table{ border-collapse:collapse;width:100%;margin-bottom:20px; }"
    "th,td{ border:1px solid #ddd;padding:8px;text-align:center; }"
    "th{ background: #722192;color:white; }"
    "tr:nth-child(even){ background: #f2f2f2; }"
    "canvas{ margin-bottom:40px; }"
    "</style>";


void writeHTML(FILE * out, SimulationState * state) {

    _htmlBegin(out, "Simulation Report");

    _writeSummaryTable(out, state);
    _writeSpeciesSummary(out, state);
    _writeSpeciesTraitsTable(out, state);      // ficha técnica de especies
    _writeToleranceTable(out, state);          // tolerancia ambiental (T/H/A expandida)

    _writePopulationChart(out, state);
    _writeCapacityChart(out, state);           // población total vs capacidad de carga

    _writeExtinctionTable(out, state);
    _writeExtinctionCauseChart(out, state);    // extinciones por causa (dona)

    _writeEncounterTable(out, state);
    _writeEnergyBalanceChart(out, state);      // balance energético en encuentros

    _writeEnvironmentTable(out, state);

    _htmlEnd(out);
}

static int _environmentChanged(EnvironmentRecord * a,EnvironmentRecord * b) {
    return a->temperature != b->temperature || a->humidity != b->humidity || a->altitude != b->altitude;
}

static void _writeSummaryTable(FILE * out, SimulationState * state) {
    fprintf(out,"<h2>Simulation Summary</h2>\n");

    fprintf(out,
        "<table>\n"
        "<tr>"
        "<th>Generations</th>"
        "<th>Ecosystems</th>"
        "<th>Regions</th>"
        "<th>Species</th>"
        "<th>Encounters</th>"
        "<th>Extinctions</th>"
        "</tr>\n"
    );

    fprintf(out,
        "<tr>"
        "<td>%d</td>"
        "<td>%d</td>"
        "<td>%d</td>"
        "<td>%d</td>"
        "<td>%d</td>"
        "<td>%d</td>"
        "</tr>\n",

        state->currentGeneration,
        _countEcosystems(state->ecosystems),
        _countRegions(state->ecosystems),
        _countSpecies(state->ecosystems),
        _countEncounters(state->encounterHistory),
        _countExtinctions(state->extinctionHistory)
    );

    fprintf(out, "</table>\n");
}

static void _writeSpeciesSummary(FILE * out, SimulationState * state) {
    fprintf(out,
        "<h2>Species Summary</h2>"
        "<table>"
        "<tr>"
        "<th>Species</th>"
        "<th>Final Population</th>"
        "</tr>"
    );

    for (RuntimeEcosystem * eco = state->ecosystems; eco; eco = eco->next) {
        for (RuntimeSpecies * sp = eco->species; sp; sp = sp->next) {
            fprintf(out,
                "<tr>"
                "<td>%s</td>"
                "<td>%d</td>"
                "</tr>",
                sp->name, _lastPopulation(state,eco->name,sp->name)
            );
        }
    }

    fprintf(out, "</table>\n");
}

static void _writeEncounterTable(FILE * out, SimulationState * state) {
    fprintf(out, "<h2>Encounter Summary</h2>\n");
    EncounterSummary * summaries = NULL;
    for(EncounterRecord * e = state->encounterHistory; e; e = e->next){
        EncounterSummary * s = _findEncounterSummary(summaries,e->speciesA,e->speciesB);
        if (!s) {
            s = calloc(1,sizeof(EncounterSummary));
            s->speciesA = e->speciesA;
            s->speciesB = e->speciesB;
            s->next = summaries;
            summaries = s;
        }
        s->count++;
        if (e->speciesARemoved) s->removalsA++;
        if (e->speciesBRemoved) s->removalsB++;
    }
    fprintf(out,
        "<table>"
        "<tr>"
        "<th>Species A</th>"
        "<th>Species B</th>"
        "<th>Encounters</th>"
        "<th>A removals</th>"
        "<th>B removals</th>"
        "</tr>"
    );

    for (EncounterSummary * e = summaries; e; e = e->next) {
        fprintf(out,
            "<tr>"
            "<td>%s</td>"
            "<td>%s</td>"
            "<td>%d</td>"
            "<td>%d</td>"
            "<td>%d</td>"
            "</tr>",

            e->speciesA,
            e->speciesB,
            e->count,
            e->removalsA,
            e->removalsB
        );
    }

    fprintf(out, "</table>\n");
    while (summaries) {
        EncounterSummary * nxt = summaries->next;
        free(summaries);
        summaries = nxt;
    }
}

static void _writeExtinctionTable(FILE * out, SimulationState * state) {

    fprintf(out,"<h2>Extinctions Summary</h2>\n");

    ExtinctionSummary * summaries = NULL;
    for(ExtinctionRecord *e =state->extinctionHistory ; e ; e = e->next){
        ExtinctionSummary *s =  _findExtinctionSummary(summaries,e->speciesName,e->ecosystemName,e->regionName);
        if (!s) {
            s = calloc(1,sizeof(ExtinctionSummary));
            s->species   = e->speciesName;
            s->ecosystem = e->ecosystemName;
            s->region    = e->regionName;
            s->next = summaries;
            s->cause = e->cause;
            summaries = s;
        }
        s->count++;
    }

    fprintf(out,
        "<table>"
        "<tr>"
        "<th>Ecosystem</th>"
        "<th>Region</th>"
        "<th>Species</th>"
        "<th>Cause</th>"
        "<th>Events</th>"
        "</tr>"
    );

    for (ExtinctionSummary * e = summaries; e ; e = e->next) {
        fprintf(out,
            "<tr>"
            "<td>%s</td>"
            "<td>%s</td>"
            "<td>%s</td>"
            "<td>%s</td>"
            "<td>%d</td>"
            "</tr>",

            e->ecosystem,
            e->region,
            e->species,
            _causeToString(e->cause),
            e->count
        );
    }

    fprintf(out,"</table>\n");

    while (summaries) {
        ExtinctionSummary * next = summaries->next;
        free(summaries);
        summaries = next;
    }
}

/* Lleva el ultimo estado ambiental visto de CADA region, para detectar cambios
   por region.*/
typedef struct EnvSeen {
    const char *        region;
    EnvironmentRecord * last;
    struct EnvSeen *    next;
} EnvSeen;

static void _writeEnvironmentTable(FILE * out, SimulationState * state) {
    fprintf(out,"<h2>Environmental Changes</h2>\n");
    fprintf(out,
        "<table>"
        "<tr>"
        "<th>Generation</th>"
        "<th>Region</th>"
        "<th>Temperature</th>"
        "<th>Humidity</th>"
        "<th>Altitude</th>"
        "</tr>"
    );

    EnvSeen * seen = NULL;
    int realChanges = 0;  

    for (EnvironmentRecord * e = state->environmentHistory; e; e = e->next) {
        EnvSeen * s = seen;
        while (s && strcmp(s->region, e->regionName) != 0) s = s->next;

        int isNewRegion = (s == NULL);
        if (!isNewRegion && !_environmentChanged(s->last, e)) continue;

        if (!isNewRegion) realChanges++;

        fprintf(out,
            "<tr>"
            "<td>%d</td>"
            "<td>%s</td>"
            "<td>%d</td>"
            "<td>%d</td>"
            "<td>%d</td>"
            "</tr>",

            e->generation,
            e->regionName,
            e->temperature,
            e->humidity,
            e->altitude
        );

        if (isNewRegion) {
            s = calloc(1, sizeof(EnvSeen));
            s->region = e->regionName;
            s->next   = seen;
            seen      = s;
        }
        s->last = e;
    }

    fprintf(out,"</table>\n");
    if (realChanges == 0)
        fprintf(out,"<p>No environmental changes detected during the simulation.</p>");

    while (seen) {
        EnvSeen * n = seen->next;
        free(seen);
        seen = n;
    }
}

static void _writePopulationChart(FILE * out, SimulationState * state){
    fprintf(out,
        "<h2>Population Evolution</h2>\n"
        "<canvas id='populationChart'></canvas>\n"
        "<script>\n"
    );

    fprintf(out,
        "new Chart(document.getElementById('populationChart'), {\n"
        "type: 'line',\n"
        "data: {\n"
        "labels: ["
    );

    for (int g = 1; g <= state->currentGeneration; g++){
        fprintf(out,"%d", g);
        if (g < state->currentGeneration) fprintf(out,",");
    }

    fprintf(out,
        "],\n"
        "datasets:[\n"
    );

    int colorIndex = 0;
    int firstSpecies = 1;

    for (RuntimeEcosystem * eco = state->ecosystems; eco; eco = eco->next){
        for (RuntimeSpecies * sp = eco->species; sp; sp = sp->next){
            if (!firstSpecies) fprintf(out,",\n");

            firstSpecies = 0;

            fprintf(out,
                "{"
                "label:'%s',"
                "borderColor:'%s',"
                "fill:false,"
                "data:[",
                sp->name,
                COLORS[colorIndex % 8]
            );

            for (int g = 1; g <= state->currentGeneration; g++){
                fprintf(out, "%d", _populationAtGeneration(state,sp->name,g));
                if (g < state->currentGeneration)fprintf(out,",");
            }
            fprintf(out,"]}");
            colorIndex++;
        }
    }

    fprintf(out,
        "]\n"
        "},\n"
        "options:{\n"
        "responsive:true,\n"
        "plugins:{\n"
        "legend:{display:true}\n"
        "},\n"
        "scales:{\n"
        "y:{beginAtZero:true}\n"
        "}\n"
        "}\n"
        "});\n"
        "</script>\n"
    );
}


/* ficha técnica con los rasgos (finales) de cada especie del ecosistema. */
static void _writeSpeciesTraitsTable(FILE * out, SimulationState * state) {
    fprintf(out,
        "<h2>Species Traits</h2>"
        "<table>"
        "<tr>"
        "<th>Species</th>"
        "<th>Lifespan</th>"
        "<th>Speed</th>"
        "<th>Repro. rate</th>"
        "<th>Initial energy</th>"
        "<th>Strategy</th>"
        "<th>Diet</th>"
        "<th>Habitat</th>"
        "</tr>"
    );

    for (RuntimeEcosystem * eco = state->ecosystems; eco; eco = eco->next) {
        for (RuntimeSpecies * sp = eco->species; sp; sp = sp->next) {
            fprintf(out,
                "<tr>"
                "<td>%s</td>"
                "<td>%d</td>"
                "<td>%d</td>"
                "<td>%.2f</td>"
                "<td>%.1f</td>"
                "<td>%s</td>"
                "<td>%s</td>"
                "<td>%s</td>"
                "</tr>",
                sp->name,
                sp->lifespan,
                sp->speed,
                sp->reproductionRate,
                sp->initialEnergy,
                _strategyToString(sp->reproductiveStrategy),
                _dietToString(sp->diet),
                _habitatToString(sp->habitat)
            );
        }
    }

    fprintf(out, "</table>\n");
}

/* Tolerancia ambiental por especie */
static void _writeToleranceTable(FILE * out, SimulationState * state) {
    fprintf(out,
        "<h2>Environmental Tolerance</h2>"
        "<table>"
        "<tr>"
        "<th>Species</th>"
        "<th>Temperature (&deg;C)</th>"
        "<th>Humidity (%%)</th>"
        "<th>Altitude (m)</th>"
        "</tr>"
    );

    for (RuntimeEcosystem * eco = state->ecosystems; eco; eco = eco->next) {
        for (RuntimeSpecies * sp = eco->species; sp; sp = sp->next) {
            EnvTolerance t = sp->envTolerance;
            int hasTolerance = t.temperature.min || t.temperature.max ||
                               t.humidity.min    || t.humidity.max    ||
                               t.altitude.min    || t.altitude.max;

            if (hasTolerance)
                fprintf(out,
                    "<tr>"
                    "<td>%s</td>"
                    "<td>[%d, %d]</td>"
                    "<td>[%d, %d]</td>"
                    "<td>[%d, %d]</td>"
                    "</tr>",
                    sp->name,
                    t.temperature.min, t.temperature.max,
                    t.humidity.min,    t.humidity.max,
                    t.altitude.min,    t.altitude.max
                );
            else
                fprintf(out,
                    "<tr><td>%s</td><td>&mdash;</td><td>&mdash;</td><td>&mdash;</td></tr>",
                    sp->name
                );
        }
    }

    fprintf(out, "</table>\n");
}

/* distribución de las extinciones según su causa (gráfico de dona). */
static void _writeExtinctionCauseChart(FILE * out, SimulationState * state) {
    fprintf(out, "<h2>Extinctions by Cause</h2>\n");

    int byCause[4] = { 0, 0, 0, 0 };  /* AGE, ENERGY, HABITAT, REMOVED */
    int total = 0;
    for (ExtinctionRecord * e = state->extinctionHistory; e; e = e->next) {
        int c = (int) e->cause;
        if (c >= 0 && c <= 3) { byCause[c]++; total++; }
    }

    if (total == 0) {
        fprintf(out, "<p>No extinctions were recorded during the simulation.</p>");
        return;
    }

    fprintf(out,
        /* El div con max-width limita el tamaño de la dona (si no, 'responsive'
           la estira a todo el ancho de la pagina)*/
        "<div style='max-width:340px'>\n"
        "<canvas id='extinctionCauseChart'></canvas>\n"
        "</div>\n"
        "<script>\n"
        "new Chart(document.getElementById('extinctionCauseChart'), {\n"
        "type:'doughnut',\n"
        "data:{\n"
        "labels:['Age','Energy','Habitat','Encounter'],\n"
        "datasets:[{\n"
        "data:[%d,%d,%d,%d],\n"
        "backgroundColor:['%s','%s','%s','%s']\n"
        "}]\n"
        "},\n"
        "options:{responsive:true,plugins:{legend:{position:'bottom'}}}\n"
        "});\n"
        "</script>\n",
        byCause[0], byCause[1], byCause[2], byCause[3],
        COLORS[1], COLORS[4], COLORS[7], COLORS[3]
    );
}

/* población total del ecosistema generación a generación, contra el
   techo de capacidad de carga (suma de carryingCapacity de las regiones). */
static void _writeCapacityChart(FILE * out, SimulationState * state) {
    fprintf(out, "<h2>Total Population vs Carrying Capacity</h2>\n");

    if (state->currentGeneration <= 0) {
        fprintf(out, "<p>No generations were simulated.</p>");
        return;
    }

    int capacity = _totalCarryingCapacity(state->ecosystems);

    fprintf(out,
        "<canvas id='capacityChart'></canvas>\n"
        "<script>\n"
        "new Chart(document.getElementById('capacityChart'), {\n"
        "type:'line',\n"
        "data:{\n"
        "labels:["
    );
    for (int g = 1; g <= state->currentGeneration; g++) {
        fprintf(out, "%d", g);
        if (g < state->currentGeneration) fprintf(out, ",");
    }

    fprintf(out,
        "],\n"
        "datasets:[\n"
        "{label:'Total population',borderColor:'%s',fill:false,data:[",
        COLORS[1]
    );
    for (int g = 1; g <= state->currentGeneration; g++) {
        fprintf(out, "%d", _totalPopulationAtGeneration(state, g));
        if (g < state->currentGeneration) fprintf(out, ",");
    }

    fprintf(out,
        "]},\n"
        "{label:'Carrying capacity',borderColor:'%s',borderDash:[6,6],fill:false,data:[",
        COLORS[7]
    );
    for (int g = 1; g <= state->currentGeneration; g++) {
        fprintf(out, "%d", capacity);
        if (g < state->currentGeneration) fprintf(out, ",");
    }

    fprintf(out,
        "]}\n"
        "]\n"
        "},\n"
        "options:{responsive:true,scales:{y:{beginAtZero:true}}}\n"
        "});\n"
        "</script>\n"
    );
}

/* energía neta ganada (verde) o perdida (rojo) por cada especie a lo
   largo de todos los encuentros en que el individuo sobrevivió. */
static void _writeEnergyBalanceChart(FILE * out, SimulationState * state) {
    fprintf(out, "<h2>Energy Balance in Encounters</h2>\n");

    EnergyAccum * accums = NULL;
    for (EncounterRecord * e = state->encounterHistory; e; e = e->next) {
        if (!e->speciesARemoved) {
            EnergyAccum * a = _findEnergyAccum(accums, e->speciesA);
            if (!a) {
                a = calloc(1, sizeof(EnergyAccum));
                a->species = e->speciesA;
                a->next = accums;
                accums = a;
            }
            a->netDelta += (e->energyAAfter - e->energyABefore);
        }
        if (!e->speciesBRemoved) {
            EnergyAccum * b = _findEnergyAccum(accums, e->speciesB);
            if (!b) {
                b = calloc(1, sizeof(EnergyAccum));
                b->species = e->speciesB;
                b->next = accums;
                accums = b;
            }
            b->netDelta += (e->energyBAfter - e->energyBBefore);
        }
    }

    if (!accums) {
        fprintf(out, "<p>No encounters with surviving individuals were recorded.</p>");
        return;
    }

    fprintf(out,
        "<canvas id='energyBalanceChart'></canvas>\n"
        "<script>\n"
        "new Chart(document.getElementById('energyBalanceChart'), {\n"
        "type:'bar',\n"
        "data:{\n"
        "labels:["
    );
    for (EnergyAccum * a = accums; a; a = a->next)
        fprintf(out, "'%s'%s", a->species, a->next ? "," : "");

    fprintf(out,
        "],\n"
        "datasets:[{\n"
        "label:'Net energy change',\n"
        "data:["
    );
    for (EnergyAccum * a = accums; a; a = a->next)
        fprintf(out, "%.2f%s", a->netDelta, a->next ? "," : "");

    fprintf(out,
        "],\n"
        "backgroundColor:["
    );
    for (EnergyAccum * a = accums; a; a = a->next)
        fprintf(out, "'%s'%s", a->netDelta >= 0 ? "#1aa855" : "#c0392b", a->next ? "," : "");

    fprintf(out,
        "]\n"
        "}]\n"
        "},\n"
        "options:{responsive:true,plugins:{legend:{display:false}},scales:{y:{beginAtZero:true}}}\n"
        "});\n"
        "</script>\n"
    );

    while (accums) {
        EnergyAccum * next = accums->next;
        free(accums);
        accums = next;
    }
}


/*HELPERS*/
static void _htmlBegin(FILE * out, const char * title) {
    fprintf(out,
        "<!DOCTYPE html>"
        "<html>"
        "<head>"
        "<meta charset='utf-8'>"
        "<title>%s</title>"
        "%s"
        "%s"
        "</head>"
        "<body>"
        "<h1>%s</h1>",
        title,
        HTML_STYLE,
        CHART_JS,
        title
    );
}

static void _htmlEnd(FILE * out) {
    fprintf(out, "</body></html>");
}

static int _countEcosystems(RuntimeEcosystem * ecosystems) {
    int count = 0;
    for (; ecosystems; ecosystems = ecosystems->next) count++;
    return count;
}

static int _countRegions(RuntimeEcosystem * ecosystems) {
    int count = 0;

    for (RuntimeEcosystem * eco = ecosystems; eco; eco = eco->next)
        for (RuntimeRegion * reg = eco->regions; reg; reg = reg->next) count++;

    return count;
}

static int _countSpecies(RuntimeEcosystem * ecosystems) {
    int count = 0;

    for (RuntimeEcosystem * eco = ecosystems; eco; eco = eco->next)
        for (RuntimeSpecies * sp = eco->species; sp; sp = sp->next) count++;

    return count;
}

static int _countEncounters(EncounterRecord * history) {
    int count = 0;

    for (; history; history = history->next) count++;

    return count;
}

static int _countExtinctions(ExtinctionRecord * history) {
    int count = 0;

    for (; history; history = history->next) count++;

    return count;
}

static int _lastPopulation(SimulationState * state, const char * ecosystem, const char * species) {
    int lastGen = -1;
    int count   = 0;

    for (PopulationRecord * p = state->history; p; p = p->next) {
        if (strcmp(p->speciesName, species) != 0)  
            continue;
        if (strcmp(p->ecosystemName, ecosystem) != 0)       /*consultar, de que me sirve continue? */
            continue;

        if (p->generation > lastGen) {
            lastGen = p->generation;
            count   = p->count;
        }
    }

    return count;
}

static const char * _causeToString(ExtinctionCause cause) {
    switch (cause) {
        case EXTINCTION_CAUSE_AGE: return "Age";
        case EXTINCTION_CAUSE_ENERGY: return "Energy";
        case EXTINCTION_CAUSE_HABITAT: return "Habitat";
        case EXTINCTION_CAUSE_REMOVED: return "Encounter";
        default: return "Unknown";
    }
}

static int _populationAtGeneration(SimulationState * state, const char * species, int generation){
    for (PopulationRecord * p = state->history; p; p = p->next){
        if (p->generation == generation && strcmp(p->speciesName, species) == 0)
            return p->count;
    }
    return 0;
}

static ExtinctionSummary *_findExtinctionSummary(ExtinctionSummary * list,const char * species,const char * eco,const char * region) {
    while (list) {
        if (strcmp(list->species,species)==0 && strcmp(list->ecosystem,eco)==0 && strcmp(list->region,region)==0)
            return list;
        list = list->next;
    }
    return NULL;
}

static EncounterSummary *_findEncounterSummary(EncounterSummary * list,const char * a,const char * b) {
    while (list) {
        if (strcmp(list->speciesA, a) == 0 &&
            strcmp(list->speciesB, b) == 0)
            return list;
        list = list->next;
    }
    return NULL;
}

static const char * _strategyToString(TokenLabel strategy) {
    switch ((int) strategy) {
        case R_SELECTED:
        case R_STRATEGY: return "r-selected";
        case K_SELECTED:
        case K_STRATEGY: return "K-selected";
        default:         return "&mdash;";
    }
}

static const char * _dietToString(TokenLabel diet) {
    switch ((int) diet) {
        case HERBIVORE:  return "Herbivore";
        case CARNIVORE:  return "Carnivore";
        case OMNIVORE:   return "Omnivore";
        case DECOMPOSER: return "Decomposer";
        default:         return "&mdash;";
    }
}

static const char * _habitatToString(TokenLabel habitat) {
    switch ((int) habitat) {
        case TERRESTRIAL: return "Terrestrial";
        case AQUATIC:     return "Aquatic";
        case AMPHIBIOUS:  return "Amphibious";
        case MIXED:       return "Mixed";
        default:          return "&mdash;";
    }
}

static int _totalPopulationAtGeneration(SimulationState * state, int generation) {
    int total = 0;
    for (PopulationRecord * p = state->history; p; p = p->next)
        if (p->generation == generation) total += p->count;
    return total;
}

static int _totalCarryingCapacity(RuntimeEcosystem * ecosystems) {
    int total = 0;
    for (RuntimeEcosystem * eco = ecosystems; eco; eco = eco->next)
        for (RuntimeRegion * r = eco->regions; r; r = r->next)
            total += r->carryingCapacity;
    return total;
}

static EnergyAccum * _findEnergyAccum(EnergyAccum * list, const char * species) {
    while (list) {
        if (strcmp(list->species, species) == 0) return list;
        list = list->next;
    }
    return NULL;
}
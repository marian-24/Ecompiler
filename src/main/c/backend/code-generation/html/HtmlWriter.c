#include "HtmlWriter.h"
#include "../../../support/logging/Logger.h"
#include "../../../support/type/CompilerState.h"

static void _writePopulationChart(FILE * out, SimulationState * state);
static void _writeSummaryTable(FILE * out, SimulationState * state);
static void _writeSpeciesSummary(FILE * out, SimulationState * state);
static void _writeEncounterTable(FILE * out, SimulationState * state);
static void _writeExtinctionTable(FILE * out, SimulationState * state);
static void _writeEnvironmentTable(FILE * out, SimulationState * state);

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
    "table{ border-collapse:collapse;width:100%%;margin-bottom:20px; }"
    "th,td{ border:1px solid #ddd;padding:8px;text-align:center; }"
    "th{ background: #722192;color:white; }"
    "tr:nth-child(even){ background: #f2f2f2; }"
    "canvas{ margin-bottom:40px; }"
    "</style>";


void writeHTML(FILE * out, SimulationState * state) {

    _htmlBegin(out, "Simulation Report");

    _writeSummaryTable(out, state);
    _writeSpeciesSummary(out, state);

    _writePopulationChart(out, state);

    _writeExtinctionTable(out, state);
    _writeEncounterTable(out, state);
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
    EnvironmentRecord * prev = NULL;
    int changes = 0;
    for (EnvironmentRecord * e = state->environmentHistory; e; e = e->next) {
        if (prev && !_environmentChanged(prev,e)) continue;
        changes++;
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
        prev = e;
    }

    fprintf(out,"</table>\n");
    if (changes <= 1) fprintf(out,"<p>No environmental changes detected during simulation.</p>");
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


/*=====HELPERS====== */
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
/*=====HELPERS====== */
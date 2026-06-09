#include "Generator.h"
/* MODULE INTERNAL STATE */

const char _indentationCharacter = ' ';
const char _indentationSize = 4;
static Logger * _logger = NULL;

/** PUBLIC FUNCTIONS */

void executeGenerator(CompilerState * compilerState) {
	if(compilerState->succeeded != SUCCEEDED) {
		logError(_logger, "Semantic analysis failed. No output will be generated.");
		return;
	}

	Program * program = compilerState->abstractSyntaxtTree;
	if(program == NULL){
		logError(_logger, "AST is empty. No output will be generated.");
		return;
	}

	logDebugging(_logger, "Generating JSON output...");
	FILE * jsonFile = fopen("simulation.json", "w");
    if (jsonFile == NULL) {
        logError(_logger, "Could not create simulation.json");
        return;
    }
    writeJSON(jsonFile, compilerState->simulationState);
    fclose(jsonFile);
	logDebugging(_logger, "simulation.json generated.");

	logDebugging(_logger, "Generating HTML output...");
	FILE * htmlFile = fopen("simulation.html", "w");
	if (htmlFile == NULL) {
		logError(_logger, "Could not create simulation.html");
    	perror("simulation.html");
    	return;
	}
    writeHTML(htmlFile, compilerState->simulationState);
    fclose(htmlFile);
	logDebugging(_logger, "simulation.html generated.");


	logDebugging(_logger, "Generation is done.");
}


/** Shutdown module's internal state. */
void _shutdownGeneratorModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: Generator...");
		destroyLogger(_logger);
		_logger = NULL;
	}
}

ModuleDestructor initializeGeneratorModule() {
	_logger = createLogger("Generator");
	return _shutdownGeneratorModule;
}
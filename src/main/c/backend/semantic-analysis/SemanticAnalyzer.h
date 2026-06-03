#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "../../support/type/CompilationStatus.h"
#include "../../support/type/ModuleDestructor.h"

typedef enum {
	SYMBOL_SPECIES,
	SYMBOL_REGION,
	SYMBOL_ECOSYSTEM
} SymbolType;

typedef struct {
	char * name;
	SymbolType type;
	void * details;
	int line;
} SymbolEntry;

typedef struct {
	SymbolEntry * entries;
	int count;
	int capacity;
} SymbolTable;

struct CompilerState;

ModuleDestructor initializeSemanticAnalyzerModule(struct CompilerState * state);
CompilationStatus executeSemanticAnalysis(struct CompilerState * state);
void destroySemanticAnalyzerState(struct CompilerState * state);

SymbolEntry * symbolTableLookup(SymbolTable * table, const char * name, SymbolType type);
void symbolTableInsert(SymbolTable * table, const char * name, SymbolType type, void * details, int line);
void destroySymbolTable(SymbolTable * table);

#endif

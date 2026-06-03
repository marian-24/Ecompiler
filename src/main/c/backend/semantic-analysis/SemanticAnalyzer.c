#include "../../support/type/CompilerState.h"
#include "SemanticAnalyzer.h"
#include "../../support/logging/Logger.h"
#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include <string.h>
#include <stdlib.h>

static Logger * _logger = NULL;

void _shutdownSemanticAnalyzerModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: SemanticAnalyzer...");
		destroyLogger(_logger);
		_logger = NULL;
	}
}

ModuleDestructor initializeSemanticAnalyzerModule(struct CompilerState * state) {
	_logger = createLogger("SemanticAnalyzer");
	return _shutdownSemanticAnalyzerModule;
}

SymbolEntry * symbolTableLookup(SymbolTable * table, const char * name, SymbolType type) {
	if (table == NULL || name == NULL) return NULL;
	logDebugging(_logger, "Looking up symbol: name='%s' type=%d (table has %d entries)", name, type, table->count);
	for (int i = 0; i < table->count; i++) {
		if (table->entries[i].type == type && strcmp(table->entries[i].name, name) == 0) {
			logDebugging(_logger, "Found symbol '%s'", name);
			return &table->entries[i];
		}
	}
	logDebugging(_logger, "Symbol '%s' not found", name);
	return NULL;
}

void symbolTableInsert(SymbolTable * table, const char * name, SymbolType type, void * details, int line) {
	if (table == NULL || name == NULL) return;

	logDebugging(_logger, "Inserting symbol: name='%s' type=%d", name, type);

	if (table->count >= table->capacity) {
		table->capacity = (table->capacity == 0) ? 10 : table->capacity * 2;
		table->entries = realloc(table->entries, sizeof(SymbolEntry) * table->capacity);
	}

	SymbolEntry * entry = &table->entries[table->count++];
	entry->name = strdup(name);
	entry->type = type;
	entry->details = details;
	entry->line = line;
}

void destroySymbolTable(SymbolTable * table) {
	if (table != NULL) {
		for (int i = 0; i < table->count; i++) {
			free(table->entries[i].name);
		}
		free(table->entries);
		free(table);
	}
}

static ExpressionType _inferExpressionType(Expression * expr) {
	if (expr == NULL) return EXPR_INTEGER;

	switch (expr->type) {
		case EXPR_INTEGER:
		case EXPR_RANDOM_RANGE:
			return EXPR_INTEGER;
		case EXPR_FLOAT:
			return EXPR_FLOAT;
		case EXPR_BOOLEAN:
			return EXPR_BOOLEAN;
		case EXPR_STRING:
			return EXPR_STRING;
		case EXPR_IDENTIFIER:
		case EXPR_ATTRIBUTE_ACCESS:
		case EXPR_ECOSYSTEM_ACCESS:
		case EXPR_POPULATION_OF:
			return EXPR_IDENTIFIER;
		case EXPR_ADD:
		case EXPR_SUB:
		case EXPR_MUL:
		case EXPR_DIV: {
			ExpressionType left = _inferExpressionType(expr->binary.left);
			ExpressionType right = _inferExpressionType(expr->binary.right);
			if (left != right) {
				logError(_logger, "Type mismatch in binary operation: cannot mix %d and %d", left, right);
				return EXPR_INTEGER;
			}
			return left;
		}
		default:
			return EXPR_INTEGER;
	}
}

static CompilationStatus _validateSpeciesDefinition(SymbolTable * table, Statement * stmt) {
	SpeciesDefinition * species = stmt->speciesDefinition;

	if (symbolTableLookup(table, species->name, SYMBOL_SPECIES) != NULL) {
		logError(_logger, "Species '%s' is already defined", species->name);
		return FAILED;
	}

	SpeciesAttributeList * attr = species->attributes;
	while (attr != NULL) {
		if (attr->attribute->type == ATTR_REPRODUCTION_RATE) {
			if (attr->attribute->reproductionRateValue < 0.0 || attr->attribute->reproductionRateValue > 1.0) {
				logError(_logger, "Reproduction rate must be between 0.0 and 1.0, got %f",
					attr->attribute->reproductionRateValue);
				return FAILED;
			}
		}
		attr = attr->next;
	}

	symbolTableInsert(table, species->name, SYMBOL_SPECIES, species, 0);
	return SUCCEEDED;
}

static CompilationStatus _validateRegionDefinition(SymbolTable * table, Statement * stmt) {
	RegionDefinition * region = stmt->regionDefinition;

	if (symbolTableLookup(table, region->name, SYMBOL_REGION) != NULL) {
		logError(_logger, "Region '%s' is already defined", region->name);
		return FAILED;
	}

	if (region->temperature < -50 || region->temperature > 60) {
		logError(_logger, "Invalid temperature: %d (must be between -50 and 60)", region->temperature);
		return FAILED;
	}

	if (region->humidity < 0 || region->humidity > 100) {
		logError(_logger, "Invalid humidity: %d (must be between 0 and 100)", region->humidity);
		return FAILED;
	}

	if (region->altitude < 0) {
		logError(_logger, "Invalid altitude: %d (must be non-negative)", region->altitude);
		return FAILED;
	}

	symbolTableInsert(table, region->name, SYMBOL_REGION, region, 0);
	return SUCCEEDED;
}

static CompilationStatus _validateEcosystemDefinition(SymbolTable * table, Statement * stmt) {
	EcosystemDefinition * ecosystem = stmt->ecosystemDefinition;

	if (ecosystem->members == NULL) {
		logError(_logger, "Ecosystem '%s' is empty", ecosystem->name);
		return FAILED;
	}

	EcosystemMemberList * member = ecosystem->members;
	while (member != NULL) {
		SymbolEntry * entry = NULL;

		if (member->member->type == MEMBER_SPECIES) {
			entry = symbolTableLookup(table, member->member->name, SYMBOL_SPECIES);
			if (entry == NULL) {
				logError(_logger, "Species '%s' is not defined", member->member->name);
				return FAILED;
			}
		} else if (member->member->type == MEMBER_REGION) {
			entry = symbolTableLookup(table, member->member->name, SYMBOL_REGION);
			if (entry == NULL) {
				logError(_logger, "Region '%s' is not defined", member->member->name);
				return FAILED;
			}
		}

		member = member->next;
	}

	symbolTableInsert(table, ecosystem->name, SYMBOL_ECOSYSTEM, ecosystem, 0);
	return SUCCEEDED;
}

static CompilationStatus _validateAddStatement(SymbolTable * table, Statement * stmt) {
	AddStatement * add = stmt->addStatement;

	if (symbolTableLookup(table, add->speciesName, SYMBOL_SPECIES) == NULL) {
		logError(_logger, "Species '%s' is not defined", add->speciesName);
		return FAILED;
	}

	if (symbolTableLookup(table, add->ecosystemName, SYMBOL_ECOSYSTEM) == NULL) {
		logError(_logger, "Ecosystem '%s' is not defined", add->ecosystemName);
		return FAILED;
	}

	if (symbolTableLookup(table, add->regionName, SYMBOL_REGION) == NULL) {
		logError(_logger, "Region '%s' is not defined", add->regionName);
		return FAILED;
	}

	return SUCCEEDED;
}

static CompilationStatus _validateStatement(SymbolTable * table, Statement * stmt) {
	if (stmt == NULL) return SUCCEEDED;

	switch (stmt->type) {
		case STATEMENT_SPECIES:
			return _validateSpeciesDefinition(table, stmt);

		case STATEMENT_REGION:
			return _validateRegionDefinition(table, stmt);

		case STATEMENT_ECOSYSTEM:
			return _validateEcosystemDefinition(table, stmt);

		case STATEMENT_ADD:
			return _validateAddStatement(table, stmt);

		case STATEMENT_REMOVE:
		case STATEMENT_MOVE:
		case STATEMENT_ON_ENCOUNTER:
		case STATEMENT_ON_GENERATION:
		case STATEMENT_EVERY_RANDOM:
		case STATEMENT_SIMULATE:
		case STATEMENT_IF:
		case STATEMENT_WHILE:
		case STATEMENT_FOR_EACH:
		case STATEMENT_LOG:
		case STATEMENT_ATTRIBUTE_ASSIGNMENT:
		case STATEMENT_BEHAVIOR:
			return SUCCEEDED;

		default:
			return SUCCEEDED;
	}
}

CompilationStatus executeSemanticAnalysis(struct CompilerState * state) {
	if (state == NULL || state->abstractSyntaxtTree == NULL) {
		logError(_logger, "Invalid compiler state");
		return FAILED;
	}

	logDebugging(_logger, "Starting semantic analysis");

	SymbolTable * table = calloc(1, sizeof(SymbolTable));
	state->symbolTable = (struct SymbolTable *) table;

	logDebugging(_logger, "First pass: registering species and regions");
	StatementList * current = state->abstractSyntaxtTree->statements;
	while (current != NULL) {
		Statement * stmt = current->statement;
		if (stmt != NULL && (stmt->type == STATEMENT_SPECIES || stmt->type == STATEMENT_REGION)) {
			CompilationStatus status = _validateStatement(table, stmt);
			if (status != SUCCEEDED) {
				logError(_logger, "Semantic analysis failed");
				destroySymbolTable(table);
				state->symbolTable = NULL;
				return FAILED;
			}
		}
		current = current->next;
	}

	logDebugging(_logger, "Second pass: registering ecosystems");
	current = state->abstractSyntaxtTree->statements;
	while (current != NULL) {
		Statement * stmt = current->statement;
		if (stmt != NULL && stmt->type == STATEMENT_ECOSYSTEM) {
			CompilationStatus status = _validateStatement(table, stmt);
			if (status != SUCCEEDED) {
				logError(_logger, "Semantic analysis failed");
				destroySymbolTable(table);
				state->symbolTable = NULL;
				return FAILED;
			}
		}
		current = current->next;
	}

	logDebugging(_logger, "Third pass: validating usage statements (add, remove, etc.)");
	current = state->abstractSyntaxtTree->statements;
	while (current != NULL) {
		Statement * stmt = current->statement;
		if (stmt != NULL && stmt->type != STATEMENT_SPECIES &&
		    stmt->type != STATEMENT_REGION &&
		    stmt->type != STATEMENT_ECOSYSTEM) {
			CompilationStatus status = _validateStatement(table, stmt);
			if (status != SUCCEEDED) {
				logError(_logger, "Semantic analysis failed");
				destroySymbolTable(table);
				state->symbolTable = NULL;
				return FAILED;
			}
		}
		current = current->next;
	}

	logDebugging(_logger, "Semantic analysis completed successfully");
	return SUCCEEDED;
}

void destroySemanticAnalyzerState(struct CompilerState * state) {
	if (state != NULL && state->symbolTable != NULL) {
		destroySymbolTable((SymbolTable *) state->symbolTable);
		state->symbolTable = NULL;
	}
}

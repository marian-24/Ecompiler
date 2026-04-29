#include "AbstractSyntaxTree.h"

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownAbstractSyntaxTreeModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: AbstractSyntaxTree...");
		destroyLogger(_logger);
		_logger = NULL;
	}
}

ModuleDestructor initializeAbstractSyntaxTreeModule() {
	_logger = createLogger("AbstractSyntaxTree");
	return _shutdownAbstractSyntaxTreeModule;
}

/* PUBLIC FUNCTIONS */

void destroyExpression(Expression * expression) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (expression != NULL) {
		switch (expression->type) {
			case EXPR_STRING:
				free(expression->stringValue);
				break;
			case EXPR_IDENTIFIER:
				free(expression->identifier);
				break;
			case EXPR_ATTRIBUTE_ACCESS:
				free(expression->attributeAccess.objectName);
				free(expression->attributeAccess.attributeName);
				break;
			case EXPR_ECOSYSTEM_ACCESS:
				free(expression->ecosystemAccess.ecosystemName);
				free(expression->ecosystemAccess.regionName);
				break;
			case EXPR_POPULATION_OF:
				free(expression->populationOf.speciesName);
				free(expression->populationOf.ecosystemName);
				free(expression->populationOf.regionName);
				break;
			case EXPR_ADD:
			case EXPR_SUB:
			case EXPR_MUL:
			case EXPR_DIV:
				destroyExpression(expression->binary.left);
				destroyExpression(expression->binary.right);
				break;
			case EXPR_INTEGER:
			case EXPR_FLOAT:
			case EXPR_BOOLEAN:
			case EXPR_RANDOM_RANGE:
				// No heap memory to free
				break;
		}
		free(expression);
	}
}

void destroyCondition(Condition * condition) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (condition != NULL) {
		switch (condition->type) {
			case COND_LT:
			case COND_GT:
			case COND_EQ:
			case COND_NEQ:
			case COND_LTE:
			case COND_GTE:
			case COND_AND:
			case COND_OR:
				destroyExpression(condition->binary.left);
				destroyExpression(condition->binary.right);
				break;
			case COND_NOT:
				destroyCondition(condition->operand);
				break;
			case COND_IN:
				free(condition->inOperator.speciesName);
				free(condition->inOperator.ecosystemName);
				free(condition->inOperator.regionName);
				break;
		}
		free(condition);
	}
}

//

void destroySpeciesAttributeList(SpeciesAttributeList * list) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (list != NULL) {
		destroySpeciesAttributeList(list->next);
		free(list->attribute);
		free(list);
	}
}
 
void destroySpeciesDefinition(SpeciesDefinition * species) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (species != NULL) {
		free(species->name);
		destroySpeciesAttributeList(species->attributes);
		free(species);
	}
}

//

void destroyRegionDefinition(RegionDefinition * region) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (region != NULL) {
		free(region->name);
		free(region);
	}
}

//

void destroyEcosystemMemberList(EcosystemMemberList * list) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (list != NULL) {
		destroyEcosystemMemberList(list->next);
		free(list->member->name);
		free(list->member);
		free(list);
	}
}
 
void destroyEcosystemDefinition(EcosystemDefinition * ecosystem) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (ecosystem != NULL) {
		free(ecosystem->name);
		destroyEcosystemMemberList(ecosystem->members);
		free(ecosystem);
	}
}

//

void destroyAddStatement(AddStatement * stmt) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (stmt != NULL) {
		free(stmt->speciesName);
		free(stmt->ecosystemName);
		free(stmt->regionName);
		free(stmt);
	}
}
 
void destroyRemoveStatement(RemoveStatement * stmt) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (stmt != NULL) {
		free(stmt->speciesName);
		free(stmt);
	}
}

void destroyMoveStatement(MoveStatement * stmt) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (stmt != NULL) {
		free(stmt->speciesName);
		free(stmt->ecosystemName);
		free(stmt->regionName);
		free(stmt);
	}
}

//

void destroyAttributeAssignment(AttributeAssignment * assignment) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (assignment != NULL) {
		free(assignment->objectName);
		free(assignment->attributeName);
		destroyExpression(assignment->value);
		free(assignment);
	}
}

//

void destroyOnEncounterBlock(OnEncounterBlock * block) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (block != NULL) {
		free(block->speciesA);
		free(block->speciesB);
		free(block->ecosystemName);
		free(block->regionName);
		destroyStatementList(block->body);
		free(block);
	}
}
 
void destroyOnGenerationBlock(OnGenerationBlock * block) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (block != NULL) {
		free(block->ecosystemName);
		free(block->regionName);
		destroyStatementList(block->body);
		free(block);
	}
}
 
void destroyEveryRandomBlock(EveryRandomBlock * block) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (block != NULL) {
		free(block->ecosystemName);
		free(block->regionName);
		destroyStatementList(block->body);
		free(block);
	}
}

//

void destroySimulateStatement(SimulateStatement * stmt) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (stmt != NULL) {
		free(stmt->ecosystemName);
		free(stmt);
	}
}

//

void destroyIfStatement(IfStatement * stmt) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (stmt != NULL) {
		destroyCondition(stmt->condition);
		destroyStatementList(stmt->thenBody);
		destroyStatementList(stmt->elseBody);
		free(stmt);
	}
}
 
void destroyWhileStatement(WhileStatement * stmt) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (stmt != NULL) {
		destroyCondition(stmt->condition);
		destroyStatementList(stmt->body);
		free(stmt);
	}
}
 
void destroyForEachStatement(ForEachStatement * stmt) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (stmt != NULL) {
		free(stmt->speciesName);
		free(stmt->ecosystemName);
		free(stmt->regionName);
		destroyStatementList(stmt->body);
		free(stmt);
	}
}

//

void destroyLogStatement(LogStatement * stmt) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (stmt != NULL) {
		free(stmt->speciesName);
		free(stmt->ecosystemName);
		free(stmt->regionName);
		free(stmt);
	}
}

void destroyStatement(Statement * statement) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (statement != NULL) {
		switch (statement->type) {
			case STATEMENT_SPECIES:
				destroySpeciesDefinition(statement->speciesDefinition);
				break;
			case STATEMENT_REGION:
				destroyRegionDefinition(statement->regionDefinition);
				break;
			case STATEMENT_ECOSYSTEM:
				destroyEcosystemDefinition(statement->ecosystemDefinition);
				break;
			case STATEMENT_ADD:
				destroyAddStatement(statement->addStatement);
				break;
			case STATEMENT_REMOVE:
				destroyRemoveStatement(statement->removeStatement);
				break;
			case STATEMENT_MOVE:
				destroyMoveStatement(statement->moveStatement);
				break;
			case STATEMENT_ON_ENCOUNTER:
				destroyOnEncounterBlock(statement->onEncounterBlock);
				break;
			case STATEMENT_ON_GENERATION:
				destroyOnGenerationBlock(statement->onGenerationBlock);
				break;
			case STATEMENT_EVERY_RANDOM:
				destroyEveryRandomBlock(statement->everyRandomBlock);
				break;
			case STATEMENT_SIMULATE:
				destroySimulateStatement(statement->simulateStatement);
				break;
			case STATEMENT_IF:
				destroyIfStatement(statement->ifStatement);
				break;
			case STATEMENT_WHILE:
				destroyWhileStatement(statement->whileStatement);
				break;
			case STATEMENT_FOR_EACH:
				destroyForEachStatement(statement->forEachStatement);
				break;
			case STATEMENT_LOG:
				destroyLogStatement(statement->logStatement);
				break;
			case STATEMENT_ATTRIBUTE_ASSIGNMENT:
				destroyAttributeAssignment(statement->attributeAssignment);
				break;
		}
		free(statement);
	}
}
 
void destroyStatementList(StatementList * list) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (list != NULL) {
		destroyStatement(list->statement);
		destroyStatementList(list->next);
		free(list);
	}
}

//

void destroyProgram(Program * program) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (program != NULL) {
		destroyStatementList(program->statements);
		free(program);
	}
}

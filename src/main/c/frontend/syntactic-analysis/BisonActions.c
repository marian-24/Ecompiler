#include "BisonActions.h"

static CompilerState * _compilerState = NULL;
static Logger * _logger = NULL;

void _shutdownBisonActionsModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: BisonActions...");
		destroyLogger(_logger);
		_logger = NULL;
	}
	_compilerState = NULL;
}

ModuleDestructor initializeBisonActionsModule(CompilerState * compilerState) {
	_compilerState = compilerState;
	_logger = createLogger("BisonActions");
	return _shutdownBisonActionsModule;
}


static void _logSyntacticAnalyzerAction(const char * functionName) {
	logDebugging(_logger, "%s", functionName);
}


Program * ProgramSemanticAction(StatementList * statements) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Program * program = calloc(1, sizeof(Program));
	program->statements = statements;
	_compilerState->abstractSyntaxtTree = program;
	return program;
}

StatementList * StatementListSemanticAction(Statement * statement, StatementList * next) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	StatementList * list = calloc(1, sizeof(StatementList));
	list->statement = statement;
	list->next = next;
	return list;
}


Statement * SpeciesStatementSemanticAction(SpeciesDefinition * def) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Statement * stmt = calloc(1, sizeof(Statement));
	stmt->type = STATEMENT_SPECIES;
	stmt->speciesDefinition = def;
	return stmt;
}

Statement * RegionStatementSemanticAction(RegionDefinition * def) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Statement * stmt = calloc(1, sizeof(Statement));
	stmt->type = STATEMENT_REGION;
	stmt->regionDefinition = def;
	return stmt;
}

Statement * EcosystemStatementSemanticAction(EcosystemDefinition * def) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Statement * stmt = calloc(1, sizeof(Statement));
	stmt->type = STATEMENT_ECOSYSTEM;
	stmt->ecosystemDefinition = def;
	return stmt;
}

Statement * AddStatementWrapperSemanticAction(AddStatement * add) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Statement * stmt = calloc(1, sizeof(Statement));
	stmt->type = STATEMENT_ADD;
	stmt->addStatement = add;
	return stmt;
}

Statement * RemoveStatementWrapperSemanticAction(RemoveStatement * rem) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Statement * stmt = calloc(1, sizeof(Statement));
	stmt->type = STATEMENT_REMOVE;
	stmt->removeStatement = rem;
	return stmt;
}

Statement * MoveStatementWrapperSemanticAction(MoveStatement * move) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Statement * stmt = calloc(1, sizeof(Statement));
	stmt->type = STATEMENT_MOVE;
	stmt->moveStatement = move;
	return stmt;
}

Statement * OnEncounterStatementSemanticAction(OnEncounterBlock * block) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Statement * stmt = calloc(1, sizeof(Statement));
	stmt->type = STATEMENT_ON_ENCOUNTER;
	stmt->onEncounterBlock = block;
	return stmt;
}

Statement * OnGenerationStatementSemanticAction(OnGenerationBlock * block) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Statement * stmt = calloc(1, sizeof(Statement));
	stmt->type = STATEMENT_ON_GENERATION;
	stmt->onGenerationBlock = block;
	return stmt;
}

Statement * EveryRandomStatementSemanticAction(EveryRandomBlock * block) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Statement * stmt = calloc(1, sizeof(Statement));
	stmt->type = STATEMENT_EVERY_RANDOM;
	stmt->everyRandomBlock = block;
	return stmt;
}

Statement * SimulateStatementWrapperSemanticAction(SimulateStatement * sim) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Statement * stmt = calloc(1, sizeof(Statement));
	stmt->type = STATEMENT_SIMULATE;
	stmt->simulateStatement = sim;
	return stmt;
}

Statement * IfStatementWrapperSemanticAction(IfStatement * ifStmt) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Statement * stmt = calloc(1, sizeof(Statement));
	stmt->type = STATEMENT_IF;
	stmt->ifStatement = ifStmt;
	return stmt;
}

Statement * WhileStatementWrapperSemanticAction(WhileStatement * whileStmt) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Statement * stmt = calloc(1, sizeof(Statement));
	stmt->type = STATEMENT_WHILE;
	stmt->whileStatement = whileStmt;
	return stmt;
}

Statement * ForEachStatementWrapperSemanticAction(ForEachStatement * forStmt) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Statement * stmt = calloc(1, sizeof(Statement));
	stmt->type = STATEMENT_FOR_EACH;
	stmt->forEachStatement = forStmt;
	return stmt;
}

Statement * LogStatementWrapperSemanticAction(LogStatement * logStmt) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Statement * stmt = calloc(1, sizeof(Statement));
	stmt->type = STATEMENT_LOG;
	stmt->logStatement = logStmt;
	return stmt;
}

Statement * AttributeAssignmentStatementSemanticAction(AttributeAssignment * assignment) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Statement * stmt = calloc(1, sizeof(Statement));
	stmt->type = STATEMENT_ATTRIBUTE_ASSIGNMENT;
	stmt->attributeAssignment = assignment;
	return stmt;
}

Statement * BehaviorStatementSemanticAction(BehaviorType type) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Statement * stmt = calloc(1, sizeof(Statement));
	stmt->type = STATEMENT_BEHAVIOR;
	stmt->behavior = type;
	return stmt;
}


Statement * IntDeclarationSemanticAction(char * name, Expression * value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	AttributeAssignment * assignment = calloc(1, sizeof(AttributeAssignment));
	assignment->objectName    = strdup(name);
	assignment->attributeName = strdup("");   /* sin objeto – variable local */
	assignment->op            = ASSIGN_SIMPLE;
	assignment->value         = value;
	Statement * stmt = calloc(1, sizeof(Statement));
	stmt->type = STATEMENT_ATTRIBUTE_ASSIGNMENT;
	stmt->attributeAssignment = assignment;
	return stmt;
}

Statement * FloatDeclarationSemanticAction(char * name, Expression * value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	AttributeAssignment * assignment = calloc(1, sizeof(AttributeAssignment));
	assignment->objectName    = strdup(name);
	assignment->attributeName = strdup("");
	assignment->op            = ASSIGN_SIMPLE;
	assignment->value         = value;
	Statement * stmt = calloc(1, sizeof(Statement));
	stmt->type = STATEMENT_ATTRIBUTE_ASSIGNMENT;
	stmt->attributeAssignment = assignment;
	return stmt;
}

Statement * StringDeclarationSemanticAction(char * name, Expression * value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	AttributeAssignment * assignment = calloc(1, sizeof(AttributeAssignment));
	assignment->objectName    = strdup(name);
	assignment->attributeName = strdup("");
	assignment->op            = ASSIGN_SIMPLE;
	assignment->value         = value;
	Statement * stmt = calloc(1, sizeof(Statement));
	stmt->type = STATEMENT_ATTRIBUTE_ASSIGNMENT;
	stmt->attributeAssignment = assignment;
	return stmt;
}

Statement * BooleanDeclarationSemanticAction(char * name, Expression * value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	AttributeAssignment * assignment = calloc(1, sizeof(AttributeAssignment));
	assignment->objectName    = strdup(name);
	assignment->attributeName = strdup("");
	assignment->op            = ASSIGN_SIMPLE;
	assignment->value         = value;
	Statement * stmt = calloc(1, sizeof(Statement));
	stmt->type = STATEMENT_ATTRIBUTE_ASSIGNMENT;
	stmt->attributeAssignment = assignment;
	return stmt;
}


SpeciesDefinition * SpeciesDefinitionSemanticAction(char * name, SpeciesAttributeList * attributes) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	SpeciesDefinition * species = calloc(1, sizeof(SpeciesDefinition));
	species->name       = strdup(name);
	species->attributes = attributes;
	return species;
}

SpeciesAttributeList * SpeciesAttributeListSemanticAction(SpeciesAttribute * attribute, SpeciesAttributeList * next) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	SpeciesAttributeList * list = calloc(1, sizeof(SpeciesAttributeList));
	list->attribute = attribute;
	list->next      = next;
	return list;
}

SpeciesAttribute * LifespanAttributeSemanticAction(int value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	SpeciesAttribute * attr = calloc(1, sizeof(SpeciesAttribute));
	attr->type          = ATTR_LIFESPAN;
	attr->lifespanValue = value;
	return attr;
}

SpeciesAttribute * ReproductionRateAttributeSemanticAction(double value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	SpeciesAttribute * attr = calloc(1, sizeof(SpeciesAttribute));
	attr->type                  = ATTR_REPRODUCTION_RATE;
	attr->reproductionRateValue = value;
	return attr;
}

SpeciesAttribute * SpeedAttributeSemanticAction(int value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	SpeciesAttribute * attr = calloc(1, sizeof(SpeciesAttribute));
	attr->type       = ATTR_SPEED;
	attr->speedValue = value;
	return attr;
}

SpeciesAttribute * ReproductiveStrategyAttributeSemanticAction(TokenLabel strategy) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	SpeciesAttribute * attr = calloc(1, sizeof(SpeciesAttribute));
	attr->type                  = ATTR_REPRODUCTIVE_STRATEGY;
	attr->reproductiveStrategy  = strategy;
	return attr;
}

SpeciesAttribute * DietAttributeSemanticAction(TokenLabel diet) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	SpeciesAttribute * attr = calloc(1, sizeof(SpeciesAttribute));
	attr->type      = ATTR_DIET;
	attr->dietValue = diet;
	return attr;
}

SpeciesAttribute * HabitatAttributeSemanticAction(TokenLabel habitat) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	SpeciesAttribute * attr = calloc(1, sizeof(SpeciesAttribute));
	attr->type         = ATTR_HABITAT;
	attr->habitatValue = habitat;
	return attr;
}

SpeciesAttribute * EnvToleranceAttributeSemanticAction(EnvTolerance * tolerance) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	SpeciesAttribute * attr = calloc(1, sizeof(SpeciesAttribute));
	attr->type         = ATTR_ENVIRONMENTAL_TOLERANCE;
	attr->envTolerance = *tolerance;
	free(tolerance);
	return attr;
}

EnvTolerance * EnvToleranceSemanticAction(RangeValue * temperature, RangeValue * humidity, RangeValue * altitude) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	EnvTolerance * tolerance = calloc(1, sizeof(EnvTolerance));
	tolerance->temperature = *temperature;
	tolerance->humidity    = *humidity;
	tolerance->altitude    = *altitude;
	free(temperature);
	free(humidity);
	free(altitude);
	return tolerance;
}

RangeValue * RangeValueSemanticAction(int min, int max) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	RangeValue * range = calloc(1, sizeof(RangeValue));
	range->min = min;
	range->max = max;
	return range;
}


RegionDefinition * RegionDefinitionSemanticAction(char * name, int temperature, int humidity,
                                                   int altitude, int carryingCapacity, TokenLabel habitat) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	RegionDefinition * region = calloc(1, sizeof(RegionDefinition));
	region->name             = strdup(name);
	region->temperature      = temperature;
	region->humidity         = humidity;
	region->altitude         = altitude;
	region->carryingCapacity = carryingCapacity;
	region->habitat          = habitat;
	return region;
}


EcosystemDefinition * EcosystemDefinitionSemanticAction(char * name, EcosystemMemberList * members) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	EcosystemDefinition * ecosystem = calloc(1, sizeof(EcosystemDefinition));
	ecosystem->name    = strdup(name);
	ecosystem->members = members;
	return ecosystem;
}

EcosystemMemberList * EcosystemMemberListSemanticAction(EcosystemMember * member, EcosystemMemberList * next) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	EcosystemMemberList * list = calloc(1, sizeof(EcosystemMemberList));
	list->member = member;
	list->next   = next;
	return list;
}

EcosystemMember * EcosystemMemberSemanticAction(EcosystemMemberType type, char * name) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	EcosystemMember * member = calloc(1, sizeof(EcosystemMember));
	member->type = type;
	member->name = strdup(name);
	return member;
}


AddStatement * AddStatementSemanticAction(int amount, char * speciesName,
                                           char * ecosystemName, char * regionName) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	AddStatement * stmt = calloc(1, sizeof(AddStatement));
	stmt->amount        = amount;
	stmt->speciesName   = strdup(speciesName);
	stmt->ecosystemName = strdup(ecosystemName);
	stmt->regionName    = strdup(regionName);
	return stmt;
}

RemoveStatement * RemoveStatementSemanticAction(char * speciesName) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	RemoveStatement * stmt = calloc(1, sizeof(RemoveStatement));
	stmt->speciesName = strdup(speciesName);
	return stmt;
}

MoveStatement * MoveStatementSemanticAction(char * speciesName, char * ecosystemName, char * regionName) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	MoveStatement * stmt = calloc(1, sizeof(MoveStatement));
	stmt->speciesName   = strdup(speciesName);
	stmt->ecosystemName = strdup(ecosystemName);
	stmt->regionName    = strdup(regionName);
	return stmt;
}


AttributeAssignment * AttributeAssignmentSemanticAction(char * objectName, char * attributeName,
                                                          AssignmentOperatorType op, Expression * value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	AttributeAssignment * assignment = calloc(1, sizeof(AttributeAssignment));
	assignment->objectName    = strdup(objectName);
	assignment->attributeName = strdup(attributeName);
	assignment->op            = op;
	assignment->value         = value;
	return assignment;
}


OnEncounterBlock * OnEncounterBlockSemanticAction(char * speciesA, char * speciesB,
                                                    char * ecosystemName, char * regionName,
                                                    StatementList * body) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	OnEncounterBlock * block = calloc(1, sizeof(OnEncounterBlock));
	block->speciesA     = strdup(speciesA);
	block->speciesB     = strdup(speciesB);
	block->ecosystemName = strdup(ecosystemName);
	block->regionName   = strdup(regionName);
	block->body         = body;
	return block;
}

OnGenerationBlock * OnGenerationBlockSemanticAction(int generationNumber, char * ecosystemName,
                                                      char * regionName, StatementList * body) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	OnGenerationBlock * block = calloc(1, sizeof(OnGenerationBlock));
	block->generationNumber = generationNumber;
	block->ecosystemName    = strdup(ecosystemName);
	block->regionName       = strdup(regionName);
	block->body             = body;
	return block;
}

EveryRandomBlock * EveryRandomBlockSemanticAction(char * ecosystemName, char * regionName,
                                                    StatementList * body) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	EveryRandomBlock * block = calloc(1, sizeof(EveryRandomBlock));
	block->ecosystemName = strdup(ecosystemName);
	block->regionName    = strdup(regionName);
	block->body          = body;
	return block;
}


SimulateStatement * SimulateStatementSemanticAction(char * ecosystemName, int generations,
                                                      int hasSeed, int seedValue) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	SimulateStatement * stmt = calloc(1, sizeof(SimulateStatement));
	stmt->ecosystemName = strdup(ecosystemName);
	stmt->generations   = generations;
	stmt->hasSeed       = hasSeed;
	stmt->seedValue     = seedValue;
	return stmt;
}


IfStatement * IfStatementSemanticAction(Condition * condition, StatementList * thenBody,
                                         StatementList * elseBody) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	IfStatement * stmt = calloc(1, sizeof(IfStatement));
	stmt->condition = condition;
	stmt->thenBody  = thenBody;
	stmt->elseBody  = elseBody;   /* NULL when there is no else branch */
	return stmt;
}

WhileStatement * WhileStatementSemanticAction(Condition * condition, StatementList * body) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	WhileStatement * stmt = calloc(1, sizeof(WhileStatement));
	stmt->condition = condition;
	stmt->body      = body;
	return stmt;
}

ForEachStatement * ForEachStatementSemanticAction(char * speciesName, char * ecosystemName,
                                                    char * regionName, StatementList * body) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ForEachStatement * stmt = calloc(1, sizeof(ForEachStatement));
	stmt->speciesName   = strdup(speciesName);
	stmt->ecosystemName = strdup(ecosystemName);
	stmt->regionName    = strdup(regionName);
	stmt->body          = body;
	return stmt;
}


LogStatement * LogPopulationSemanticAction(char * speciesName, char * ecosystemName, char * regionName) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	LogStatement * stmt = calloc(1, sizeof(LogStatement));
	stmt->type          = LOG_POPULATION;
	stmt->speciesName   = strdup(speciesName);
	stmt->ecosystemName = strdup(ecosystemName);
	stmt->regionName    = strdup(regionName);
	return stmt;
}

LogStatement * LogStateSemanticAction(char * ecosystemName) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	LogStatement * stmt = calloc(1, sizeof(LogStatement));
	stmt->type          = LOG_STATE;
	stmt->speciesName   = NULL;
	stmt->ecosystemName = strdup(ecosystemName);
	stmt->regionName    = NULL;
	return stmt;
}

LogStatement * LogGenerationSemanticAction(void) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	LogStatement * stmt = calloc(1, sizeof(LogStatement));
	stmt->type          = LOG_GENERATION;
	stmt->speciesName   = NULL;
	stmt->ecosystemName = NULL;
	stmt->regionName    = NULL;
	return stmt;
}

Expression * IntegerExpressionSemanticAction(int value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expr = calloc(1, sizeof(Expression));
	expr->type     = EXPR_INTEGER;
	expr->intValue = value;
	return expr;
}

Expression * FloatExpressionSemanticAction(double value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expr = calloc(1, sizeof(Expression));
	expr->type       = EXPR_FLOAT;
	expr->floatValue = value;
	return expr;
}

Expression * BooleanExpressionSemanticAction(int value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expr = calloc(1, sizeof(Expression));
	expr->type      = EXPR_BOOLEAN;
	expr->boolValue = value;
	return expr;
}

Expression * StringExpressionSemanticAction(char * value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expr = calloc(1, sizeof(Expression));
	expr->type        = EXPR_STRING;
	expr->stringValue = strdup(value);
	return expr;
}

Expression * IdentifierExpressionSemanticAction(char * name) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expr = calloc(1, sizeof(Expression));
	expr->type       = EXPR_IDENTIFIER;
	expr->identifier = strdup(name);
	return expr;
}

Expression * AttributeAccessExpressionSemanticAction(char * objectName, char * attributeName) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expr = calloc(1, sizeof(Expression));
	expr->type                          = EXPR_ATTRIBUTE_ACCESS;
	expr->attributeAccess.objectName    = strdup(objectName);
	expr->attributeAccess.attributeName = strdup(attributeName);
	return expr;
}

Expression * PopulationOfExpressionSemanticAction(char * speciesName, char * ecosystemName,
                                                    char * regionName) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expr = calloc(1, sizeof(Expression));
	expr->type                       = EXPR_POPULATION_OF;
	expr->populationOf.speciesName   = strdup(speciesName);
	expr->populationOf.ecosystemName = strdup(ecosystemName);
	expr->populationOf.regionName    = strdup(regionName);
	return expr;
}

Expression * RandomRangeExpressionSemanticAction(int min, int max) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expr = calloc(1, sizeof(Expression));
	expr->type              = EXPR_RANDOM_RANGE;
	expr->randomRange.min   = min;
	expr->randomRange.max   = max;
	return expr;
}

Expression * BinaryExpressionSemanticAction(Expression * left, Expression * right, ExpressionType type) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expr = calloc(1, sizeof(Expression));
	expr->type          = type;
	expr->binary.left   = left;
	expr->binary.right  = right;
	return expr;
}

Condition * BinaryConditionSemanticAction(Expression * left, Expression * right, ConditionType type) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Condition * cond = calloc(1, sizeof(Condition));
	cond->type         = type;
	cond->binary.left  = left;
	cond->binary.right = right;
	return cond;
}

Condition * LogicalConditionSemanticAction(Condition * left, Condition * right, ConditionType type) {
	_logSyntacticAnalyzerAction(__FUNCTION__);

	Condition * cond = calloc(1, sizeof(Condition));
	cond->type         = type;
	cond->binary.left  = (Expression *) left;
	cond->binary.right = (Expression *) right;
	return cond;
}

Condition * NotConditionSemanticAction(Condition * operand) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Condition * cond = calloc(1, sizeof(Condition));
	cond->type    = COND_NOT;
	cond->operand = operand;
	return cond;
}

Condition * InConditionSemanticAction(char * speciesName, char * ecosystemName, char * regionName) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Condition * cond = calloc(1, sizeof(Condition));
	cond->type                       = COND_IN;
	cond->inOperator.speciesName     = strdup(speciesName);
	cond->inOperator.ecosystemName   = strdup(ecosystemName);
	cond->inOperator.regionName      = strdup(regionName);
	return cond;
}

Constant * IntegerConstantSemanticAction(const int value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Constant * constant = calloc(1, sizeof(Constant));
	constant->value = value;
	return constant;
}

Expression * ArithmeticExpressionSemanticAction(Expression * leftExpression, Expression * rightExpression, ExpressionType type) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return BinaryExpressionSemanticAction(leftExpression, rightExpression, type);
}

Expression * FactorExpressionSemanticAction(Factor * factor) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expression = calloc(1, sizeof(Expression));
	expression->factor = factor;
	expression->type = FACTOR;
	return expression;
}

Factor * ConstantFactorSemanticAction(Constant * constant) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Factor * factor = calloc(1, sizeof(Factor));
	factor->constant = constant;
	factor->type = CONSTANT;
	return factor;
}

Factor * ExpressionFactorSemanticAction(Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Factor * factor = calloc(1, sizeof(Factor));
	factor->expression = expression;
	factor->type = EXPRESSION;
	return factor;
}

Program * ExpressionProgramSemanticAction(Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Program * program = calloc(1, sizeof(Program));
	program->expression = expression;
	_compilerState->abstractSyntaxtTree = program;
	return program;
}
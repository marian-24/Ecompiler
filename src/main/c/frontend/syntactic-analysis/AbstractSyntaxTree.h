#ifndef ABSTRACT_SYNTAX_TREE_HEADER
#define ABSTRACT_SYNTAX_TREE_HEADER

#include "../../support/logging/Logger.h"
#include "../../support/type/ModuleDestructor.h"
#include "../../support/type/TokenLabel.h"
#include <stdlib.h>

/** Initialize module's internal state. */
ModuleDestructor initializeAbstractSyntaxTreeModule();

/**
 * This type definitions allows self-referencing types (e.g., an expression
 * that is made of another expressions, such as talking about you in 3rd
 * person, but without the madness).
 */


typedef struct Program Program;
typedef struct StatementList StatementList;
typedef struct Statement Statement;

//species
typedef struct SpeciesDefinition SpeciesDefinition;
typedef struct SpeciesAttributeList SpeciesAttributeList;
typedef struct SpeciesAttribute SpeciesAttribute;
typedef struct EnvTolerance EnvTolerance;
typedef struct RangeValue RangeValue;

//Region
typedef struct RegionDefinition     RegionDefinition;

//Ecosystem
typedef struct EcosystemDefinition  EcosystemDefinition;
typedef struct EcosystemMemberList  EcosystemMemberList;
typedef struct EcosystemMember      EcosystemMember;

//Population
typedef struct AddStatement         AddStatement;
typedef struct RemoveStatement      RemoveStatement;
typedef struct MoveStatement        MoveStatement;

//Encounter
typedef struct OnEncounterBlock     OnEncounterBlock;
typedef struct OnGenerationBlock    OnGenerationBlock;
typedef struct EveryRandomBlock     EveryRandomBlock;

//Simulation
typedef struct SimulateStatement    SimulateStatement;

//Control flow
typedef struct IfStatement          IfStatement;
typedef struct WhileStatement       WhileStatement;
typedef struct ForEachStatement     ForEachStatement;

//log
typedef struct LogStatement         LogStatement;

//expressions
typedef struct Expression           Expression;
typedef struct Condition            Condition;

//attribute modificaion
typedef struct AttributeAssignment  AttributeAssignment;

/**
 * Node types for the Abstract Syntax Tree (AST).
 */



typedef enum StatementType {
	STATEMENT_SPECIES,
	STATEMENT_REGION,
	STATEMENT_ECOSYSTEM,
	STATEMENT_ADD,
	STATEMENT_REMOVE,
	STATEMENT_MOVE,
	STATEMENT_ON_ENCOUNTER,
	STATEMENT_ON_GENERATION,
	STATEMENT_EVERY_RANDOM,
	STATEMENT_SIMULATE,
	STATEMENT_IF,
	STATEMENT_WHILE,
	STATEMENT_FOR_EACH,
	STATEMENT_LOG,
	STATEMENT_ATTRIBUTE_ASSIGNMENT
} StatementType;

typedef enum SpeciesAttributeType {
	ATTR_LIFESPAN,
	ATTR_REPRODUCTION_RATE,
	ATTR_SPEED,
	ATTR_REPRODUCTIVE_STRATEGY,
	ATTR_DIET,
	ATTR_HABITAT,
	ATTR_ENVIRONMENTAL_TOLERANCE
} SpeciesAttributeType;

typedef enum EcosystemMemberType {
	MEMBER_SPECIES,
	MEMBER_REGION
} EcosystemMemberType;
 
typedef enum LogType {
	LOG_POPULATION,
	LOG_STATE,
	LOG_GENERATION
} LogType;

typedef enum ExpressionType {
	EXPR_INTEGER,
	EXPR_FLOAT,
	EXPR_BOOLEAN,
	EXPR_STRING,
	EXPR_IDENTIFIER,
	EXPR_ATTRIBUTE_ACCESS,    // Especie.atributo
	EXPR_ECOSYSTEM_ACCESS,    // Ecosistema.Region
	EXPR_POPULATION_OF,       // population of Especie in Ecosistema.Region
	EXPR_RANDOM_RANGE,        // random[min, max]
	EXPR_ADD,
	EXPR_SUB,
	EXPR_MUL,
	EXPR_DIV
} ExpressionType;

typedef enum ConditionType {
	COND_LT,
	COND_GT,
	COND_EQ,
	COND_NEQ,
	COND_LTE,
	COND_GTE,
	COND_AND,
	COND_OR,
	COND_NOT,
	COND_IN     // Especie in Ecosistema.Region
} ConditionType;

typedef enum AssignmentOperatorType {
	ASSIGN_SIMPLE,   // =
	ASSIGN_ADD,      // +=
	ASSIGN_SUB,      // -=
	ASSIGN_MUL,      // *=
	ASSIGN_DIV       // /=
} AssignmentOperatorType;



//expression node

struct Expression {
	ExpressionType type;
	union {
		int intValue;
		double floatValue;
		int boolValue;
		char * stringValue;
		char * identifier;
		struct {
			char * objectName;
			char * attributeName;
		} attributeAccess;               // Puma.speed
		struct {
			char * ecosystemName;
			char * regionName;
		} ecosystemAccess;               // Patagonia.Bosque
		struct {
			char * speciesName;
			char * ecosystemName;
			char * regionName;
		} populationOf;                  // population of Puma in Patagonia.Bosque
		struct {
			int min;
			int max;
		} randomRange;                   // random[min, max]
		struct {
			Expression * left;
			Expression * right;
		} binary;                        // left OP right
	};
};
 
 
 //condition node
struct Condition {
	ConditionType type;
	union {
		struct {
			Expression * left;
			Expression * right;
		} binary;                        // left OP right
		struct {
			char * speciesName;
			char * ecosystemName;
			char * regionName;
		} inOperator;                    // Especie in Ecosistema.Region
		Condition * operand;             // NOT condition
	};
};

 //range value
struct RangeValue {
	int min;
	int max;
};

 //environmental
struct EnvTolerance {
	RangeValue temperature;
	RangeValue humidity;
	RangeValue altitude;
};

 //species
struct SpeciesAttribute {
	SpeciesAttributeType type;
	union {
		int lifespanValue;               // int lifespan = N
		double reproductionRateValue;    // float reproduction_rate = N
		int speedValue;                  // int speed = N
		TokenLabel reproductiveStrategy; // r_selected | k_selected
		TokenLabel dietValue;            // herbivore | carnivore | omnivore | decomposer
		TokenLabel habitatValue;         // terrestrial | aquatic | amphibious
		EnvTolerance envTolerance;       // environmental_tolerance { ... }
	};
};

struct SpeciesAttributeList {
	SpeciesAttribute * attribute;
	SpeciesAttributeList * next;
};

struct SpeciesDefinition {
	char * name;
	SpeciesAttributeList * attributes;
};

 //region
struct RegionDefinition {
	char * name;
	int temperature;
	int humidity;
	int altitude;
	int carryingCapacity;
	TokenLabel habitat;              // terrestrial | aquatic | mixed
};

 //ecosystem
struct EcosystemMember {
	EcosystemMemberType type;
	char * name;
};
 
struct EcosystemMemberList {
	EcosystemMember * member;
	EcosystemMemberList * next;
};

struct EcosystemDefinition {
	char * name;
	EcosystemMemberList * members;
};

 //attribute 
struct AttributeAssignment {
	char * objectName;
	char * attributeName;
	AssignmentOperatorType op;
	Expression * value;
};

 //statement
struct AddStatement {
	int amount;
	char * speciesName;
	char * ecosystemName;
	char * regionName;
};

struct RemoveStatement {
	char * speciesName;
};

struct MoveStatement {
	char * speciesName;
	char * ecosystemName;
	char * regionName;
};
 
struct Statement {
	StatementType type;
	union {
		SpeciesDefinition * speciesDefinition;
		RegionDefinition * regionDefinition;
		EcosystemDefinition * ecosystemDefinition;
		AddStatement * addStatement;
		RemoveStatement * removeStatement;
		MoveStatement * moveStatement;
		struct OnEncounterBlock * onEncounterBlock;
		struct OnGenerationBlock * onGenerationBlock;
		struct EveryRandomBlock * everyRandomBlock;
		struct SimulateStatement * simulateStatement;
		struct IfStatement * ifStatement;
		struct WhileStatement * whileStatement;
		struct ForEachStatement * forEachStatement;
		struct LogStatement * logStatement;
		AttributeAssignment * attributeAssignment;
	};
};

struct StatementList {
	Statement * statement;
	StatementList * next;
};

 //on encounter
struct OnEncounterBlock {
	char * speciesA;
	char * speciesB;
	char * ecosystemName;
	char * regionName;
	StatementList * body;
};

 //on generation
struct OnGenerationBlock {
	int generationNumber;
	char * ecosystemName;
	char * regionName;
	StatementList * body;
};
 
 //every
struct EveryRandomBlock {
	char * ecosystemName;
	char * regionName;
	StatementList * body;
};
 

 //simulate
struct SimulateStatement {
	char * ecosystemName;
	int generations;
	int hasSeed;
	int seedValue;
};

 //if
struct IfStatement {
	Condition * condition;
	StatementList * thenBody;
	StatementList * elseBody;    // NULL if no else
};

 //while
struct WhileStatement {
	Condition * condition;
	StatementList * body;
};

 //foreach
struct ForEachStatement {
	char * speciesName;
	char * ecosystemName;
	char * regionName;
	StatementList * body;
};

 //log
struct LogStatement {
	LogType type;
	char * speciesName;      // for LOG_POPULATION
	char * ecosystemName;    // for LOG_POPULATION and LOG_STATE
	char * regionName;       // for LOG_POPULATION
};

 //program (root node)
struct Program {
	StatementList * statements;
};

 //destructors

void destroyProgram(Program * program);
void destroyStatementList(StatementList * list);
void destroyStatement(Statement * statement);
void destroyExpression(Expression * expression);
void destroyCondition(Condition * condition);
void destroySpeciesDefinition(SpeciesDefinition * species);
void destroySpeciesAttributeList(SpeciesAttributeList * list);
void destroyRegionDefinition(RegionDefinition * region);
void destroyEcosystemDefinition(EcosystemDefinition * ecosystem);
void destroyEcosystemMemberList(EcosystemMemberList * list);
void destroyOnEncounterBlock(OnEncounterBlock * block);
void destroyOnGenerationBlock(OnGenerationBlock * block);
void destroyEveryRandomBlock(EveryRandomBlock * block);
void destroySimulateStatement(SimulateStatement * stmt);
void destroyIfStatement(IfStatement * stmt);
void destroyWhileStatement(WhileStatement * stmt);
void destroyForEachStatement(ForEachStatement * stmt);
void destroyLogStatement(LogStatement * stmt);
void destroyAddStatement(AddStatement * stmt);
void destroyRemoveStatement(RemoveStatement * stmt);
void destroyMoveStatement(MoveStatement * stmt);
void destroyAttributeAssignment(AttributeAssignment * assignment);



#endif

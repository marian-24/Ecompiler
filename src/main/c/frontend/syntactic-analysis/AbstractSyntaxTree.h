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

/*typedef enum ExpressionType ExpressionType;
typedef enum FactorType FactorType;

typedef struct Constant Constant;
typedef struct Expression Expression;
typedef struct Factor Factor;
typedef struct Program Program;
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

//Ecosystem

//Population

//Encounter

//Simulation

//Control flow

//log

//expressions

//attribute modificaion


/**
 * Node types for the Abstract Syntax Tree (AST).
 */
//enums for node types
/*
enum ExpressionType {
	ADDITION,
	DIVISION,
	FACTOR,
	MULTIPLICATION,
	SUBTRACTION
};

enum FactorType {
	CONSTANT,
	EXPRESSION
};

struct Constant {
	int value;
};

struct Factor {
	union {
		Constant * constant;
		Expression * expression;
	};
	FactorType type;
};

struct Expression {
	union {
		Factor * factor;
		struct {
			Expression * leftExpression;
			Expression * rightExpression;
		};
	};
	ExpressionType type;
};

struct Program {
	Expression * expression;
};
*/

//expression node

/**
 * Node recursive super-duper-trambolik-destructors.
 */

 //condition node


 //range value

 //environmental

 //species

 //region

 //ecosystem

 //attribute

 //statement

 //on encounter

 //on generation
 
 //every

 //simulate

 //if

 //while

 //foreach

 //log

 //program (root node)

 //destructors

/*void destroyConstant(Constant * constant);
void destroyExpression(Expression * expression);
void destroyFactor(Factor * factor);
void destroyProgram(Program * program);
*/



#endif

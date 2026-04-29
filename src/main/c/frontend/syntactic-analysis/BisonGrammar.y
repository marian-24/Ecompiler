%{

#include "../../support/type/TokenLabel.h"
#include "AbstractSyntaxTree.h"
#include "BisonActions.h"

/**
 * The error reporting function for Bison parser.
 *
 * @todo Add location to the grammar and "pushToken" API function.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Error-Reporting-Function.html
 * @see https://www.gnu.org/software/bison/manual/html_node/Tracking-Locations.html
 */
void yyerror(const YYLTYPE * location, const char * message) {}

%}

// You touch this, and you die.
%define api.pure full
%define api.push-pull push
%define api.value.union.name SemanticValue
%define parse.error detailed
%locations

%union {
	/** Terminals. */

	signed int integer;
	double decimal;
	int boolean;
	char * string;
	TokenLabel token;

	/** Non-terminals. */
	Program * program;
	StatementList * statementList;
	Statement * statement;

	SpeciesDefinition * speciesDefinition;
	SpeciesAttributeList * speciesAttributeList;
	SpeciesAttribute * speciesAttribute;
	EnvTolerance * envTolerance;
	RangeValue * rangeValue;

	RegionDefinition * regionDefinition;

	EcosystemDefinition * ecosystemDefinition;
	EcosystemMemberList * ecosystemMemberList;
	EcosystemMember * ecosystemMember;

	AddStatement * addStatement;
	RemoveStatement * removeStatement;
	MoveStatement * moveStatement;
	AttributeAssignment * attributeAssignment;

	OnEncounterBlock * onEncounterBlock;
	OnGenerationBlock * onGenerationBlock;
	EveryRandomBlock * everyRandomBlock;

	SimulateStatement * simulateStatement;

	IfStatement * ifStatement;
	WhileStatement * whileStatement;
	ForEachStatement * forEachStatement;

	LogStatement * logStatement;

	Expression * expression;
	Condition * condition;

}

/**
 * Destructors. This functions are executed after the parsing ends, so if the
 * AST must be used in the following phases of the compiler you shouldn't used
 * this approach for the AST root node ("program" non-terminal, in this
 * grammar), or it will drop the entire tree even if the parsing succeeds.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Destructor-Decl.html
 */
/**
we do not add destructors for <program>, <statementList> or <statement>. 
adding them would drop the entire tree even on success!
*/
%destructor { destroyExpression($$); }     <expression>
%destructor { destroyCondition($$); }      <condition>
%destructor { free($$); }                  <string>

/** Terminals. */

/**literals*/
%token <integer> INTEGER
%token <decimal> FLOAT
%token <boolean> BOOLEAN
%token <string>  STRING
%token <string>  ID

/**keywords- domain*/
%token <token> SPECIES
%token <token> REGION
%token <token> ECOSYSTEM

/**keywords- species Definitions*/
%token <token> CREATE_SPECIES
%token <token> LIFESPAN
%token <token> REPRODUCTION_RATE
%token <token> SPEED
%token <token> REPRODUCTION_STRATEGY
%token <token> DIET
%token <token> ENVIRONMENTAL_TOLERANCE
%token <token> HABITAT
%token <token> ENERGY

/**Keywords- region*/

/**keywords- primitive types*/

/**keywords- population actions*/

/**keywords- Encounter*/

/** keyword- Simulation*/

/**k- control*/

/**k-Predefined behaviors */

/**output*/

/** ENUM*/
/**diet*/

/**habitat*/

/**liteerals*/

/**Arithmetic operators*/

/** Assignment operators */

/** Relational operators */

/** Logical operators */

/** Punctuation */

%token <token> IGNORED
%token <token> UNKNOWN

/** Non-terminals. */
%type <program>              program
%type <statementList>        statementList
%type <statement>            statement

/**
 * Precedence and associativity.
 *
 * @see https://en.cppreference.com/w/cpp/language/operator_precedence.html
 * @see https://www.gnu.org/software/bison/manual/html_node/Precedence.html
 */
%left OR
%left AND
%right NOT
%left LT GT EQ NEQ LTE GTE
%left ADD_OP SUB_OP
%left MUL_OP DIV_OP

%%

// IMPORTANT: To use λ in the following grammar, use the %empty symbol.

program: statementList
	{ $$ = ProgramSemanticAction($1); }
	;

statementList: statement
	{ $$ = StatementListSemanticAction($1, NULL); }
	| statementList statement
	{ $$ = StatementListSemanticAction($2, $1); }
	;

%%

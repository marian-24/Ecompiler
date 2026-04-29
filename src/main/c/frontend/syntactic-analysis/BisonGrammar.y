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
%token <token> HUMIDITY
%token <token> ALTITUDE
%token <token> CARRYING_CAPACITY

/**keywords- primitive types*/
%token <token> INT
%token <token> FLOAT_TYPE
%token <token> BOOLEAN_TYPE
%token <token> STRING_TYPE

/**keywords- population actions*/
%token <token> ADD
%token <token> TO
%token <token> REMOVE
%token <token> MOVE

/**keywords- Encounter*/
%token <token> ON
%token <token> ENCOUNTER
%token <token> WITH
%token <token> IN
%token <token> EVERY
%token <token> RANDOM
%token <token> GENERATION
%token <token> GENERATIONS

/** keyword- Simulation*/
%token <token> SIMULATE
%token <token> SEED

/**k- control*/
%token <token> IF
%token <token> ELSE
%token <token> WHILE
%token <token> FOR
%token <token> EACH

/**k-Predefined behaviors */
%token <token> HUNT
%token <token> FLEE
%token <token> IGNORE
%token <token> EAT
%token <token> DRINK
%token <token> HARVEST

/**output*/
%token <token> LOG
%token <token> POPULATION
%token <token> OF
%token <token> STATE

/** ENUM*/
/**Reproductive strategy */
%token <token> R_SELECTED
%token <token> K_SELECTED
%token <token> R_STRATEGY
%token <token> K_STRATEGY

/**diet*/
%token <token> HERBIVORE
%token <token> CARNIVORE
%token <token> OMNIVORE
%token <token> DECOMPOSER

/**habitat*/
%token <token> TERRESTRIAL
%token <token> AQUATIC
%token <token> AMPHIBIOUS
%token <token> MIXED

/**liteerals*/
%token <token> TRUE_VAL
%token <token> FALSE_VAL

/**Arithmetic operators*/
%token <token> ADD_OP
%token <token> SUB_OP
%token <token> MUL_OP
%token <token> DIV_OP

/** Assignment operators */
%token <token> ASSIGN
%token <token> ADD_ASSIGN
%token <token> SUB_ASSIGN
%token <token> MUL_ASSIGN
%token <token> DIV_ASSIGN

/** Relational operators */
%token <token> LT
%token <token> GT
%token <token> EQ
%token <token> NEQ
%token <token> LTE
%token <token> GTE

/** Logical operators */
%token <token> AND
%token <token> OR
%token <token> NOT

/** Punctuation */
%token <token> OPEN_BRACE
%token <token> CLOSE_BRACE
%token <token> OPEN_PAREN
%token <token> CLOSE_PAREN
%token <token> OPEN_BRACKET
%token <token> CLOSE_BRACKET
%token <token> SEMICOLON
%token <token> COMMA
%token <token> DOT

%token <token> UNKNOWN

/** Non-terminals. */
%type <program>              program
%type <statementList>        statementList
%type <statement>            statement

%type <speciesDefinition>    speciesDefinition
%type <speciesAttributeList> speciesAttributeList
%type <speciesAttribute>     speciesAttribute
%type <envTolerance>         envTolerance
%type <rangeValue>           rangeValue

%type <regionDefinition>     regionDefinition

%type <ecosystemDefinition>  ecosystemDefinition
%type <ecosystemMemberList>  ecosystemMemberList
%type <ecosystemMember>      ecosystemMember

%type <addStatement>         addStatement
%type <removeStatement>      removeStatement
%type <moveStatement>        moveStatement
%type <attributeAssignment>  attributeAssignment

%type <onEncounterBlock>     onEncounterBlock
%type <onGenerationBlock>    onGenerationBlock
%type <everyRandomBlock>     everyRandomBlock

%type <simulateStatement>    simulateStatement

%type <ifStatement>          ifStatement
%type <whileStatement>       whileStatement
%type <forEachStatement>     forEachStatement

%type <logStatement>         logStatement

%type <expression>           expression
%type <condition>            condition

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

statement: speciesDefinition
	{ $$ = SpeciesStatementSemanticAction($1); }
	| regionDefinition
	{ $$ = RegionStatementSemanticAction($1); }
	| ecosystemDefinition
	{ $$ = EcosystemStatementSemanticAction($1); }
	| addStatement
	{ $$ = AddStatementWrapperSemanticAction($1); }
	| removeStatement
	{ $$ = RemoveStatementWrapperSemanticAction($1); }
	| moveStatement
	{ $$ = MoveStatementWrapperSemanticAction($1); }
	| onEncounterBlock
	{ $$ = OnEncounterStatementSemanticAction($1); }
	| onGenerationBlock
	{ $$ = OnGenerationStatementSemanticAction($1); }
	| everyRandomBlock
	{ $$ = EveryRandomStatementSemanticAction($1); }
	| simulateStatement
	{ $$ = SimulateStatementWrapperSemanticAction($1); }
	| ifStatement
	{ $$ = IfStatementWrapperSemanticAction($1); }
	| whileStatement
	{ $$ = WhileStatementWrapperSemanticAction($1); }
	| forEachStatement
	{ $$ = ForEachStatementWrapperSemanticAction($1); }
	| logStatement
	{ $$ = LogStatementWrapperSemanticAction($1); }
	| attributeAssignment
	{ $$ = AttributeAssignmentStatementSemanticAction($1); }
	;

/**species*/
speciesDefinition: SPECIES ID[name] ASSIGN CREATE_SPECIES OPEN_BRACE speciesAttributeList CLOSE_BRACE
	{ $$ = SpeciesDefinitionSemanticAction($name, $6); }
	;

speciesAttributeList: speciesAttribute
	{ $$ = SpeciesAttributeListSemanticAction($1, NULL); }
	| speciesAttributeList speciesAttribute
	{ $$ = SpeciesAttributeListSemanticAction($2, $1); }
	;

speciesAttribute: INT LIFESPAN ASSIGN INTEGER SEMICOLON
	{ $$ = LifespanAttributeSemanticAction($4); }
	| FLOAT_TYPE REPRODUCTION_RATE ASSIGN FLOAT SEMICOLON
	{ $$ = ReproductionRateAttributeSemanticAction($4); }
	| INT SPEED ASSIGN INTEGER SEMICOLON
	{ $$ = SpeedAttributeSemanticAction($4); }
	| REPRODUCTION_STRATEGY ID[name] ASSIGN REPRODUCTION_STRATEGY DOT reproductiveStrategyValue SEMICOLON
	{ $$ = ReproductiveStrategyAttributeSemanticAction($6); }
	| DIET ID[name] ASSIGN DIET DOT dietValue SEMICOLON
	{ $$ = DietAttributeSemanticAction($6); }
	| HABITAT ID[name] ASSIGN HABITAT DOT habitatSpeciesValue SEMICOLON
	{ $$ = HabitatAttributeSemanticAction($6); }
	| ENVIRONMENTAL_TOLERANCE ID[name] SEMICOLON envTolerance
	{ $$ = EnvToleranceAttributeSemanticAction($4); }
	;

reproductiveStrategyValue: R_SELECTED   { $$ = R_SELECTED; }
	| K_SELECTED                        { $$ = K_SELECTED; }
	| R_STRATEGY                        { $$ = R_STRATEGY; }
	| K_STRATEGY                        { $$ = K_STRATEGY; }
	;

dietValue: HERBIVORE    { $$ = HERBIVORE; }
	| CARNIVORE         { $$ = CARNIVORE; }
	| OMNIVORE          { $$ = OMNIVORE; }
	| DECOMPOSER        { $$ = DECOMPOSER; }
	;

habitatSpeciesValue: TERRESTRIAL    { $$ = TERRESTRIAL; }
	| AQUATIC                       { $$ = AQUATIC; }
	| AMPHIBIOUS                    { $$ = AMPHIBIOUS; }
	;

envTolerance: ID[name] DOT TEMPERATURE ASSIGN rangeValue SEMICOLON
              ID[name2] DOT HUMIDITY ASSIGN rangeValue SEMICOLON
              ID[name3] DOT ALTITUDE ASSIGN rangeValue SEMICOLON
	{ $$ = EnvToleranceSemanticAction($5, $11, $17); }
	;

rangeValue: OPEN_BRACKET INTEGER[min] COMMA INTEGER[max] CLOSE_BRACKET
	{ $$ = RangeValueSemanticAction($min, $max); }
	;

regionDefinition: REGION ID[name] OPEN_BRACE
                      TEMPERATURE ASSIGN INTEGER[temp] SEMICOLON
                      HUMIDITY ASSIGN INTEGER[hum] SEMICOLON
                      ALTITUDE ASSIGN INTEGER[alt] SEMICOLON
                      CARRYING_CAPACITY ASSIGN INTEGER[cap] SEMICOLON
                      HABITAT ASSIGN habitatRegionValue[hab] SEMICOLON
                  CLOSE_BRACE
	{ $$ = RegionDefinitionSemanticAction($name, $temp, $hum, $alt, $cap, $hab); }
	;

habitatRegionValue: TERRESTRIAL { $$ = TERRESTRIAL; }
	| AQUATIC                   { $$ = AQUATIC; }
	| MIXED                     { $$ = MIXED; }
	;

/**ecosystem*/
ecosystemDefinition: ECOSYSTEM ID[name] OPEN_BRACE ecosystemMemberList CLOSE_BRACE
	{ $$ = EcosystemDefinitionSemanticAction($name, $4); }
	;

ecosystemMemberList: ecosystemMember
	{ $$ = EcosystemMemberListSemanticAction($1, NULL); }
	| ecosystemMemberList ecosystemMember
	{ $$ = EcosystemMemberListSemanticAction($2, $1); }
	;

ecosystemMember: REGION ID[name] SEMICOLON
	{ $$ = EcosystemMemberSemanticAction(MEMBER_REGION, $name); }
	| SPECIES ID[name] SEMICOLON
	{ $$ = EcosystemMemberSemanticAction(MEMBER_SPECIES, $name); }
	;

addStatement: ADD INTEGER[amount] ID[species] TO ID[ecosystem] DOT ID[region] SEMICOLON
	{ $$ = AddStatementSemanticAction($amount, $species, $ecosystem, $region); }
	;

removeStatement: REMOVE ID[species] SEMICOLON
	{ $$ = RemoveStatementSemanticAction($species); }
	;

moveStatement: MOVE ID[species] TO ID[ecosystem] DOT ID[region] SEMICOLON
	{ $$ = MoveStatementSemanticAction($species, $ecosystem, $region); }
	;

/**attribute*/
attributeAssignment: ID[object] DOT ID[attr] ASSIGN expression SEMICOLON
	{ $$ = AttributeAssignmentSemanticAction($object, $attr, ASSIGN_SIMPLE, $5); }
	| ID[object] DOT ID[attr] ADD_ASSIGN expression SEMICOLON
	{ $$ = AttributeAssignmentSemanticAction($object, $attr, ASSIGN_ADD, $5); }
	| ID[object] DOT ID[attr] SUB_ASSIGN expression SEMICOLON
	{ $$ = AttributeAssignmentSemanticAction($object, $attr, ASSIGN_SUB, $5); }
	| ID[object] DOT ID[attr] MUL_ASSIGN expression SEMICOLON
	{ $$ = AttributeAssignmentSemanticAction($object, $attr, ASSIGN_MUL, $5); }
	| ID[object] DOT ID[attr] DIV_ASSIGN expression SEMICOLON
	{ $$ = AttributeAssignmentSemanticAction($object, $attr, ASSIGN_DIV, $5); }
	;

onEncounterBlock: ON ENCOUNTER ID[speciesA] WITH ID[speciesB] IN ID[ecosystem] DOT ID[region] OPEN_BRACE statementList CLOSE_BRACE
	{ $$ = OnEncounterBlockSemanticAction($speciesA, $speciesB, $ecosystem, $region, $12); }
	;

onGenerationBlock: ON GENERATION INTEGER[gen] IN ID[ecosystem] DOT ID[region] OPEN_BRACE statementList CLOSE_BRACE
	{ $$ = OnGenerationBlockSemanticAction($gen, $ecosystem, $region, $10); }
	;

everyRandomBlock: EVERY RANDOM GENERATIONS IN ID[ecosystem] DOT ID[region] OPEN_BRACE statementList CLOSE_BRACE
	{ $$ = EveryRandomBlockSemanticAction($ecosystem, $region, $10); }
	;

simulateStatement: SIMULATE ID[ecosystem] FOR INTEGER[gens] GENERATIONS SEMICOLON
	{ $$ = SimulateStatementSemanticAction($ecosystem, $gens, 0, 0); }
	| SIMULATE ID[ecosystem] FOR INTEGER[gens] GENERATIONS WITH SEED INTEGER[seed] SEMICOLON
	{ $$ = SimulateStatementSemanticAction($ecosystem, $gens, 1, $seed); }
	;

ifStatement: IF OPEN_PAREN condition CLOSE_PAREN OPEN_BRACE statementList CLOSE_BRACE
	{ $$ = IfStatementSemanticAction($3, $6, NULL); }
	| IF OPEN_PAREN condition CLOSE_PAREN OPEN_BRACE statementList CLOSE_BRACE ELSE OPEN_BRACE statementList CLOSE_BRACE
	{ $$ = IfStatementSemanticAction($3, $6, $10); }
	;

whileStatement: WHILE OPEN_PAREN condition CLOSE_PAREN OPEN_BRACE statementList CLOSE_BRACE
	{ $$ = WhileStatementSemanticAction($3, $6); }
	;

forEachStatement: FOR EACH ID[species] IN ID[ecosystem] DOT ID[region] OPEN_BRACE statementList CLOSE_BRACE
	{ $$ = ForEachStatementSemanticAction($species, $ecosystem, $region, $10); }
	;

logStatement: LOG POPULATION OF ID[species] IN ID[ecosystem] DOT ID[region] SEMICOLON
	{ $$ = LogPopulationSemanticAction($species, $ecosystem, $region); }
	| LOG STATE OF ID[ecosystem] SEMICOLON
	{ $$ = LogStateSemanticAction($ecosystem); }
	| LOG GENERATION SEMICOLON
	{ $$ = LogGenerationSemanticAction(); }
	;

expression: INTEGER
	{ $$ = IntegerExpressionSemanticAction($1); }
	| FLOAT
	{ $$ = FloatExpressionSemanticAction($1); }
	| TRUE_VAL
	{ $$ = BooleanExpressionSemanticAction(1); }
	| FALSE_VAL
	{ $$ = BooleanExpressionSemanticAction(0); }
	| STRING
	{ $$ = StringExpressionSemanticAction($1); }
	| ID
	{ $$ = IdentifierExpressionSemanticAction($1); }
	| ID[object] DOT ID[attr]
	{ $$ = AttributeAccessExpressionSemanticAction($object, $attr); }
	| ID[ecosystem] DOT ID[region]
	{ $$ = EcosystemAccessExpressionSemanticAction($ecosystem, $region); }
	| POPULATION OF ID[species] IN ID[ecosystem] DOT ID[region]
	{ $$ = PopulationOfExpressionSemanticAction($species, $ecosystem, $region); }
	| RANDOM OPEN_BRACKET INTEGER[min] COMMA INTEGER[max] CLOSE_BRACKET
	{ $$ = RandomRangeExpressionSemanticAction($min, $max); }
	| expression[left] ADD_OP expression[right]
	{ $$ = BinaryExpressionSemanticAction($left, $right, EXPR_ADD); }
	| expression[left] SUB_OP expression[right]
	{ $$ = BinaryExpressionSemanticAction($left, $right, EXPR_SUB); }
	| expression[left] MUL_OP expression[right]
	{ $$ = BinaryExpressionSemanticAction($left, $right, EXPR_MUL); }
	| expression[left] DIV_OP expression[right]
	{ $$ = BinaryExpressionSemanticAction($left, $right, EXPR_DIV); }
	| OPEN_PAREN expression CLOSE_PAREN
	{ $$ = $2; }
	;

/** Conditions*/

condition: expression[left] LT expression[right]
	{ $$ = BinaryConditionSemanticAction($left, $right, COND_LT); }
	| expression[left] GT expression[right]
	{ $$ = BinaryConditionSemanticAction($left, $right, COND_GT); }
	| expression[left] EQ expression[right]
	{ $$ = BinaryConditionSemanticAction($left, $right, COND_EQ); }
	| expression[left] NEQ expression[right]
	{ $$ = BinaryConditionSemanticAction($left, $right, COND_NEQ); }
	| expression[left] LTE expression[right]
	{ $$ = BinaryConditionSemanticAction($left, $right, COND_LTE); }
	| expression[left] GTE expression[right]
	{ $$ = BinaryConditionSemanticAction($left, $right, COND_GTE); }
	| condition[left] AND condition[right]
	{ $$ = LogicalConditionSemanticAction($left, $right, COND_AND); }
	| condition[left] OR condition[right]
	{ $$ = LogicalConditionSemanticAction($left, $right, COND_OR); }
	| NOT condition[operand]
	{ $$ = NotConditionSemanticAction($operand); }
	| ID[species] IN ID[ecosystem] DOT ID[region]
	{ $$ = InConditionSemanticAction($species, $ecosystem, $region); }
	| OPEN_PAREN condition CLOSE_PAREN
	{ $$ = $2; }
	;


%%

#ifndef BISON_ACTIONS_HEADER
#define BISON_ACTIONS_HEADER

#include "../../support/logging/Logger.h"
#include "../../support/type/CompilerState.h"
#include "../../support/type/ModuleDestructor.h"
#include "../../support/type/TokenLabel.h"
#include "AbstractSyntaxTree.h"
#include "BisonParser.h"
#include <stdlib.h>
#include <string.h>

/** Initialize module's internal state. */
ModuleDestructor initializeBisonActionsModule(CompilerState * compilerState);


 // PROGRAM & STATEMENT LIST

Program *       ProgramSemanticAction(StatementList * statements);
StatementList * StatementListSemanticAction(Statement * statement, StatementList * next);

 // STATEMENT WRAPPERS

Statement * SpeciesStatementSemanticAction(SpeciesDefinition * def);
Statement * RegionStatementSemanticAction(RegionDefinition * def);
Statement * EcosystemStatementSemanticAction(EcosystemDefinition * def);
Statement * AddStatementWrapperSemanticAction(AddStatement * add);
Statement * RemoveStatementWrapperSemanticAction(RemoveStatement * rem);
Statement * MoveStatementWrapperSemanticAction(MoveStatement * move);
Statement * OnEncounterStatementSemanticAction(OnEncounterBlock * block);
Statement * OnGenerationStatementSemanticAction(OnGenerationBlock * block);
Statement * EveryRandomStatementSemanticAction(EveryRandomBlock * block);
Statement * SimulateStatementWrapperSemanticAction(SimulateStatement * sim);
Statement * IfStatementWrapperSemanticAction(IfStatement * ifStmt);
Statement * WhileStatementWrapperSemanticAction(WhileStatement * whileStmt);
Statement * ForEachStatementWrapperSemanticAction(ForEachStatement * forStmt);
Statement * LogStatementWrapperSemanticAction(LogStatement * logStmt);
Statement * AttributeAssignmentStatementSemanticAction(AttributeAssignment * assignment);
Statement * BehaviorStatementSemanticAction(BehaviorType type);

 
// VARIABLE DECLARATIONS

Statement * IntDeclarationSemanticAction(char * name, Expression * value);
Statement * FloatDeclarationSemanticAction(char * name, Expression * value);
Statement * StringDeclarationSemanticAction(char * name, Expression * value);
Statement * BooleanDeclarationSemanticAction(char * name, Expression * value);

 
// SPECIES

SpeciesDefinition *    SpeciesDefinitionSemanticAction(char * name, SpeciesAttributeList * attributes);
SpeciesAttributeList * SpeciesAttributeListSemanticAction(SpeciesAttribute * attribute, SpeciesAttributeList * next);
SpeciesAttribute *     LifespanAttributeSemanticAction(int value);
SpeciesAttribute *     ReproductionRateAttributeSemanticAction(double value);
SpeciesAttribute *     SpeedAttributeSemanticAction(int value);
SpeciesAttribute *     ReproductiveStrategyAttributeSemanticAction(TokenLabel strategy);
SpeciesAttribute *     DietAttributeSemanticAction(TokenLabel diet);
SpeciesAttribute *     HabitatAttributeSemanticAction(TokenLabel habitat);
SpeciesAttribute *     EnvToleranceAttributeSemanticAction(EnvTolerance * tolerance);
EnvTolerance *         EnvToleranceSemanticAction(RangeValue * temperature, RangeValue * humidity, RangeValue * altitude);
RangeValue *           RangeValueSemanticAction(int min, int max);

 // REGION
    
RegionDefinition * RegionDefinitionSemanticAction(char * name, int temperature, int humidity,
                                                   int altitude, int carryingCapacity, TokenLabel habitat);

 // ECOSYSTEM

EcosystemDefinition * EcosystemDefinitionSemanticAction(char * name, EcosystemMemberList * members);
EcosystemMemberList * EcosystemMemberListSemanticAction(EcosystemMember * member, EcosystemMemberList * next);
EcosystemMember *     EcosystemMemberSemanticAction(EcosystemMemberType type, char * name);

 // POPULATION STATEMENTS

AddStatement *    AddStatementSemanticAction(int amount, char * speciesName, char * ecosystemName, char * regionName);
RemoveStatement * RemoveStatementSemanticAction(char * speciesName);
MoveStatement *   MoveStatementSemanticAction(char * speciesName, char * ecosystemName, char * regionName);

 // ATTRIBUTE ASSIGNMENT

AttributeAssignment * AttributeAssignmentSemanticAction(char * objectName, char * attributeName,
                                                          AssignmentOperatorType op, Expression * value);

 // EVENT BLOCKS

OnEncounterBlock *  OnEncounterBlockSemanticAction(char * speciesA, char * speciesB,
                                                    char * ecosystemName, char * regionName,
                                                    StatementList * body);
OnGenerationBlock * OnGenerationBlockSemanticAction(int generationNumber, char * ecosystemName,
                                                      char * regionName, StatementList * body);
EveryRandomBlock *  EveryRandomBlockSemanticAction(char * ecosystemName, char * regionName,
                                                    StatementList * body);

 // SIMULATE

SimulateStatement * SimulateStatementSemanticAction(char * ecosystemName, int generations,
                                                      int hasSeed, int seedValue);

 // CONTROL FLOW

IfStatement *      IfStatementSemanticAction(Condition * condition, StatementList * thenBody,
                                              StatementList * elseBody);
WhileStatement *   WhileStatementSemanticAction(Condition * condition, StatementList * body);
ForEachStatement * ForEachStatementSemanticAction(char * speciesName, char * ecosystemName,
                                                   char * regionName, StatementList * body);

 // LOG

LogStatement * LogPopulationSemanticAction(char * speciesName, char * ecosystemName, char * regionName);
LogStatement * LogStateSemanticAction(char * ecosystemName);
LogStatement * LogGenerationSemanticAction(void);

 // EXPRESSIONS

Expression * IntegerExpressionSemanticAction(int value);
Expression * FloatExpressionSemanticAction(double value);
Expression * BooleanExpressionSemanticAction(int value);
Expression * StringExpressionSemanticAction(char * value);
Expression * IdentifierExpressionSemanticAction(char * name);
Expression * AttributeAccessExpressionSemanticAction(char * objectName, char * attributeName);
Expression * PopulationOfExpressionSemanticAction(char * speciesName, char * ecosystemName, char * regionName);
Expression * RandomRangeExpressionSemanticAction(int min, int max);
Expression * BinaryExpressionSemanticAction(Expression * left, Expression * right, ExpressionType type);

// CONDITIONS
Condition * BinaryConditionSemanticAction(Expression * left, Expression * right, ConditionType type);
Condition * LogicalConditionSemanticAction(Condition * left, Condition * right, ConditionType type);
Condition * NotConditionSemanticAction(Condition * operand);
Condition * InConditionSemanticAction(char * speciesName, char * ecosystemName, char * regionName);


Constant *   IntegerConstantSemanticAction(const int value);
Expression * ArithmeticExpressionSemanticAction(Expression * leftExpression, Expression * rightExpression, ExpressionType type);
Expression * FactorExpressionSemanticAction(Factor * factor);
Factor *     ConstantFactorSemanticAction(Constant * constant);
Factor *     ExpressionFactorSemanticAction(Expression * expression);
Program *    ExpressionProgramSemanticAction(Expression * expression);

#endif
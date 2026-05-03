%top{

#include "../../support/type/CompilationStatus.h"
#include "../../support/type/FlexContext.h"
#include "../../support/type/TokenLabel.h"
#include "../syntactic-analysis/AbstractSyntaxTree.h"
#include "../syntactic-analysis/BisonParser.h"

}

%{

/*
 * @see https://westes.github.io/flex/manual/Definitions-Section.html#Definitions-Section
 */

/*
extern CompilationStatus EnterImportExpressionLexemeAction(FlexContext context);
extern CompilationStatus LeaveImportExpressionLexemeAction();
extern CompilationStatus ParenthesisLexemeAction(TokenLabel label);
extern CompilationStatus SubexpressionLexemeAction();
*/

/** Keywords - Tipos de dominio*/
extern CompilationStatus KeywordLexemeAction(TokenLabel label);

/** Enum values */
extern CompilationStatus EnumValueLexemeAction(TokenLabel label);

/** Operators */
extern CompilationStatus ArithmeticOperatorLexemeAction(TokenLabel label);
extern CompilationStatus AssignmentOperatorLexemeAction(TokenLabel label);
extern CompilationStatus RelationalOperatorLexemeAction(TokenLabel label);
extern CompilationStatus LogicalOperatorLexemeAction(TokenLabel label);

/** Punctuation */
extern CompilationStatus PunctuationLexemeAction(TokenLabel label);

/** Literals */
extern CompilationStatus IntegerLexemeAction();
extern CompilationStatus FloatLexemeAction();
extern CompilationStatus BooleanLexemeAction();
extern CompilationStatus StringLexemeAction();
extern CompilationStatus IdentifierLexemeAction();

/** Comments */
extern CompilationStatus EnterMultilineCommentLexemeAction(FlexContext context);
extern CompilationStatus LeaveMultilineCommentLexemeAction();
extern CompilationStatus IgnoredLexemeAction();

/** Control */
extern CompilationStatus EOFLexemeAction();
extern CompilationStatus UnknownLexemeAction();


%}

/**
 * Flex contexts (a.k.a. start conditions).
 *
 * @see https://westes.github.io/flex/manual/Start-Conditions.html
 */
/*%x IMPORT_EXPRESSION*/
%x MULTILINE_COMMENT

/**
 * Reusable patterns.
 *
 * @see https://westes.github.io/flex/manual/Matching.html
 * @see https://westes.github.io/flex/manual/Patterns.html
 */
whitespace			[[:space:]]+
digit               [[:digit:]]
letter              [[:alpha:]]
alphanumeric        [[:alnum:]_]
integer             {digit}+
float               {digit}+"."{digit}+
identifier          {letter}{alphanumeric}*
string              \"[^\"]*\"
%%


"/*"								{ return EnterMultilineCommentLexemeAction(MULTILINE_COMMENT); }
<MULTILINE_COMMENT>"*/"				{ return LeaveMultilineCommentLexemeAction(); }
<MULTILINE_COMMENT>[^*]+			{ return IgnoredLexemeAction(); }
<MULTILINE_COMMENT>.				{ return IgnoredLexemeAction(); }
"//"[^\n]*                          { return IgnoredLexemeAction(); }

    /** keywords - dominios*/
"species"                           { return KeywordLexemeAction(SPECIES); }
"region"                            { return KeywordLexemeAction(REGION); }
"ecosystem"                         { return KeywordLexemeAction(ECOSYSTEM); }

    /**Keywords - Atributos de especies*/
"createSpecies"                     { return KeywordLexemeAction(CREATE_SPECIES); }
"lifespan"                          { return KeywordLexemeAction(LIFESPAN); }
"reproductionRate"                  { return KeywordLexemeAction(REPRODUCTION_RATE); }
"speed"                             { return KeywordLexemeAction(SPEED); }
"reproductionStrategy"              { return KeywordLexemeAction(REPRODUCTION_STRATEGY); }
"diet"                              { return KeywordLexemeAction(DIET); }
"habitat"                           { return KeywordLexemeAction(HABITAT); }
"environmentalTolerance"            { return KeywordLexemeAction(ENVIRONMENTAL_TOLERANCE); }
"energy"                            { return KeywordLexemeAction(ENERGY); }

    /**Keywords - Atributos de regiones */
"temperature"                       { return KeywordLexemeAction(TEMPERATURE); }
"humidity"                          { return KeywordLexemeAction(HUMIDITY); }
"altitude"                          { return KeywordLexemeAction(ALTITUDE); }
"carryingCapacity"                  { return KeywordLexemeAction(CARRYING_CAPACITY); }

    /**Keywords Tipos primitivos*/
"int"                               { return KeywordLexemeAction(INT_TYPE); }
"float"                             { return KeywordLexemeAction(FLOAT_TYPE); }
"boolean"                           { return KeywordLexemeAction(BOOLEAN_TYPE); }
"string"                            { return KeywordLexemeAction(STRING_TYPE); }

    /**Keywords - acciones*/
"add"                               { return KeywordLexemeAction(ADD); }
"to"                                { return KeywordLexemeAction(TO); }
"remove"                            { return KeywordLexemeAction(REMOVE); }
"move"                              { return KeywordLexemeAction(MOVE); }

    /**Keywords - encuentros*/
"on"                                { return KeywordLexemeAction(ON); }
"encounter"                         { return KeywordLexemeAction(ENCOUNTER); }
"with"                              { return KeywordLexemeAction(WITH); }
"in"                                { return KeywordLexemeAction(IN); }
"every"                             { return KeywordLexemeAction(EVERY); }
"random"                            { return KeywordLexemeAction(RANDOM); }
"generation"                        { return KeywordLexemeAction(GENERATION); }
"generations"                       { return KeywordLexemeAction(GENERATIONS); }

    /**Keywords - simulaciones*/
"simulate"                          { return KeywordLexemeAction(SIMULATE); }
"seed"                              { return KeywordLexemeAction(SEED); }

    /**Keywords - condicionales e iteradores*/
"if"                                { return KeywordLexemeAction(IF); }
"else"                              { return KeywordLexemeAction(ELSE); }
"while"                             { return KeywordLexemeAction(WHILE); }
"for"                               { return KeywordLexemeAction(FOR); }
"each"                              { return KeywordLexemeAction(EACH); }

    /**Keywords - comportamiento predefinido*/
"hunt"                              { return KeywordLexemeAction(HUNT); }
"flee"                              { return KeywordLexemeAction(FLEE); }
"ignore"                            { return KeywordLexemeAction(IGNORE); }
"eat"                               { return KeywordLexemeAction(EAT); }
"drink"                             { return KeywordLexemeAction(DRINK); }
"harvest"                           { return KeywordLexemeAction(HARVEST); }

    /**Keywords - output*/
"log"                               { return KeywordLexemeAction(LOG); }
"population"                        { return KeywordLexemeAction(POPULATION); }
"of"                                { return KeywordLexemeAction(OF); }
"state"                             { return KeywordLexemeAction(STATE); }

    /**enum*/
"R_SELECTED"                        { return EnumValueLexemeAction(R_SELECTED); }
"K_SELECTED"                        { return EnumValueLexemeAction(K_SELECTED); }
"R_STRATEGY"                        { return EnumValueLexemeAction(R_STRATEGY); }
"K_STRATEGY"                        { return EnumValueLexemeAction(K_STRATEGY); }
"HERBIVORE"                         { return EnumValueLexemeAction(HERBIVORE); }
"CARNIVORE"                         { return EnumValueLexemeAction(CARNIVORE); }
"OMNIVORE"                          { return EnumValueLexemeAction(OMNIVORE); }
"DECOMPOSER"                        { return EnumValueLexemeAction(DECOMPOSER); }
"TERRESTRIAL"                       { return EnumValueLexemeAction(TERRESTRIAL); }
"AQUATIC"                           { return EnumValueLexemeAction(AQUATIC); }
"AMPHIBIOUS"                        { return EnumValueLexemeAction(AMPHIBIOUS); }
"MIXED"                             { return EnumValueLexemeAction(MIXED); }

    /**booleanos*/
"true"                              { return BooleanLexemeAction(); }
"false"                             { return BooleanLexemeAction(); }

        /**operadores, primero los de 2 caracteres!*/
"+="                                { return AssignmentOperatorLexemeAction(ADD_ASSIGN); }
"-="                                { return AssignmentOperatorLexemeAction(SUB_ASSIGN); }
"*="                                { return AssignmentOperatorLexemeAction(MUL_ASSIGN); }
"/="                                { return AssignmentOperatorLexemeAction(DIV_ASSIGN); }
"="                                 { return AssignmentOperatorLexemeAction(ASSIGN); }
    /**operadores relacionales*/
"<="                                { return RelationalOperatorLexemeAction(LTE); }
">="                                { return RelationalOperatorLexemeAction(GTE); }
"=="                                { return RelationalOperatorLexemeAction(EQ); }
"!="                                { return RelationalOperatorLexemeAction(NEQ); }

    /**operadores, ahora los de 1 caracter*/
"<"                                 { return RelationalOperatorLexemeAction(LT); }
">"                                 { return RelationalOperatorLexemeAction(GT); }
"AND"                               { return LogicalOperatorLexemeAction(AND); }
"OR"                                { return LogicalOperatorLexemeAction(OR); }
"NOT"                               { return LogicalOperatorLexemeAction(NOT); }
"-"									{ return ArithmeticOperatorLexemeAction(SUB_OP); }
"*"									{ return ArithmeticOperatorLexemeAction(MUL_OP); }
"/"									{ return ArithmeticOperatorLexemeAction(DIV_OP); }
"+"									{ return ArithmeticOperatorLexemeAction(ADD_OP); }
    /**puntuacion*/
"{"                                 { return PunctuationLexemeAction(OPEN_BRACE); }
"}"                                 { return PunctuationLexemeAction(CLOSE_BRACE); }
"("                                 { return PunctuationLexemeAction(OPEN_PAREN); }
")"                                 { return PunctuationLexemeAction(CLOSE_PAREN); }
"["                                 { return PunctuationLexemeAction(OPEN_BRACKET); }
"]"                                 { return PunctuationLexemeAction(CLOSE_BRACKET); }
";"                                 { return PunctuationLexemeAction(SEMICOLON); }
","                                 { return PunctuationLexemeAction(COMMA); }
"."                                 { return PunctuationLexemeAction(DOT); }


    /**literals*/
{float}							    { return FloatLexemeAction(); }
{string}							{ return StringLexemeAction(); }
{integer}							{ return IntegerLexemeAction(); }

    /**Identifiers*/
{identifier}                        {return IdentifierLexemeAction();}

    /**Whitespace and unknown*/
{whitespace}						{ return IgnoredLexemeAction(); }
.									{ return UnknownLexemeAction(); }

<<EOF>>								{ return EOFLexemeAction(); }

%%

/***
 * @see https://westes.github.io/flex/manual/User-Code-Section.html#User-Code-Section
 */

#include "FlexActions.h"
#include "FlexExport.h"

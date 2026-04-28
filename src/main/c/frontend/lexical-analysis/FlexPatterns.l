%top{

#include "../../support/type/CompilationStatus.h"
#include "../../support/type/FlexContext.h"
#include "../../support/type/TokenLabel.h"
#include "../syntactic-analysis/AbstractSyntaxTree.h"
#include "../syntactic-analysis/BisonParser.h"

}

%{

/**
 * @see https://westes.github.io/flex/manual/Definitions-Section.html#Definitions-Section
 */

/*
extern CompilationStatus EnterImportExpressionLexemeAction(FlexContext context);
extern CompilationStatus LeaveImportExpressionLexemeAction();
extern CompilationStatus ParenthesisLexemeAction(TokenLabel label);
extern CompilationStatus SubexpressionLexemeAction();
*/

/** Keywords */
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

%{

/**
 * @see https://westes.github.io/flex/manual/Rules-Section.html#Rules-Section
 */

%}
/* comments*/

//"{"									{ return EnterImportExpressionLexemeAction(IMPORT_EXPRESSION); }
//<IMPORT_EXPRESSION>[^{}]+			{ return SubexpressionLexemeAction(); }
//<IMPORT_EXPRESSION>"}"				{ return LeaveImportExpressionLexemeAction(); }
//<IMPORT_EXPRESSION>.				{ return UnknownLexemeAction(); }

"/*"								{ return EnterMultilineCommentLexemeAction(MULTILINE_COMMENT); }
<MULTILINE_COMMENT>"*/"				{ return LeaveMultilineCommentLexemeAction(); }
<MULTILINE_COMMENT>[^*]+			{ return IgnoredLexemeAction(); }
<MULTILINE_COMMENT>.				{ return IgnoredLexemeAction(); }
"//"[^\n]*                          { return IgnoredLexemeAction(); }

/*Keywords - Domain types*/

/*Keywords - Species attributes*/

/*Keywords - Region attributes*/

/*Keywords - Primitive types*/

/*Keywords - Actions*/

/*Keywords - Encounter*/

/*Keywords - Simulation*/

/*Keywords - Control flow*/

/*Keywords - Control flow*/

/*Keywords - Output*/

/*Enum values*/

/*Logical operators*/

/*Assignment operators*/

/*Relational operators*/


/*Arithmetic operators*/
"-"									{ return ArithmeticOperatorLexemeAction(SUB); }
"*"									{ return ArithmeticOperatorLexemeAction(MUL); }
"/"									{ return ArithmeticOperatorLexemeAction(DIV); }
"+"									{ return ArithmeticOperatorLexemeAction(ADD); }

/*Punctuation*/
"("									{ return ParenthesisLexemeAction(OPEN_PARENTHESIS); }
")"									{ return ParenthesisLexemeAction(CLOSE_PARENTHESIS); }

/*literals*/
[[:digit:]]+						{ return IntegerLexemeAction(); }

/*Identifiers*/
{identifier}                        {return IdentifierLexemeAction();}

/*Whitespace and unknown*/
{whitespace}						{ return IgnoredLexemeAction(); }
.									{ return UnknownLexemeAction(); }

<<EOF>>								{ return EOFLexemeAction(); }

%%

/**
 * @see https://westes.github.io/flex/manual/User-Code-Section.html#User-Code-Section
 */

#include "FlexActions.h"
#include "FlexExport.h"

#ifndef COMPILER_STATE_HEADER
#define COMPILER_STATE_HEADER

#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"

struct SymbolTable;

/**
 * The global state of the compiler. Should transport every data structure
 * needed across the different phases of a compilation.
 */
typedef struct CompilerState {
	/**
	 * The root node of the AST.
	 */
	Program * abstractSyntaxtTree;

	/**
	 * You should change or remove this field, or a random child will die, and it
	 * will be your fault. !!
	 *
	 * Set to false by the semantic analysis phase if any error is found.
	 */
	int succeeded;

	/**
	 * Symbol table built by the semantic analysis phase.
	 * Contains all species, regions, and ecosystems definitions.
	 */
	struct SymbolTable * symbolTable;

	// TODO: Add an stack to handle nested scopes.
	// TODO: Add more configuration.
	// TODO: Add whatever you need.
	// TODO: ...
} CompilerState;

#endif

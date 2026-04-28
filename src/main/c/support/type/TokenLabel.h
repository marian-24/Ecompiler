#ifndef TOKEN_LABEL_HEADER
#define TOKEN_LABEL_HEADER

/**
 * The type of a Bison token label, that is, an identifier of a token, that
 * transports the lexeme and semantic value as a whole.
 */
//typedef signed int TokenLabel;
typedef enum TokenLabel{
    //keywords- domain types
    SPECIES,
    REGION,
    ECOSYSTEM,
    //KEYWORDS - SPECIES DEFINITION
    CREATE_SPECIES,
    LIFESPAN,
    REPRODUCTION_RATE,
    SPEED,
    REPRODUCTION_STRATEGY,
    DIET,
    ENVIRONMENTAL_TOLERANCE,
    HABITAT,
    ENERGY,
    //KEYWORDS- REGION ATTRIBUTES
    //TEMPERATURE,
    HUMIDITY,
    ALTITUDE,
    CARRYNG_CAPACITY,
    //KEYWORDS- PRIMITIVE TYPES
    INT,
    FLOAT_TYPE,
    BOOLEAN_TYPE,
    STRING_TYPE,
    //KEYWORDS- POPULATION ACTIONS
    ADD,
    TO,
    REMOVE,
    MOVE,
    //KEYWORDS- ENCOUNTER
    ON,
    ENCOUNTER,
    WITH,
    IN,
    EVERY,
    RANDOM,
    GENERATION,
    GENERATIONS,
    //KEYWORDS- SIMULATION CONTROL
    SIMULATE,
    SEED,
    //KEYWORDS- CONTROL FLOW
    IF,
    ELSE,
    WHILE,
    FOR,
    EACH,
    //KEYWORDS- BEHAVIORS
    HUNT,
    FLEE,
    IGNORE,
    EAT,
    DRINK,
    HARVEST,
    //KEYWORDS- OUTPUT
    LOG,
    POPULATION,
    OF,
    STATE,
    //ENUM VALUES- REPRODUCTION
    R_SELECTED,
    L_SELECTED,
    R_STRATEGY,
    K_STRATEGY,
    //ENUM VALUES- DIET
    HERBIVORE,
    CARNIVORE,
    OMNIVORE,
    DESCOMPOSER,
    //ENUM VALUES- HABITAT
    TERRESTRIAL,
    AQUATIC,
    AMPHIBIOUS,
    MIXED,
    //BOOLEAN 
    TRUE_VAL,
    FALSE_VAL,
	//Arithmetic operators
	ADD_OP,
	SUB_OP,
	MUL_OP,
	DIV_OP,
    //Assignment operators
    ASSIGN,
	ADD_ASSIGN,
	SUB_ASSIGN,
	MUL_ASSIGN,
	DIV_ASSIGN,
    //Relational operators
    LT,
	GT,
	EQ,
	NEQ,
	LTE,
	GTE,
    //LOGICAL
    AND,
    OR,
    NOT,
    //Punctuation
	OPEN_BRACE,
	CLOSE_BRACE,
	OPEN_PAREN,
	CLOSE_PAREN,
	OPEN_BRACKET,
	CLOSE_BRACKET,
	SEMICOLON,
	COMMA,
	DOT,
    //LITERALS
    INTEGER,
	FLOAT,
	BOOLEAN,
	STRING,
	ID,
    //OTHER
    UNKNOWN
}TokenLabel;

#endif

#ifndef TOKEN_LABEL_HEADER
#define TOKEN_LABEL_HEADER

/**
 * The type of a Bison token label, that is, an identifier of a token, that
 * transports the lexeme and semantic value as a whole.
 */
//typedef signed int TokenLabel;
typedef enum TokenLabel{
    //dejo solo los que bison no define 
    IGNORED = 1000,
    OPEN_COMMENT,
    CLOSE_COMMENT
}TokenLabel;

#endif

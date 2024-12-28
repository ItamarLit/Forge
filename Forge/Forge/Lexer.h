#ifndef LEXER_H 
#define LEXER_H

#include "HashMap.h"
#include "Token.h"

#define STATE_COUNT 9
#define MAX_CHAR_COUNT 256
#define VALID_STATE 3


// states in FSM
typedef enum states {
    // NOT accepting
    START_STATE,
    ERROR_TOKEN_STATE,
    STRING_LITERAL_STATE,
    // accepting
    IDENTIFIER_STATE, //3
    INT_LITERAL_STATE,
    OPERATOR_STATE,
    DOUBLE_OPERATOR_STATE,
    TERMINATED_STRING_LITERAL_STATE, //7 
    OPEN_PAREN_STATE,
    CLOSED_PAREN_STATE,
    OPEN_BRACE_STATE,
    CLOSE_BRACE_STATE,
    SEMI_COLON_STATE,
    COMMA_STATE,
    POINT_STATE,

} State;



typedef struct {
    State current_state;
    char input_char;
    State next_state;
} Transition;

// func that inits the state machine
void init_state_machine(HashMap** map);
// func that preforms the lexing
void lex(HashMap* map, char* input, pTokenArray ptoken_array);
// func that will identifiy the keywords
int identifyKeyowrd(char* lexeme);
// func that will identify the token type
TokenType state_to_token_type(State state);

#endif
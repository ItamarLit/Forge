#include "lexer.h"
#include "Token.h"
#include <stdio.h>
#include <stdlib.h>
#pragma warning (disable:4996)


// Initialize the hash map for the FSM transitions
void init_state_machine(HashMap** map) {
    // init the hash map with the starting size
    *map = initHashMap(INITAL_HASHMAP_SIZE);

    // Transitions for START_STATE
    putState(START_STATE, '_', IDENTIFIER_STATE, *map);
    putState(START_STATE, '"', STRING_LITERAL_STATE, *map);
    // skip spaces
    putState(START_STATE, ' ', START_STATE, *map);
    putState(START_STATE, '\n', START_STATE, *map);
    putState(START_STATE, '\t', START_STATE, *map);
    putState(START_STATE, '\r', START_STATE, *map);



    for (char c = 'a'; c <= 'z'; c++) {
        // for start state
        putState(START_STATE, c, IDENTIFIER_STATE, *map);
        // for identifier state
        putState(IDENTIFIER_STATE, c, IDENTIFIER_STATE, *map);
        putState(STRING_LITERAL_STATE, c, STRING_LITERAL_STATE, *map);


    }
    for (char c = 'A'; c <= 'Z'; c++) {
        putState(START_STATE, c, IDENTIFIER_STATE, *map);
        // for identifier state
        putState(IDENTIFIER_STATE, c, IDENTIFIER_STATE, *map);
        putState(STRING_LITERAL_STATE, c, STRING_LITERAL_STATE, *map);

    }
    for (char c = '0'; c <= '9'; c++) {
        // for start state
        putState(START_STATE, c, INT_LITERAL_STATE, *map);
        putState(STRING_LITERAL_STATE, c, STRING_LITERAL_STATE, *map);


        // for identifier state
        putState(IDENTIFIER_STATE, c, IDENTIFIER_STATE, *map);
        // for int state
        putState(INT_LITERAL_STATE, c, INT_LITERAL_STATE, *map);


    }

    // Transitions for operators
    putState(START_STATE, '=', OPERATOR_STATE, *map);
    putState(START_STATE, '+', OPERATOR_STATE, *map);
    putState(START_STATE, '-', OPERATOR_STATE, *map);
    putState(START_STATE, '*', OPERATOR_STATE, *map);
    putState(START_STATE, '/', OPERATOR_STATE, *map);


    putState(STRING_LITERAL_STATE, ' ', STRING_LITERAL_STATE, *map);

    // Transition for ending string literal
    putState(STRING_LITERAL_STATE, '"', TERMINATED_STRING_LITERAL_STATE, *map);

    // transition for double operator
    putState(OPERATOR_STATE, '=', DOUBLE_OPERATOR_STATE, *map);

    // Transitions for the paren
    putState(START_STATE, '(', OPEN_PAREN_STATE, *map);
    putState(START_STATE, ')', CLOSED_PAREN_STATE, *map);

    // Transitions for the braces
    putState(START_STATE, '{', OPEN_BRACE_STATE, *map);
    putState(START_STATE, '}', CLOSE_BRACE_STATE, *map);

    // Trasition for the semi colon, comma and point state
    putState(START_STATE, ';', SEMI_COLON_STATE, *map);
    putState(START_STATE, ',', COMMA_STATE, *map);
    putState(START_STATE, '.', POINT_STATE, *map);
}

TokenType state_to_token_type(State state) {
    static const TokenType state_to_token_type_map[] = {
        [ERROR_TOKEN_STATE] = ERROR,
        [IDENTIFIER_STATE] = IDENTIFIER,
        [INT_LITERAL_STATE] = INT_LITERAL,
        [OPERATOR_STATE] = OPERATOR,
        [DOUBLE_OPERATOR_STATE] = DOUBLE_OPERATOR,
        [TERMINATED_STRING_LITERAL_STATE] = STRING_LITERAL,
        [OPEN_PAREN_STATE] = OPEN_PAREN,
        [CLOSED_PAREN_STATE] = CLOSED_PAREN,
        [OPEN_BRACE_STATE] = OPEN_BRACE,
        [CLOSE_BRACE_STATE] = CLOSE_BRACE,
        [SEMI_COLON_STATE] = SEMI_COLON,
        [COMMA_STATE] = COMMA,
        [POINT_STATE] = POINT,
    };

    if (state >= VALID_STATE && state < sizeof(state_to_token_type_map) / sizeof(state_to_token_type_map[0])) {
        return state_to_token_type_map[state];
    }

    return -1;
}


void lex(HashMap* map, char* input, pTokenArray ptoken_array) {
    State current_state = START_STATE;
    State last_accepting_state = -1;
    char current_lexeme[MAX_LEXEME_LEN];
    int lexeme_index = 0;
    // go over the whole input
    while (*input != '\0') {
        // get the next state from the hash map o(1)
        current_state = getNextState(current_state, *input, map);
        // check if there is a valid transition and if so check that it isnt a whitespace skip (Start_State)
        if (current_state != -1 && current_state != START_STATE)
        {
            // add the current char
            current_lexeme[lexeme_index++] = *input;
            input++;
            // check if the current state is accepting
            if (current_state >= VALID_STATE) {
                last_accepting_state = current_state;
            }
        }
        else
        {
            // check if the token that is invalid is a whitespace and needs to be skipped
            if (current_state == START_STATE) {
                input++;
            }
            else if (last_accepting_state >= VALID_STATE)
            {
                // check if the token is valid and needs to be added
                current_lexeme[lexeme_index] = '\0';
                addToken(&ptoken_array, state_to_token_type(last_accepting_state), current_lexeme);
                // reset
                current_state = START_STATE;
                last_accepting_state = START_STATE;
                lexeme_index = 0;
            }
            else {
                // after we encounter an error token we check to see if we have many in a row
                // WHILE NO TRANSITION FROM START STATE ADD TO TOKEN AND ADD ERROR TOKEN  
                while ((getNextState(START_STATE, *input, map) == -1) && *input != '\0')
                {
                    current_lexeme[lexeme_index++] = *input;
                    input++;
                }
                // check if the token is valid and needs to be added
                current_lexeme[lexeme_index] = '\0';
                addToken(&ptoken_array, state_to_token_type(ERROR_TOKEN_STATE), current_lexeme);
                // reset
                current_state = START_STATE;
                last_accepting_state = START_STATE;
                lexeme_index = 0;
            }
        }
    }
    // After end of input, check if we have an accepting state
    if (last_accepting_state >= VALID_STATE) {
        current_lexeme[lexeme_index] = '\0';
        addToken(&ptoken_array, last_accepting_state, current_lexeme);
    }
}
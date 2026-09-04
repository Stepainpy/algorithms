#ifndef SHUNTING_YARD_ALGORITHM_H
#define SHUNTING_YARD_ALGORITHM_H

/* Order of token equivalent his precedence (from low to high) */
typedef enum {
    SYAT_EOS = 0, /* End Of Sequence */

    /* Punctuation */
    SYAT_LBRACKET,
    SYAT_RBRACKET,
    SYAT_COMMA,

    /* Literals */
    /* Next token after this is raw value or "ID" */
    SYAT_NUMBER,
    SYAT_FUNCTION,

    /* Binary operators */
    SYAT_EQ,
    SYAT_NE,
    SYAT_LT,
    SYAT_GT,
    SYAT_LE,
    SYAT_GE,

    SYAT_IOR,
    SYAT_XOR,
    SYAT_AND,

    SYAT_SHL,
    SYAT_SHR,
    SYAT_ROL,
    SYAT_ROR,

    SYAT_ADD,
    SYAT_SUB,
    SYAT_MUL,
    SYAT_DIV,
    SYAT_MOD,
    SYAT_POW,

    /* Unary operators */
    SYAT_NEG,
    SYAT_INV
} sya_token_t;

typedef enum {
    SYAA_LEFT  = 0,
    SYAA_RIGHT = 1
} sya_assoc_t;

typedef enum {
    SYAE_OK = 0,
    SYAE_NULL_POINTER,
    SYAE_STACK_OVERFLOW,
    SYAE_STACK_UNDERFLOW,
    SYAE_NO_LEFT_BRACKET,
    SYAE_NO_RIGHT_BRACKET
} sya_error_t;

sya_assoc_t sya_get_assoc(sya_token_t token);

sya_error_t sya_convert(sya_token_t* sequence);

#endif /* SHUNTING_YARD_ALGORITHM_H */
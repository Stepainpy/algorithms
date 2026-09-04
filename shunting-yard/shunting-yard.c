#include "shunting-yard.h"

#include <stddef.h>

#define SYAC_STACK_SIZE 64

#define sya_push(value) do {        \
    if (sp >= SYAC_STACK_SIZE)      \
        return SYAE_STACK_OVERFLOW; \
    stack[sp++] = (value);          \
} while (0)

sya_assoc_t sya_get_assoc(sya_token_t token) {
    switch (token) {
        case SYAT_POW:
        case SYAT_NEG:
        case SYAT_INV:
            return SYAA_RIGHT;
        default:
            return SYAA_LEFT;
    }
}

sya_error_t sya_convert(sya_token_t* sequence) {
    sya_token_t stack[SYAC_STACK_SIZE];
    sya_token_t* head = sequence;
    sya_token_t* out  = sequence;
    size_t sp = 0;

    if (!sequence) return SYAE_NULL_POINTER;

    while (1) {
        sya_token_t token = *head++, value;
        switch (token) {
            /* Exit from loop */
            case SYAT_EOS: goto loop_exit;

            /* Handling of literals */
            case SYAT_NUMBER:
                value = *head++;
                *out++ = token;
                *out++ = value;
                break;

            case SYAT_FUNCTION:
                value = *head++;
                sya_push(value);
                sya_push(token);
                break;

            /* Handling of punctuations */
            case SYAT_LBRACKET:
                sya_push(token);
                break;

            case SYAT_RBRACKET:
                while (sp > 0 && stack[sp - 1] != SYAT_LBRACKET) {
                    token = stack[--sp];
                    *out++ = token;
                    if (token == SYAT_FUNCTION) {
                        if (sp == 0) return SYAE_STACK_UNDERFLOW;
                        *out++ = stack[--sp];
                    }
                }
                if (sp == 0) return SYAE_NO_LEFT_BRACKET;
                --sp; /* truncate left bracket */
                if (sp > 0 && stack[sp - 1] == SYAT_FUNCTION) {
                    token = stack[--sp];
                    if (sp == 0) return SYAE_STACK_UNDERFLOW;
                    value = stack[--sp];
                    *out++ = token;
                    *out++ = value;
                }
                break;

            case SYAT_COMMA:
                while (sp > 0 && stack[sp - 1] != SYAT_LBRACKET) {
                    token = stack[--sp];
                    *out++ = token;
                    if (token == SYAT_FUNCTION) {
                        if (sp == 0) return SYAE_STACK_UNDERFLOW;
                        *out++ = stack[--sp];
                    }
                }
                if (sp == 0) return SYAE_NO_LEFT_BRACKET;
                break;

            /* Handling of operators */
            default:
                while (sp > 0 && (
                     token <  stack[sp - 1] ||
                    (token == stack[sp - 1] && sya_get_assoc(token) == SYAA_LEFT)
                )) *out++ = stack[--sp];
                sya_push(token);
                break;
        }
    }

loop_exit:
    while (sp > 0) {
        if (stack[sp - 1] == SYAT_LBRACKET)
            return SYAE_NO_RIGHT_BRACKET;
        *out++ = stack[--sp];
    }

    *out++ = SYAT_EOS;
    return SYAE_OK;
}
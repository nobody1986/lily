#ifndef LILY_RAISER_H
# define LILY_RAISER_H

# include <setjmp.h>

# include "lily_core_types.h"
# include "lily_msgbuf.h"

# define lily_Error               0
# define lily_SyntaxError         1
# define lily_DivisionByZeroError 2
# define lily_IndexError          3
# define lily_ValueError          4
# define lily_RuntimeError        5
# define lily_KeyError            6
# define lily_FormatError         7
# define lily_IOError             8

/* The raiser is included by a majority of Lily's core modules, but seed raise
   is only used by foreign modules. Since foreign modules will need to include
   seeds to have them, just forward declare this. */
struct lily_base_seed_;

typedef struct lily_jump_link_ {
    struct lily_jump_link_ *prev;
    struct lily_jump_link_ *next;

    jmp_buf jump;
} lily_jump_link;

typedef struct lily_raiser_ {
    lily_jump_link *all_jumps;
    lily_msgbuf *msgbuf;
    lily_class *exception_cls;
    lily_value *exception_value;

    /* This is 0 if the error line is the lexer's current line number.
       Otherwise, this is the line number to report. It is not an offset.
       Uses:
       * Merging ASTs. number a = 1 +
         1 should report an error on line 1 (the assignment), not line 2.
       * Any vm error. */
    uint32_t line_adjust;
    int16_t error_code;
    uint16_t have_error;
} lily_raiser;

lily_raiser *lily_new_raiser(void);
void lily_free_raiser(lily_raiser *);
void lily_raise(lily_raiser *, int, char *, ...);
void lily_raise_prebuilt(lily_raiser *, int);
void lily_raise_value(lily_raiser *, lily_class *, lily_value *, const char *);
void lily_raise_prepared(lily_raiser *);
lily_jump_link *lily_jump_setup(lily_raiser *);
void lily_jump_back(lily_raiser *);
void lily_release_jump(lily_raiser *);

const char *lily_name_for_error(lily_raiser *);

#endif

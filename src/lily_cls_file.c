#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "lily_alloc.h"
#include "lily_vm.h"
#include "lily_value.h"
#include "lily_seed.h"

lily_file_val *lily_new_file_val(FILE *inner_file, char mode_ch)
{
    lily_file_val *filev = lily_malloc(sizeof(lily_file_val));

    filev->refcount = 1;
    filev->inner_file = inner_file;
    filev->read_ok = (mode_ch == 'r');
    filev->write_ok = (mode_ch == 'w');
    filev->is_builtin = 0;

    return filev;
}

void lily_destroy_file(lily_value *v)
{
    lily_file_val *filev = v->value.file;

    if (filev->inner_file && filev->is_builtin == 0)
        fclose(filev->inner_file);

    lily_free(filev);
}

static void write_check(lily_vm_state *vm, lily_file_val *filev)
{
    if (filev->inner_file == NULL)
        lily_raise(vm->raiser, lily_IOError, "IO operation on closed file.\n");

    if (filev->write_ok == 0)
        lily_raise(vm->raiser, lily_IOError, "File not open for writing.\n");
}

static void read_check(lily_vm_state *vm, lily_file_val *filev)
{
    if (filev->inner_file == NULL)
        lily_raise(vm->raiser, lily_IOError, "IO operation on closed file.\n");

    if (filev->read_ok == 0)
        lily_raise(vm->raiser, lily_IOError, "File not open for reading.\n");
}

void lily_file_close(lily_vm_state *vm, uint16_t argc, uint16_t *code)
{
    lily_value **vm_regs = vm->vm_regs;
    lily_file_val *filev = vm_regs[code[1]]->value.file;

    if (filev->inner_file != NULL) {
        if (filev->is_builtin == 0)
            fclose(filev->inner_file);
        filev->inner_file = NULL;
    }
}

void lily_file_open(lily_vm_state *vm, uint16_t argc, uint16_t *code)
{
    lily_value **vm_regs = vm->vm_regs;
    char *path = vm_regs[code[1]]->value.string->string;
    char *mode = vm_regs[code[2]]->value.string->string;
    lily_value *result_reg = vm_regs[code[0]];

    errno = 0;
    char mode_ch = mode[0];
    if (mode_ch != 'a' &&
        mode_ch != 'w' &&
        mode_ch != 'r')
        lily_raise(vm->raiser, lily_IOError,
                "Mode must start with one of 'arw', but got '%c'.\n", mode_ch);

    FILE *f = fopen(path, mode);
    if (f == NULL) {
        lily_raise(vm->raiser, lily_IOError, "Errno %d: ^R (%s).\n",
                errno, errno, path);
    }

    lily_file_val *filev = lily_new_file_val(f, mode_ch);

    lily_move_file(result_reg, filev);
}

void lily_file_write(lily_vm_state *, uint16_t, uint16_t *);

void lily_file_print(lily_vm_state *vm, uint16_t argc, uint16_t *code)
{
    lily_file_write(vm, argc, code);
    fputc('\n', vm->vm_regs[code[1]]->value.file->inner_file);
}

void lily_file_readline(lily_vm_state *vm, uint16_t argc, uint16_t *code)
{
    lily_value **vm_regs = vm->vm_regs;
    lily_file_val *filev = vm_regs[code[1]]->value.file;
    lily_value *result_reg = vm_regs[code[0]];
    lily_msgbuf *vm_buffer = vm->vm_buffer;
    lily_msgbuf_flush(vm_buffer);

    /* This ensures that the buffer will always have space for the \0 at the
       very end. */
    int buffer_size = vm_buffer->size - 1;
    char *buffer = vm_buffer->message;

    int ch = 0;
    int pos = 0;

    read_check(vm, filev);
    FILE *f = filev->inner_file;

    /* This uses fgetc in a loop because fgets may read in \0's, but doesn't
       tell how much was written. */
    while (1) {
        ch = fgetc(f);

        if (ch == EOF) {
            buffer[pos] = '\0';
            break;
        }

        buffer[pos] = (char)ch;

        /* \r is intentionally not checked for, because it's been a very, very
           long time since any os used \r alone for newlines. */
        if (ch == '\n') {
            buffer[pos + 1] = '\0';
            pos++;
            break;
        }

        if (pos == buffer_size) {
            lily_msgbuf_grow(vm_buffer);
            buffer = vm_buffer->message;
            buffer_size = vm_buffer->size - 1;
        }

        pos++;
    }

    lily_string_val *new_sv = lily_malloc(sizeof(lily_string_val));
    char *sv_buffer = lily_malloc(pos + 1);

    /* Use memcpy, in case there are embedded \0 values somewhere. */
    memcpy(sv_buffer, buffer, pos + 1);

    new_sv->string = sv_buffer;
    new_sv->refcount = 1;
    new_sv->size = pos;

    lily_msgbuf_flush(vm_buffer);

    lily_move_string(result_reg, new_sv);
}

void lily_file_write(lily_vm_state *vm, uint16_t argc, uint16_t *code)
{
    lily_value **vm_regs = vm->vm_regs;
    lily_file_val *filev = vm_regs[code[1]]->value.file;
    lily_value *to_write = vm_regs[code[2]];

    write_check(vm, filev);

    if (to_write->flags & VAL_IS_STRING)
        fputs(to_write->value.string->string, filev->inner_file);
    else {
        lily_msgbuf *msgbuf = vm->vm_buffer;
        lily_msgbuf_flush(msgbuf);
        lily_vm_add_value_to_msgbuf(vm, msgbuf, to_write);
        fputs(msgbuf->message, filev->inner_file);
    }
}

static const lily_func_seed file_close =
    {NULL, "close", dyna_function, "(File)", lily_file_close};

static const lily_func_seed file_open =
    {&file_close, "open", dyna_function, "(String, String):File", lily_file_open};

static const lily_func_seed file_print =
    {&file_open, "print", dyna_function, "[A](File, A)", lily_file_print};

static const lily_func_seed file_readline =
    {&file_print, "readline", dyna_function, "(File):ByteString", lily_file_readline};

static const lily_func_seed dynaload_start =
    {&file_readline, "write", dyna_function, "[A](File, A)", lily_file_write};


static const lily_class_seed file_seed =
{
    NULL,             /* next */
    "File",           /* name */
    dyna_class,       /* load_type */
    1,                /* is_refcounted */
    0,                /* generic_count */
    &dynaload_start,  /* dynaload_table */
    lily_destroy_file /* destroy_func */
};

lily_class *lily_file_init(lily_symtab *symtab)
{
    return lily_new_class_by_seed(symtab, &file_seed);
}

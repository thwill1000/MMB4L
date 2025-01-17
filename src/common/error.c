/*-*****************************************************************************

MMBasic for Linux (MMB4L)

error.c

Copyright 2021-2023 Geoff Graham, Peter Mather and Thomas Hugo Williams.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holders nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

4. The name MMBasic be used when referring to the interpreter in any
   documentation and promotional material and the original copyright message
   be displayed  on the console at startup (additional copyright messages may
   be added).

5. All advertising materials mentioning features or use of this software must
   display the following acknowledgement: This product includes software
   developed by Geoff Graham, Peter Mather and Thomas Hugo Williams.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#include "error.h"

#include "mmb4l.h"
#include "cstring.h"
#include "exit_codes.h"
#include "path.h"
#include "program.h"
#include "utility.h"

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern jmp_buf ErrNext;

void error_init(ErrorState *error_state) {
    error_state->code = 0;
    *error_state->file = '\0';
    error_state->line = -1;
    *error_state->message = '\0';
    error_state->skip = 0;
    error_state->override_line = false;
}

void error_get_line_and_file(int *line, char *file_path) {

    *line = -1;
    memset(file_path, 0, STRINGSIZE);

    if (!CurrentLinePtr) return;

    assert(CurrentLinePtr < (char *) ProgMemory + PROG_FLASH_SIZE);

    if (*CurrentLinePtr != T_NEWLINE) {
        // normally CurrentLinePtr points to a T_NEWLINE token but in this
        // case it does not so we have to search for the start of the line
        // and set CurrentLinePtr to that
        char *p = (char *) ProgMemory;
        char *tp = p;
        while (*p != 0xff) {
            while (*p)
                p++;  // look for the zero marking the start of an element
            if (p >= CurrentLinePtr ||
                p[1] == 0) {  // the previous line was the one that we wanted
                CurrentLinePtr = tp;
                break;
            }
            if (p[1] == T_NEWLINE) {
                tp = ++p;  // save because it might be the line we want
            }
            p++;  // step over the zero marking the start of the element
            skipspace(p);
            if (p[0] == T_LABEL) p += p[1] + 2;  // skip over the label
        }
    }

    assert(CurrentLinePtr < (char *) ProgMemory + PROG_FLASH_SIZE);

    // We now have CurrentLinePtr pointing to the start of the line.
    llist(tknbuf, CurrentLinePtr);
    // p = tknbuf;
    // skipspace(p);

    // Search backwards for the '|' character that delimits the meta-data about
    // which file the statement originated from. Note that if the name of the
    // file from which the statement was sourced contains a '|' then all bets
    // are off.
    char *pipe_pos = strrchr(tknbuf, '|');
    if (!pipe_pos) return;

    char *comma_pos = strchr(pipe_pos, ',');
    if (comma_pos) {
        // Line is from a #included file.
        pipe_pos++;
        comma_pos++;
        *line = atoi(comma_pos);

        memcpy(file_path, pipe_pos, comma_pos - pipe_pos - 1);
        file_path[comma_pos - pipe_pos] = '\0';
        char tmp[STRINGSIZE];
        MmResult result = path_get_parent(CurrentFile, tmp, STRINGSIZE);
        if (SUCCEEDED(result)) result = cstring_cat(tmp, "/", STRINGSIZE);
        if (SUCCEEDED(result)) result = cstring_cat(tmp, file_path, STRINGSIZE);
        if (SUCCEEDED(result)) result = path_get_canonical(tmp, file_path, STRINGSIZE);
        if (FAILED(result)) strcpy(file_path, "<invalid path>");
    } else {
        // Line is from the main file.
        pipe_pos++;
        *line = atoi(pipe_pos);
        strcpy(file_path, CurrentFile);
    }
}

// throw an error
// displays the error message and aborts the program
// the message can contain variable text which is indicated by a special character in the message string
//  $ = insert a string at this place
//  @ = insert a character
//  % = insert a number
// the optional data to be inserted is the second argument to this function
// this uses longjump to skip back to the command input and cleanup the stack
static void verror(MmResult error, const char *msg, va_list argp) {
    options_load(&mmb_options, OPTIONS_FILE_NAME, NULL);  // make sure that the option struct is in a clean state

    mmb_error_state_ptr->code = error;
    if (!mmb_error_state_ptr->override_line) {
        // Unless explicitly overridden we automatically determine error line/file.
        error_get_line_and_file(&mmb_error_state_ptr->line, mmb_error_state_ptr->file);
    }
    mmb_error_state_ptr->override_line = false;

    char buf[STRINGSIZE * 2];

    if (mmb_error_state_ptr->line <= 0) {
        sprintf(buf, "Error: ");
    } else if (strcmp(mmb_error_state_ptr->file, CurrentFile) == 0) {
        sprintf(buf, "Error in line %d: ", mmb_error_state_ptr->line);
    } else {
        sprintf(buf, "Error in %s line %d: ", mmb_error_state_ptr->file, mmb_error_state_ptr->line);
    }

    assert(msg);
    char tmp[20];
    while (*msg) {
        if (*msg == '$') {
            (void) cstring_cat(buf, va_arg(argp, char *), STRINGSIZE * 2);
        } else if (*msg == '@') {
            sprintf(tmp, "%c", (char) va_arg(argp, int));
            (void) cstring_cat(buf, tmp, STRINGSIZE * 2);
        } else if (*msg == '%' || *msg == '|') {
            IntToStr(tmp, va_arg(argp, int), 10);
            (void) cstring_cat(buf, tmp, STRINGSIZE * 2);
        } else if (*msg == '&') {
            IntToStr(tmp, va_arg(argp, int), 16);
            (void) cstring_cat(buf, tmp, STRINGSIZE * 2);
        } else {
            sprintf(tmp, "%c", *msg);
            (void) cstring_cat(buf, tmp, STRINGSIZE * 2);
        }
        msg++;
    }

    cstring_cpy(mmb_error_state_ptr->message, buf, MAXERRMSG);

    if (mmb_error_state_ptr->skip) {
        *mmb_error_state_ptr->file = '\0';
        mmb_error_state_ptr->line = -1;
        longjmp(ErrNext, 1);
    } else {
        longjmp(mark, JMP_ERROR);
    }
}

MmResult error_throw_legacy(const char *msg, ...) {
    va_list argp;
    va_start(argp, msg);
    verror(kError, msg, argp);
    assert(0); // Don't expect to get here because of long_jmp().
    va_end(argp);
    return kError;
}

MmResult error_throw_ex(MmResult result, const char *msg, ...) {
    va_list argp;
    va_start(argp, msg);
    verror(result, msg, argp);
    assert(0); // Don't expect to get here because of long_jmp().
    va_end(argp);
    return result;
}

MmResult error_throw(MmResult result) {
    return error_throw_ex(result, mmresult_to_string(result));
}

uint8_t error_to_exit_code(MmResult result) {
    switch (result) {
        default:
            return EX_FAIL;
    }
}

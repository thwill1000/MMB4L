/*-*****************************************************************************

MMBasic for Linux (MMB4L)

file.h

Copyright 2021-2024 Geoff Graham, Peter Mather and Thomas Hugo Williams.

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

#if !defined(MMB4L_FILE)
#define MMB4L_FILE

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "../Configuration.h"
#include "mmresult.h"
#include "rx_buf.h"

enum FileEntryType { fet_closed, fet_file, fet_serial };

typedef struct {
    enum FileEntryType type;
    union {
        FILE *file_ptr;
        int serial_fd;
    };
    RxBuf rx_buf;
} FileEntry;

extern FileEntry file_table[MAXOPENFILES + 1];

void MMgetline(int fnbr, char *p); // main.c

/** Finds the first available free file number. */
int file_find_free(void);

MmResult file_open(const char *filename, const char *mode, int fnbr);
MmResult file_close(int fnbr);
void file_close_all(void);
int file_eof(int fnbr);
int file_getc(int fnbr);
int file_loc(int fnbr);
int file_lof(int fnbr);
int file_putc(int fnbr, int ch);
size_t file_read(int fnbr, char *buf, size_t sz);
void file_seek(int fnbr, int idx);
size_t file_write(int fnbr, const char *buf, size_t sz);

#endif

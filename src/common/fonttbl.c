/*-*****************************************************************************

MMBasic for Linux (MMB4L)

fonttbl.c

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

#include "fonttbl.h"

#include "../fonts/ArialNumFontPlus.h"
#include "../fonts/Fnt_10x16.h"
#include "../fonts/font1.h"
#include "../fonts/Font_8x6.h"
#include "../fonts/Hom_16x24_LE.h"
#include "../fonts/Inconsola.h"
#include "../fonts/Misc_12x20_LE.h"

#include <stddef.h>

unsigned char* FontTable[FONT_TABLE_SIZE + 1] = {
    NULL, // Element 0 unused.
    (unsigned char *) font1,
    (unsigned char *) Misc_12x20_LE,
    (unsigned char *) Hom_16x24_LE,
    (unsigned char *) Fnt_10x16,
    (unsigned char *) Inconsola,
    (unsigned char *) ArialNumFontPlus,
    (unsigned char *) F_6x8_LE,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

void font_clear_user_defined() {
    for (int i = FONT_BUILTIN_NBR + 1; i <= FONT_TABLE_SIZE; ++i) {
        FontTable[i] = NULL;
    }
}

uint32_t font_width(uint32_t font) {
    const uint32_t font_id = font >> 4;
    const uint32_t scaling = font & 0b1111;
    if (font_id > FONT_TABLE_SIZE || !FontTable[font_id]) return 0;
    return FontTable[font_id][0] * scaling;
}

uint32_t font_height(uint32_t font) {
    const uint32_t font_id = font >> 4;
    const uint32_t scaling = font & 0b1111;
    if (font_id > FONT_TABLE_SIZE || !FontTable[font_id]) return 0;
    return FontTable[font_id][1] * scaling;
}

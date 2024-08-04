/*-*****************************************************************************

MMBasic for Linux (MMB4L)

cmd_gamepad.c

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

#include "../common/gamepad.h"
#include "../common/interrupt.h"
#include "../common/mmb4l.h"
#include "../common/utility.h"

/** GAMEPAD OFF */
static MmResult cmd_gamepad_off(const char *p) {
    skipspace(p);
    if (!parse_is_end(p)) return kUnexpectedText;
    return gamepad_close(1);
}

/** GAMEPAD ON [interrupt] [, bitmask] */
static MmResult cmd_gamepad_on(const char *p) {
    getargs(&p, 3, ",");
    if (argc > 3) return kArgumentCount;
    const char* interrupt_addr = (argc > 0) ? GetIntAddress(argv[0]) : NULL;
    uint16_t bitmask = (argc == 3) ? getint(argv[2], 0, GAMEPAD_BITMASK_ALL) : GAMEPAD_BITMASK_ALL;
    return gamepad_open(1, interrupt_addr, bitmask);
}

/** GAMEPAD STOP */
static MmResult cmd_gamepad_stop(const char *p) {
    skipspace(p);
    if (!parse_is_end(p)) return kUnexpectedText;
    return gamepad_vibrate(1, 0, 0, 0);
}

/** GAMEPAD VIBRATE */
static MmResult cmd_gamepad_vibrate(const char *p) {
    skipspace(p);
    if (!parse_is_end(p)) return kUnexpectedText;
    return gamepad_vibrate(1, UINT16_MAX, UINT16_MAX, 10000);
}

void cmd_gamepad(void) {
    if (mmb_options.simulate != kSimulateMmb4w) ERROR_ON_FAILURE(kUnsupportedOnCurrentDevice);
    MmResult result = kOk;
    const char *p;
    if ((p = checkstring(cmdline, "ON"))) {
        result = cmd_gamepad_on(p);
    } else if ((p = checkstring(cmdline, "OFF"))) {
        result = cmd_gamepad_off(p);
    } else if ((p = checkstring(cmdline, "VIBRATE"))) {
        result = cmd_gamepad_vibrate(p);
    } else if ((p = checkstring(cmdline, "STOP"))) {
        result = cmd_gamepad_stop(p);
    } else {
        ERROR_UNKNOWN_SUBCOMMAND("GAMEPAD");
    }
    ERROR_ON_FAILURE(result);
}

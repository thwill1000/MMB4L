#include <stdio.h>

#include "../common/mmb4l.h"
#include "../common/options.h"
#include "../common/utility.h"

void cmd_new(void) {
//    if(CurrentLinePtr) ERROR_INVALID_IN_PROGRAM;
    checkend(cmdline);
    ClearSavedVars();
    FlashWriteInit(ProgMemory, mmb_options.prog_flash_size);
    ClearProgram();
    WatchdogSet = false;
    mmb_options.autorun = false;
    MmResult result = options_save(&mmb_options, OPTIONS_FILE_NAME);
    if (FAILED(result)) {
        char buf[STRINGSIZE];
        sprintf(buf, "Warning: failed to save options: %s", mmresult_to_string(result));
        MMPrintString(buf);
    }
    longjmp(mark, JMP_NEW);
}

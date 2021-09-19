#include "../common/console.h"
#include "../common/error.h"
#include "../common/parse.h"
#include "../common/program.h"
#include "../common/utility.h"
#include "../common/version.h"

void option_list(char *p); // cmd_option.c

/* qsort C-string comparison function */
static int cstring_cmp(const void *a, const void *b)  {
    const char **ia = (const char **)a;
    const char **ib = (const char **)b;
    return strcasecmp(*ia, *ib);
}

static void list_tokens(const char *title, const struct s_tokentbl *primary, int num_primary, const char **secondary) {
    int num_secondary = 0;
    const char **ptr = secondary;
    while (*ptr++) num_secondary++;

    const int total = num_primary + num_secondary;

    char **tbl = (char **) GetTempMemory(
            total * sizeof(char *) // Memory for char pointers.
            + total * 20);         // Memory for 20 character strings.

    // Initialise char pointers.
    for (int i = 0; i < total; i++) {
        tbl[i] = (char *) (tbl + total + i * 20);
    }

    // Copy primary items.
    for (int i = 0; i < num_primary; i++) {
        strcpy(tbl[i], primary[i].name);
        if (primary[i].fptr == cmd_dummy) {
            strcat(tbl[i], " (d)");
        }
    }

    // Copy secondary items.
    char buf[256];
    for (int i = num_primary; i < total; i++) {
        sprintf(buf, "%s (*)", secondary[i - num_primary]);
        strcpy(tbl[i], buf);
    }

    // Sort the table.
    qsort(tbl, total, sizeof(char *), cstring_cmp);

    int step = Option.Width / 20;
    for (int i = 0; i < total; i += step) {
        for (int k = 0; k < step; k++) {
            if (i + k < total) {
                MMPrintString(tbl[i + k]);
                if (k != (step - 1))
                    for (int j = strlen(tbl[i + k]); j < 19; j++) MMPrintString(" "); //putConsole(' ');
            }
        }
        MMPrintString("\r\n");
    }
    sprintf(buf, "Total of %d %s using %d slots\r\n\r\n", total, title, num_primary);
    MMPrintString(buf);

}

static void list_commands() {
    const char *secondary_commands[] = {
            "foo",
            "bar",
            NULL };
    list_tokens("commands", commandtbl, CommandTableSize - 1, secondary_commands);
}

static void list_functions() {
    const char *secondary_functions[] = {
            "foo",
            "bar",
            NULL };
    list_tokens("functions", tokentbl, TokenTableSize - 1, secondary_functions);
}

static void list_file(const char *filename, int all) {
    if (!filename && CurrentFile[0] == '\0') {
        error("Nothing to list");
        return;
    }

    char file_path[STRINGSIZE];
    if (!munge_path(filename ? filename : CurrentFile, file_path, STRINGSIZE)) {
        if (error_check()) return;
    }

    char line_buffer[STRINGSIZE];
    int list_count = 1;
    int file_num = FindFreeFileNbr();
    MMfopen(file_path, "rb", file_num);
    while (!MMfeof(file_num)) {
        memset(line_buffer, 0, STRINGSIZE);
        MMgetline(file_num, line_buffer);
        for (size_t i = 0; i < strlen(line_buffer); i++) {
            if (line_buffer[i] == TAB) line_buffer[i] = ' ';
        }
        MMPrintString(line_buffer);
        list_count += strlen(line_buffer) / Option.Width;
        ListNewLine(&list_count, all);
    }
    MMfclose(file_num);

    // Ensure listing is followed by an empty line.
    if (strcmp(line_buffer, "") != 0) MMPrintString("\r\n");
}

static void list_flash(int all) {
    if (CurrentFile[0] == '\0') {
        error("Nothing to list");
        return;
    }

    // Make sure we are looking at the latest (on disk) version of the program.
    if (!program_load_file(CurrentFile)) return;

    ListProgram(ProgMemory, all);

    MMPrintString("\r\n");
}

static void list_csubs(int all) {
    if (CurrentFile[0] == '\0') {
        error("Nothing to list");
        return;
    }

    // Make sure we are looking at the latest (on disk) version of the program.
    if (!program_load_file(CurrentFile)) return;

    program_list_csubs(all);
}

static void list_options() {
    option_list("");
}

void cmd_list(void) {
    char *p;
    skipspace(cmdline);

    // Use the current console dimensions for the output of the LIST command.
    console_get_size(&Option.Width, &Option.Height);

    if (parse_is_end(cmdline)) {
        list_file(NULL, false);
    } else if ((p = checkstring(cmdline, "COMMANDS"))) {
        if (!parse_is_end(p)) ERROR_SYNTAX;
        list_commands();
    } else if ((p = checkstring(cmdline, "CSUBS"))) {
        if (parse_is_end(p)) {
            list_csubs(false);
        } else if ((p = checkstring(p, "ALL"))) {
            if (!parse_is_end(p)) ERROR_SYNTAX;
            list_csubs(true);
        } else {
            ERROR_SYNTAX;
        }
    } else if ((p = checkstring(cmdline, "FLASH"))) {
        if (parse_is_end(p)) {
            list_flash(false);
        } else if ((p = checkstring(p, "ALL"))) {
            if (!parse_is_end(p)) ERROR_SYNTAX;
            list_flash(true);
        } else {
            ERROR_SYNTAX;
        }
    } else if ((p = checkstring(cmdline, "FUNCTIONS"))) {
        if (!parse_is_end(p)) ERROR_SYNTAX;
        list_functions();
    } else if ((p = checkstring(cmdline, "OPTIONS"))) {
        if (!parse_is_end(p)) ERROR_SYNTAX;
        list_options();
    } else {
        if ((p = checkstring(cmdline, "ALL"))) {
            if (parse_is_end(p)) {
                list_file(NULL, true);
            } else {
                list_file(getCstring(p), true);
            }
        } else {
            list_file(getCstring(cmdline), false);
        }
    }
}

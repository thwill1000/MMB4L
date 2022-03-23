// Copyright (c) 2021-2022 Thomas Hugo Williams

#if !defined(MMB4L_OPTION_H)
#define MMB4L_OPTION_H

#include <stdbool.h>

#include "../Configuration.h"
#include "mmresult.h"

#define OPTIONS_FILE_NAME  "~/.mmbasic/mmbasic.options"

typedef struct {
    char *id;      // Users specify these with OPTION EDITOR.
    char *value;   // Internally we use this.
    char *command; // Command to run for the given editor.
    bool blocking; // Does the editor command block.
} OptionsEditor;

enum options_console { BOTH, SCREEN, SERIAL };

enum options_resolution { CHARACTER, PIXEL };

typedef struct {
    int base;
    char break_key;
    char *codepage; // Pointer to one of the arrays/maps declared in 'codepage.c'
    char default_type;
    char editor[STRINGSIZE];  // TODO: should probably be shorter
    char explicit_type;
    char tab;
    char list_case;
    int  height;
    int  width;
    int  prog_flash_size;
    int  autorun;

    // Added for MMB4L
    enum options_console console;
    enum options_resolution resolution;

    char search_path[STRINGSIZE];

#if defined OPTION_TESTS
    bool    persistent_bool;
    int     persistent_int;
    MMFLOAT persistent_float;
    char    persistent_string[32];
#endif
} Options;

typedef void (*OPTIONS_WARNING_CB) (const char *);

extern Options mmb_options;
extern OptionsEditor options_editors[];

/** Initialises the options. */
void options_init(Options *options);

/** Loads persistent options from a file. */
MmResult options_load(Options *options, const char *filename, OPTIONS_WARNING_CB warning_cb);

/** Saves persistent options to a file. */
MmResult options_save(const Options *options, const char *filename);

MmResult options_set(Options *options, const char *name, const char *value);

void options_console_to_string(enum options_console console, char *buf);
void options_editor_to_string(const char *editor, char *buf);
void options_explicit_to_string(char explicit_type, char *buf);
void options_list_case_to_string(char list_case, char *buf);
void options_resolution_to_string(enum options_resolution resolution, char *buf);
void options_type_to_string(char type, char *buf);

#endif
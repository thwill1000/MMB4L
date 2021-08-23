#include <dirent.h>

#include "../common/version.h"

int32_t dirflags;

static char get_achar(/* Get a character and advances ptr 1 or 2 */
                      const char **ptr /* Pointer to pointer to the
                                          SBCS/DBCS/Unicode string */
) {
    char chr;

    chr = (char)*(*ptr)++;                         /* Get a byte */
    if ((chr >= 'a') && (chr <= 'z')) chr -= 0x20; /* To upper ASCII char */
    return chr;
}

static int
pattern_matching(                 /* 0:not matched, 1:matched */
                 const char *pat, /* Matching pattern */
                 const char *nam, /* String to be tested */
                 int skip,        /* Number of pre-skip chars (number of ?s) */
                 int inf          /* Infinite search (* specified) */
) {
    const char *pp, *np;
    char pc, nc;
    int nm, nx;

    while (skip--) { /* Pre-skip name chars */
        if (!get_achar(&nam))
            return 0; /* Branch mismatched if less name chars */
    }
    if (!*pat && inf) return 1; /* (short circuit) */

    do {
        pp = pat;
        np = nam; /* Top of pattern and name to match */
        for (;;) {
            if (*pp == '?' || *pp == '*') { /* Wildcard? */
                nm = nx = 0;
                do { /* Analyze the wildcard chars */
                    if (*pp++ == '?')
                        nm++;
                    else
                        nx = 1;
                } while (*pp == '?' || *pp == '*');
                if (pattern_matching(pp, np, nm, nx))
                    return 1; /* Test new branch (recurs upto number of wildcard
                                 blocks in the pattern) */
                nc = *np;
                break; /* Branch mismatched */
            }
            pc = get_achar(&pp); /* Get a pattern char */
            nc = get_achar(&np); /* Get a name char */
            if (pc != nc) break; /* Branch mismatched? */
            if (pc == 0)
                return 1; /* Branch matched? (matched at end of both strings) */
        }
        get_achar(&nam); /* nam++ */
    } while (inf &&
             nc); /* Retry until end of name if infinite search is specified */

    return 0;
}

void fun_dir(void) {
    static DIR *dp;
    char *p;
    struct dirent *entry;
    static char pp[32];
    getargs(&ep, 3, ",");
    if (argc != 0) dirflags = DT_REG; // 0 ?
    if (argc > 3) error("Syntax");

    if (argc == 3) {
        if (checkstring(argv[2], "DIR"))
            dirflags = DT_DIR;
        else if (checkstring(argv[2], "FILE"))
            dirflags = DT_REG;
        else if (checkstring(argv[2], "ALL"))
            dirflags = 0;
        else
            error("Invalid flag specification");
    }

    if (argc != 0) {
        // This must be the first call eg:  DIR$("*.*", FILE)
        p = getCstring(argv[0]);
        strcpy(pp, p);
        dp = opendir(".");
        if (dp == NULL) {
            error("Unable to open directory");
        }
    }

    if (dirflags == DT_DIR) {
        for (;;) {
            entry = readdir(dp); /* Get a directory item */
            if (!entry) break; /* Terminate if any error or end of directory */
            if (pattern_matching(pp, entry->d_name, 0, 0) &&
                (entry->d_type == DT_DIR))
                break; /* Test for the file name */
        }
    } else if (dirflags == DT_REG) {
        for (;;) {
            entry = readdir(dp); /* Get a directory item */
            if (!entry) break; /* Terminate if any error or end of directory */
            if (pattern_matching(pp, entry->d_name, 0, 0) &&
                (entry->d_type == DT_REG))
                break; /* Test for the file name */
        }
    } else {
        for (;;) {
            entry = readdir(dp); /* Get a directory item */
            if (!entry) break; /* Terminate if any error or end of directory */
            if (pattern_matching(pp, entry->d_name, 0, 0) &&
                (entry->d_type == DT_DIR))
                break; /* Test for the file name */
        }
    }

    if (!entry) {
        closedir(dp);
        sret =
            GetTempStrMemory();  // this will last for the life of the command
        sret[0] = 0;
        CtoM(sret);  // convert to a MMBasic style string
    } else {
        sret =
            GetTempStrMemory();  // this will last for the life of the command
        strcpy(sret, entry->d_name);
        CtoM(sret);  // convert to a MMBasic style string
    }
    targ = T_STR;
}
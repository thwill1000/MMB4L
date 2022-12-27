/*
 * Copyright (c) 2022 Thomas Hugo Williams
 * License MIT <https://opensource.org/licenses/MIT>
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h> // Needed for EXPECT_THAT.

extern "C" {

#include "../../Hardware_Includes.h"
#include "../MMBasic.h"
#include "../Commands.h"
#include "../FunTable.h"
#include "../mmbasic_core_xtra.h"
#include "../../common/variables.h"

char error_msg[256];

void CheckAbort(void) { }

void cmd_read_clear_cache()  { }

/** Write a NULL terminated stirng to the console. */
void console_puts(const char *s) {
}

void error_init(ErrorState *error_state) { }

void error_throw(MmResult error) {
    error_throw_ex(error, mmresult_to_string(error));
}

void error_throw_ex(MmResult error, const char *msg, ...) {
    strcpy(error_msg, msg);
}

void error_throw_legacy(const char *msg, ...) {
    strcpy(error_msg, msg);
}

void file_close_all(void) { }

bool interrupt_check(void) {
    return false;
}

void interrupt_clear(void) { }

// Declared in MMBasic.c
extern struct s_funtbl funtbl[MAXSUBFUN];

// Declared in "main.c"
char *CFunctionFlash;
char *CFunctionLibrary;
char **FontTable;
ErrorState *mmb_error_state_ptr = &mmb_normal_error_state;
Options mmb_options;
ErrorState mmb_normal_error_state;

// Declared in "Commands.c"
char DimUsed;
int doindex;
struct s_dostack dostack[MAXDOLOOPS];
const char *errorstack[MAXGOSUB];
int forindex;
struct s_forstack forstack[MAXFORLOOPS + 1];
int gosubindex;
const char *gosubstack[MAXGOSUB];
int TraceBuffIndex;
const char *TraceBuff[TRACE_BUFF_SIZE];
int TraceOn;

} // extern "C"

class MmBasicCoreTest : public ::testing::Test {

protected:

    void SetUp() override {
        variables_init_called = false;
        InitBasic();
        ClearRuntime();
        error_msg[0] = '\0';
        m_program[0] = '\0';
        VarIndex = 999;
    }

    void TearDown() override {
    }

    char m_program[256];

};

TEST_F(MmBasicCoreTest, FunctionTableHash) {
    char name[MAXVARLEN + 1];
    HASH_TYPE hash;

    sprintf(m_program, "foo");
    int actual = mmb_function_table_hash(m_program, name, &hash);

    EXPECT_EQ(0, actual);
    EXPECT_STREQ("FOO", name);
    EXPECT_EQ(503, hash);

    sprintf(m_program, "bar");
    actual = mmb_function_table_hash(m_program, name, &hash);

    EXPECT_EQ(0, actual);
    EXPECT_STREQ("BAR", name);
    EXPECT_EQ(122, hash);
}

TEST_F(MmBasicCoreTest, FunctionTableHash_GivenMaximumLengthName) {
    char name[MAXVARLEN + 1];
    HASH_TYPE hash;

    sprintf(m_program, "_32_character_name_9012345678901");
    int actual = mmb_function_table_hash(m_program, name, &hash);

    EXPECT_EQ(0, actual);
    EXPECT_STREQ("_32_CHARACTER_NAME_9012345678901", name);
    EXPECT_EQ(479, hash);
}

TEST_F(MmBasicCoreTest, FunctionTableHash_GivenNameTooLong) {
    char name[MAXVARLEN + 1];
    HASH_TYPE hash;

    sprintf(m_program, "_33_character_name_90123456789012");
    int actual = mmb_function_table_hash(m_program, name, &hash);

    EXPECT_EQ(-1, actual);
    EXPECT_STREQ("_33_CHARACTER_NAME_9012345678901", name);
    EXPECT_EQ(24, hash);
}

TEST_F(MmBasicCoreTest, FunctionTablePrepare) {
    sprintf(m_program,
            "# foo\n"
            "#\n"
            "# bar\n"
            "#\n");
    subfun[0] = m_program;
    subfun[1] = m_program + 8;
    subfun[2] = NULL;

    mmb_function_table_prepare(true);

    EXPECT_EQ(2, mmb_function_table_size());
    EXPECT_STREQ("", funtbl[0].name);
    EXPECT_EQ(0, funtbl[0].index);
    EXPECT_STREQ("BAR", funtbl[122].name);
    EXPECT_EQ(1, funtbl[122].index);
    EXPECT_STREQ("FOO", funtbl[503].name);
    EXPECT_EQ(0, funtbl[503].index);
}

TEST_F(MmBasicCoreTest, FunctionTablePrepare_GivenNameTooLong) {
    sprintf(m_program,
            "# name_32_characters_xxxxxxxxxxxxx\n"
            "#\n"
            "# name_33_characters_zzzzzzzzzzzzzz\n"
            "#\n");
    subfun[0] = m_program;
    subfun[1] = m_program + 37;
    subfun[2] = NULL;

    mmb_function_table_prepare(true);

    EXPECT_EQ(2, mmb_function_table_size());
    EXPECT_STREQ("", funtbl[0].name);
    EXPECT_EQ(0, funtbl[0].index);
    EXPECT_STREQ("NAME_32_CHARACTERS_XXXXXXXXXXXXX", funtbl[308].name);
    EXPECT_EQ(0, funtbl[308].index);
    EXPECT_STREQ("NAME_33_CHARACTERS_ZZZZZZZZZZZZZ", funtbl[431].name);
    EXPECT_EQ(1, funtbl[431].index);
    EXPECT_STREQ("SUB/FUNCTION name too long", error_msg);
}

TEST_F(MmBasicCoreTest, FunctionTablePrepare_GivenDuplicateName) {
    sprintf(m_program,
            "# foo\n"
            "#\n"
            "# fOo\n"
            "#\n");
    subfun[0] = m_program;
    subfun[1] = m_program + 8;
    subfun[2] = NULL;

    mmb_function_table_prepare(true);

    EXPECT_EQ(1, mmb_function_table_size());
    EXPECT_STREQ("", funtbl[0].name);
    EXPECT_EQ(0, funtbl[0].index);
    EXPECT_STREQ("FOO", funtbl[503].name);
    EXPECT_EQ(0, funtbl[503].index);
    EXPECT_STREQ("Duplicate SUB/FUNCTION declaration", error_msg);
}

TEST_F(MmBasicCoreTest, FunctionTableFind) {
    sprintf(m_program,
            "# foo\n"
            "#\n"
            "# bar\n"
            "#\n");
    subfun[0] = m_program;
    subfun[1] = m_program + 8;
    subfun[2] = NULL;

    mmb_function_table_prepare(true);

    EXPECT_EQ(0,  mmb_function_table_find("Foo("));
    EXPECT_EQ(1,  mmb_function_table_find("BAr "));
    EXPECT_EQ(-1, mmb_function_table_find("WOMBAT("));
    EXPECT_STREQ("", error_msg);
    EXPECT_EQ(-1, mmb_function_table_find("name_33_characters_zzzzzzzzzzzzzz"));
    EXPECT_STREQ("SUB/FUNCTION name too long", error_msg);
}

TEST_F(MmBasicCoreTest, FindVar_GivenNoExplicitType) {
    sprintf(m_program, "foo = 1");
    void *actual = findvar(m_program, V_FIND);

    EXPECT_EQ(&vartbl[0].val, actual);
    EXPECT_EQ(0, VarIndex);
    EXPECT_STREQ("", error_msg);
    EXPECT_EQ(0, vartbl[0].level);
    EXPECT_STREQ("FOO", vartbl[0].name);
    EXPECT_EQ(T_NBR, vartbl[0].type);
    EXPECT_EQ(1, varcnt);
}

TEST_F(MmBasicCoreTest, FindVar_GivenTypeSuffix) {
    // Make an INTEGER variable.
    sprintf(m_program, "my_int%% = 1");
    void *actual = findvar(m_program, V_FIND);

    EXPECT_EQ(0, VarIndex);
    EXPECT_STREQ("MY_INT", vartbl[0].name);
    EXPECT_EQ(0, vartbl[0].level);
    EXPECT_EQ(T_INT, vartbl[0].type);
    EXPECT_EQ(&vartbl[0].val, actual);
    EXPECT_STREQ("", error_msg);

    // Make a FLOAT variable.
    sprintf(m_program, "my_float! = 1");
    actual = findvar(m_program, T_NBR);

    EXPECT_EQ(1, VarIndex);
    EXPECT_STREQ("MY_FLOAT", vartbl[1].name);
    EXPECT_EQ(0, vartbl[1].level);
    EXPECT_EQ(T_NBR, vartbl[1].type);
    EXPECT_EQ(&vartbl[1].val, actual);
    EXPECT_STREQ("", error_msg);

    // Make a STRING variable.
    sprintf(m_program, "my_string$ = \"wombat\"");
    actual = findvar(m_program, T_STR);

    EXPECT_EQ(2, VarIndex);
    EXPECT_STREQ("MY_STRING", vartbl[2].name);
    EXPECT_EQ(0, vartbl[2].level);
    EXPECT_EQ(T_STR, vartbl[2].type);
    EXPECT_EQ(vartbl[2].val.s, actual);
    EXPECT_STREQ("", error_msg);

    EXPECT_EQ(3, varcnt);
}

TEST_F(MmBasicCoreTest, FindVar_GivenImpliedType) {
    // Make an INTEGER variable.
    sprintf(m_program, "my_int = 1");
    void *actual = findvar(m_program, T_IMPLIED | T_INT);

    EXPECT_EQ(&vartbl[0].val, actual);
    EXPECT_EQ(0, VarIndex);
    EXPECT_STREQ("", error_msg);
    EXPECT_EQ(0, vartbl[0].level);
    EXPECT_STREQ("MY_INT", vartbl[0].name);
    EXPECT_EQ(T_IMPLIED | T_INT, vartbl[0].type);

    // Make a FLOAT variable.
    sprintf(m_program, "my_float = 1");
    actual = findvar(m_program, T_IMPLIED | T_NBR);

    EXPECT_EQ(&vartbl[1].val, actual);
    EXPECT_EQ(1, VarIndex);
    EXPECT_STREQ("", error_msg);
    EXPECT_EQ(0, vartbl[1].level);
    EXPECT_STREQ("MY_FLOAT", vartbl[1].name);
    EXPECT_EQ(T_IMPLIED | T_NBR, vartbl[1].type);

    // Make a STRING variable.
    sprintf(m_program, "my_string = \"wombat\"");
    actual = findvar(m_program, T_IMPLIED | T_STR);

    EXPECT_EQ(vartbl[2].val.s, actual);
    EXPECT_EQ(2, VarIndex);
    EXPECT_STREQ("", error_msg);
    EXPECT_EQ(0, vartbl[2].level);
    EXPECT_STREQ("MY_STRING", vartbl[2].name);
    EXPECT_EQ(T_IMPLIED | T_STR, vartbl[2].type);

    EXPECT_EQ(3, varcnt);
}

TEST_F(MmBasicCoreTest, FindVar_GivenMaxNameLen) {
    sprintf(m_program, "_32_characters_long9012345678901 = 1");
    void *actual = findvar(m_program, V_FIND);

    EXPECT_EQ(&vartbl[0].val, actual);
    EXPECT_EQ(0, VarIndex);
    EXPECT_STREQ("", error_msg);
    EXPECT_EQ(0, vartbl[0].level);
    EXPECT_EQ(T_NBR, vartbl[0].type);

    // The name is not null terminated when it is the maximum allowed length (32 chars).
    EXPECT_EQ(0, memcmp("_32_CHARACTERS_LONG9012345678901", vartbl[0].name, 32));
    EXPECT_EQ(1, varcnt);
}

TEST_F(MmBasicCoreTest, FindVar_GivenNameTooLong) {
    sprintf(m_program, "_33_characters_long90123456789012 = 1");
    void *actual = findvar(m_program, V_FIND);

    EXPECT_STREQ("Variable name too long", error_msg);
    EXPECT_EQ(0, varcnt);
}

TEST_F(MmBasicCoreTest, FindVar_GivenExists) {
    // First findvar() creates new variables.
    sprintf(m_program, "foo = 1");
    EXPECT_EQ(&vartbl[0].val, findvar(m_program, V_FIND));
    sprintf(m_program, "bar = 1");
    EXPECT_EQ(&vartbl[1].val, findvar(m_program, V_FIND));

    // Second findvar() finds them.
    sprintf(m_program, "foo = 2");
    EXPECT_EQ(&vartbl[0].val, findvar(m_program, V_FIND));
    sprintf(m_program, "bar = 1");
    EXPECT_EQ(&vartbl[1].val, findvar(m_program, V_FIND));

    EXPECT_EQ(2, varcnt);
}

TEST_F(MmBasicCoreTest, FindVar_GivenThrowErrorIfNotExist) {
    sprintf(m_program, "foo = 1");
    void *actual = findvar(m_program, V_NOFIND_ERR);

    EXPECT_STREQ("Cannot find $", error_msg);
    EXPECT_EQ(0, varcnt);
}

TEST_F(MmBasicCoreTest, FindVar_GivenReturnNullIfNotExist) {
    sprintf(m_program, "foo = 1");
    void *actual = findvar(m_program, V_NOFIND_NULL);

    EXPECT_EQ(NULL, actual);
    EXPECT_EQ(999, VarIndex);
    EXPECT_STREQ("", error_msg);
    EXPECT_EQ(0, varcnt);
}

TEST_F(MmBasicCoreTest, FindVar_GivenPreviouslyDeclaredWithDifferentType) {
    sprintf(m_program, "foo%% = 1");
    (void) findvar(m_program, V_FIND);

    error_msg[0] = '\0';
    sprintf(m_program, "foo!");
    (void) findvar(m_program, V_FIND);
    EXPECT_STREQ("$ already declared", error_msg);

    error_msg[0] = '\0';
    sprintf(m_program, "foo$");
    (void) findvar(m_program, V_FIND);
    EXPECT_STREQ("$ already declared", error_msg);

    error_msg[0] = '\0';
    sprintf(m_program, "foo");
    (void) findvar(m_program, V_FIND);
    EXPECT_STREQ("$ already declared", error_msg);
}

TEST_F(MmBasicCoreTest, FindVar_GivenIllegalName) {
    sprintf(m_program, "1foo = 1");
    void *actual = findvar(m_program, V_FIND);

    EXPECT_STREQ("Variable name", error_msg);
    EXPECT_EQ(0, varcnt);
}

TEST_F(MmBasicCoreTest, FindVar_GivenConflictingType) {
    // Make an INTEGER variable.
    error_msg[0] = '\0';
    sprintf(m_program, "my_int! = 1");
    void *actual = findvar(m_program, T_IMPLIED | T_INT);

    EXPECT_STREQ("Conflicting variable type", error_msg);

    // Make a FLOAT variable.
    error_msg[0] = '\0';
    sprintf(m_program, "my_float$ = 1");
    actual = findvar(m_program, T_IMPLIED | T_NBR);

    EXPECT_STREQ("Conflicting variable type", error_msg);

    // Make a STRING variable.
    error_msg[0] = '\0';
    sprintf(m_program, "my_string%% = \"wombat\"");
    actual = findvar(m_program, T_IMPLIED | T_STR);

    EXPECT_STREQ("Conflicting variable type", error_msg);

    EXPECT_EQ(0, varcnt);
}

TEST_F(MmBasicCoreTest, FindVar_GivenDimExistingVariable) {
    // Implicitly create "foo".
    sprintf(m_program, "foo = 1");
    (void) findvar(m_program, V_FIND);

    // DIM foo
    sprintf(m_program, "foo = 2");
    void *actual = findvar(m_program, V_DIM_VAR);

    EXPECT_STREQ("$ already declared", error_msg);
    EXPECT_EQ(1, varcnt);
}

TEST_F(MmBasicCoreTest, FindVar_GivenImplicitGlobalInSubroutine) {
    LocalIndex = 3;

    sprintf(m_program, "foo = 1");
    void *actual = findvar(m_program, V_FIND);

    EXPECT_EQ(&vartbl[0].val, actual);
    EXPECT_EQ(0, VarIndex);
    EXPECT_STREQ("", error_msg);
    EXPECT_EQ(0, vartbl[0].level);
    EXPECT_STREQ("FOO", vartbl[0].name);
    EXPECT_EQ(T_NBR, vartbl[0].type);
    EXPECT_EQ(1, varcnt);
}

TEST_F(MmBasicCoreTest, FindVar_GivenDimGlobalInSubroutine) {
    LocalIndex = 3;

    sprintf(m_program, "foo = 1");
    void *actual = findvar(m_program, V_DIM_VAR);

    EXPECT_EQ(&vartbl[0].val, actual);
    EXPECT_EQ(0, VarIndex);
    EXPECT_STREQ("", error_msg);
    EXPECT_EQ(0, vartbl[0].level);
    EXPECT_STREQ("FOO", vartbl[0].name);
    EXPECT_EQ(T_NBR, vartbl[0].type);
    EXPECT_EQ(1, varcnt);
}

TEST_F(MmBasicCoreTest, FindVar_GivenExplicitCreationOfLocal) {
    LocalIndex = 3;

    sprintf(m_program, "foo = 1");
    void *actual = findvar(m_program, V_LOCAL);

    EXPECT_EQ(&vartbl[0].val, actual);
    EXPECT_EQ(0, VarIndex);
    EXPECT_STREQ("", error_msg);
    EXPECT_EQ(3, vartbl[0].level);
    EXPECT_STREQ("FOO", vartbl[0].name);
    EXPECT_EQ(T_NBR, vartbl[0].type);
    EXPECT_EQ(1, varcnt);
}

TEST_F(MmBasicCoreTest, FindVar_GivenLocal_GivenLocalExists) {
    LocalIndex = 3;
    sprintf(m_program, "foo = 1");
    (void) findvar(m_program, V_LOCAL);

    sprintf(m_program, "foo = 1");
    (void) findvar(m_program, V_LOCAL);

    EXPECT_STREQ("$ already declared", error_msg);
}

TEST_F(MmBasicCoreTest, FindVar_GivenGlobalAndLocalOfSameName) {
    sprintf(m_program, "foo = 1");
    void *global_var = findvar(m_program, V_FIND); // Implicit creation of global.
    LocalIndex = 2;
    void *local_var = findvar(m_program, V_LOCAL); // Explicit creation of local.

    // Global scope.
    error_msg[0] = '\0';
    LocalIndex = 0;
    void *actual = findvar(m_program, V_FIND);
    EXPECT_EQ(global_var, actual);
    EXPECT_EQ(0, VarIndex);
    EXPECT_STREQ("", error_msg);

    // Level 1 scope.
    error_msg[0] = '\0';
    LocalIndex = 1;
    actual = findvar(m_program, V_FIND);
    EXPECT_EQ(global_var, actual);
    EXPECT_EQ(0, VarIndex);
    EXPECT_STREQ("", error_msg);

    // Level 2 scope.
    error_msg[0] = '\0';
    LocalIndex = 2;
    actual = findvar(m_program, V_FIND);
    EXPECT_EQ(local_var, actual);
    EXPECT_EQ(1, VarIndex);
    EXPECT_STREQ("", error_msg);

    // Level 3 scope.
    error_msg[0] = '\0';
    LocalIndex = 3;
    actual = findvar(m_program, V_FIND);
    EXPECT_EQ(global_var, actual);
    EXPECT_EQ(0, VarIndex);
    EXPECT_STREQ("", error_msg);
}

TEST_F(MmBasicCoreTest, FindVar_GivenFindingUndeclaredVariable_GivenExplicitOn) {
    mmb_options.explicit_type = true;

    sprintf(m_program, "foo = 1");
    void *actual = findvar(m_program, V_FIND);

    EXPECT_STREQ("$ is not declared", error_msg);
}

TEST_F(MmBasicCoreTest, FindVar_GivenCreationOfGlobal_GivenSubFunWithSameName) {
    sprintf(m_program,
            "# foo\n"
            "#\n"
            "# bar\n"
            "#\n"
            "foo = 1");
    subfun[0] = m_program;
    subfun[1] = m_program + 8;
    subfun[2] = NULL;
    mmb_function_table_prepare(true);

    error_msg[0] = '\0';
    void *actual = findvar(m_program + 16, V_DIM_VAR);
    EXPECT_STREQ("A sub/fun has the same name: $", error_msg);

    // With V_FUNCT ... though actually this never happens in production with V_DIM_VAR.
    error_msg[0] = '\0';
    actual = findvar(m_program + 16, V_DIM_VAR | V_FUNCT);
    EXPECT_STREQ("", error_msg);
}

TEST_F(MmBasicCoreTest, FindVar_GivenCreationOfLocal_GivenSubFunWithSameName) {
    sprintf(m_program,
            "# foo\n"
            "#\n"
            "# bar\n"
            "#\n"
            "foo = 1");
    subfun[0] = m_program;
    subfun[1] = m_program + 8;
    subfun[2] = NULL;
    mmb_function_table_prepare(true);

    error_msg[0] = '\0';
    LocalIndex = 3;
    void *actual = findvar(m_program + 16, V_LOCAL);
    EXPECT_STREQ("A sub/fun has the same name: $", error_msg);

    // With V_FUNCT.
    error_msg[0] = '\0';
    actual = findvar(m_program + 16, V_LOCAL | V_FUNCT);
    EXPECT_STREQ("", error_msg);
}

TEST_F(MmBasicCoreTest, FindVar_GivenDim_ReusesEmptySlot) {
    sprintf(m_program, "foo = ...");
    (void) findvar(m_program, V_DIM_VAR);
    sprintf(m_program, "bar = ...");
    (void) findvar(m_program, V_DIM_VAR);

    EXPECT_STREQ("FOO", vartbl[0].name);
    EXPECT_STREQ("BAR", vartbl[1].name);

    variables_delete(0);

    sprintf(m_program, "wombat = ...");
    void *actual = findvar(m_program, V_DIM_VAR);

    EXPECT_EQ(0, VarIndex);
    EXPECT_STREQ("WOMBAT", vartbl[0].name);
    EXPECT_EQ(0, vartbl[0].dims[0]);
    EXPECT_EQ(T_NBR, vartbl[0].type);
    EXPECT_EQ(&vartbl[0].val, actual);
    EXPECT_STREQ("", error_msg);
    EXPECT_EQ(2, varcnt);
}

TEST_F(MmBasicCoreTest, FindVar_GivenExistingInteger) {
    sprintf(m_program, "foo%% = ...");
    void *int_array = findvar(m_program, V_DIM_VAR);

    sprintf(m_program, "bar = ...");
    (void) findvar(m_program, V_DIM_VAR);

    sprintf(m_program, "foo%% = ...");
    void *actual = findvar(m_program, V_FIND);

    EXPECT_EQ(int_array, actual);
    EXPECT_EQ(0, VarIndex);
    EXPECT_STREQ("", error_msg);
    EXPECT_EQ(0, vartbl[0].dims[0]);
    EXPECT_STREQ("FOO", vartbl[0].name);
    EXPECT_EQ(T_INT, vartbl[0].type);
    EXPECT_EQ(2, varcnt);
}

TEST_F(MmBasicCoreTest, FindVar_GivenDimFloatArray) {
    sprintf(m_program, "my_array!(2) = ...");
    void *created = findvar(m_program, V_DIM_VAR);

    EXPECT_EQ(vartbl[0].val.fa, created);
    EXPECT_EQ(0, VarIndex);
    EXPECT_STREQ("", error_msg);
    EXPECT_EQ(2, vartbl[0].dims[0]);
    EXPECT_EQ(0, vartbl[0].dims[1]);
    EXPECT_STREQ("MY_ARRAY", vartbl[0].name);
    EXPECT_EQ(T_NBR, vartbl[0].type);

    sprintf(m_program, "my_array(1) = ...");
    void *found = findvar(m_program, V_FIND);

    EXPECT_EQ(created, (void *) ((uintptr_t) found - 8)); // Account for offset of element.
    EXPECT_EQ(0, VarIndex);
    EXPECT_STREQ("", error_msg);
    EXPECT_EQ(2, vartbl[0].dims[0]);
    EXPECT_EQ(0, vartbl[0].dims[1]);
    EXPECT_STREQ("MY_ARRAY", vartbl[0].name);
    EXPECT_EQ(T_NBR, vartbl[0].type);

    EXPECT_EQ(1, varcnt);
}

TEST_F(MmBasicCoreTest, FindVar_GivenDimIntegerArray) {
    sprintf(m_program, "my_array%%(2,4) = ...");
    void *created = findvar(m_program, V_DIM_VAR);

    EXPECT_EQ(vartbl[0].val.fa, created);
    EXPECT_EQ(0, VarIndex);
    EXPECT_STREQ("", error_msg);
    EXPECT_EQ(2, vartbl[0].dims[0]);
    EXPECT_EQ(4, vartbl[0].dims[1]);
    EXPECT_EQ(0, vartbl[0].dims[2]);
    EXPECT_STREQ("MY_ARRAY", vartbl[0].name);
    EXPECT_EQ(T_INT, vartbl[0].type);

    sprintf(m_program, "my_array%%(1,1) = ...");
    void *found = findvar(m_program, V_FIND);

    EXPECT_EQ(created, (void *) ((uintptr_t) found - 8 * 4)); // Account for offset of element.
    EXPECT_EQ(0, VarIndex);
    EXPECT_STREQ("", error_msg);
    EXPECT_EQ(2, vartbl[0].dims[0]);
    EXPECT_EQ(4, vartbl[0].dims[1]);
    EXPECT_EQ(0, vartbl[0].dims[2]);
    EXPECT_STREQ("MY_ARRAY", vartbl[0].name);
    EXPECT_EQ(T_INT, vartbl[0].type);

    EXPECT_EQ(1, varcnt);
}

TEST_F(MmBasicCoreTest, FindVar_GivenDimStringArray) {
    sprintf(m_program, "my_array$(2,4,6)");
    void *created = findvar(m_program, V_DIM_VAR);

    EXPECT_EQ(vartbl[0].val.fa, created);
    EXPECT_EQ(0, VarIndex);
    EXPECT_STREQ("", error_msg);
    EXPECT_EQ(2, vartbl[0].dims[0]);
    EXPECT_EQ(4, vartbl[0].dims[1]);
    EXPECT_EQ(6, vartbl[0].dims[2]);
    EXPECT_EQ(0, vartbl[0].dims[3]);
    EXPECT_STREQ("MY_ARRAY", vartbl[0].name);
    EXPECT_EQ(255, vartbl[0].size);
    EXPECT_EQ(T_STR, vartbl[0].type);

    sprintf(m_program, "my_array$(1,1,1)");
    void *found = findvar(m_program, V_FIND);

    EXPECT_EQ(created, (void *) ((uintptr_t) found - 256 * 19)); // Account for offset of element.
    EXPECT_EQ(0, VarIndex);
    EXPECT_STREQ("", error_msg);
    EXPECT_EQ(2, vartbl[0].dims[0]);
    EXPECT_EQ(4, vartbl[0].dims[1]);
    EXPECT_EQ(6, vartbl[0].dims[2]);
    EXPECT_EQ(0, vartbl[0].dims[3]);
    EXPECT_STREQ("MY_ARRAY", vartbl[0].name);
    EXPECT_EQ(255, vartbl[0].size);
    EXPECT_EQ(T_STR, vartbl[0].type);

    EXPECT_EQ(1, varcnt);
}

TEST_F(MmBasicCoreTest, FindVar_GivenDimArrayWithNoType) {
    mmb_options.default_type = T_NOTYPE;

    sprintf(m_program, "my_array = ...");
    void *actual = findvar(m_program, V_DIM_VAR);

    EXPECT_STREQ("Variable type not specified", error_msg);
}

TEST_F(MmBasicCoreTest, FindVar_GivenDimEmptyArray) {
    sprintf(m_program, "foo() = ...");
    void *actual = findvar(m_program, V_DIM_VAR);

    EXPECT_STREQ("Dimensions", error_msg);
}

TEST_F(MmBasicCoreTest, FindVar_GivenDimArrayWithTooManyDimensions) {
    sprintf(m_program, "foo(1,1,1,1,1,1,1,1,1) = ...");
    void *actual = findvar(m_program, V_DIM_VAR);

    EXPECT_STREQ("Dimensions", error_msg);
}

TEST_F(MmBasicCoreTest, FindVar_GivenDimArrayWithDimensionEqualToOptionBase) {
    mmb_options.base = 0;
    sprintf(m_program, "foo(0) = ...");
    error_msg[0] = '\0';
    void *actual = findvar(m_program, V_DIM_VAR);

    EXPECT_STREQ("Dimensions", error_msg);

    mmb_options.base = 1;
    sprintf(m_program, "bar(1) = ...");
    error_msg[0] = '\0';
    actual = findvar(m_program, V_DIM_VAR);

    EXPECT_STREQ("Dimensions", error_msg);
}

extern "C" void op_subtract();

TEST_F(MmBasicCoreTest, FindVar_GivenFindWithDimensionLessThanOptionBase) {
    error_msg[0] = '\0';
    sprintf(m_program, "foo(2,5) = ...");
    (void) findvar(m_program, V_DIM_VAR);

    // We can't just put a literal negative value for one of the bounds,
    // we need to insert the token for the subtract operator.
    char SUBTRACT_TOKEN = 0;
    for (int ii = 0; tokentbl[ii].fptr; ++ii) {
        if (tokentbl[ii].fptr == op_subtract) {
            SUBTRACT_TOKEN = ii + C_BASETOKEN;
        }
    }

    error_msg[0] = '\0';
    mmb_options.base = 0;
    sprintf(m_program, "foo(1,%c1) = ...", SUBTRACT_TOKEN);
    (void) findvar(m_program, V_FIND);
    EXPECT_STREQ("Dimensions", error_msg);

    error_msg[0] = '\0';
    mmb_options.base = 1;
    sprintf(m_program, "foo(1,0) = ...");
    (void) findvar(m_program, V_FIND);
    EXPECT_STREQ("Dimensions", error_msg);
}

TEST_F(MmBasicCoreTest, FindVar_GivenFindNonExistentEmptyArray) {
    sprintf(m_program, "foo() = ...");
    void *actual = findvar(m_program, V_FIND);

    EXPECT_STREQ("Dimensions", error_msg);
}

TEST_F(MmBasicCoreTest, FindVar_GivenFindNonExistentEmptyArray_GivenEmptyOk) {
    sprintf(m_program, "foo() = ...");
    void *actual = findvar(m_program, V_DIM_VAR | V_EMPTY_OK);

    EXPECT_EQ(vartbl[0].val.fa, actual);
    EXPECT_EQ(0, VarIndex);
    EXPECT_STREQ("", error_msg);
    EXPECT_EQ(-1, vartbl[0].dims[0]);
    EXPECT_EQ(0, vartbl[0].dims[1]);
    EXPECT_EQ(0, vartbl[0].dims[2]);
    EXPECT_EQ(0, vartbl[0].level);
    EXPECT_STREQ("FOO", vartbl[0].name);
    EXPECT_EQ(T_NBR, vartbl[0].type);
    EXPECT_EQ(1, varcnt);
}

TEST_F(MmBasicCoreTest, FindVar_GivenFindExistingEmptyArray) {
    sprintf(m_program, "foo(2,4) = ...");
    (void) findvar(m_program, V_DIM_VAR);

    sprintf(m_program, "foo() = ...");
    void *actual = findvar(m_program, V_EMPTY_OK);

    EXPECT_EQ(vartbl[0].val.fa, actual);
    EXPECT_EQ(0, VarIndex);
    EXPECT_STREQ("", error_msg);
    EXPECT_EQ(2, vartbl[0].dims[0]);
    EXPECT_EQ(4, vartbl[0].dims[1]);
    EXPECT_EQ(0, vartbl[0].dims[2]);
    EXPECT_EQ(0, vartbl[0].level);
    EXPECT_STREQ("FOO", vartbl[0].name);
    EXPECT_EQ(T_NBR, vartbl[0].type);
    EXPECT_EQ(1, varcnt);
}

TEST_F(MmBasicCoreTest, FindVar_GivenRedimArray) {
    sprintf(m_program, "foo%%(2,4) = ...");
    void *int_array = findvar(m_program, V_DIM_VAR);

    sprintf(m_program, "foo%%(1,2) = ...");
    void *actual = findvar(m_program, V_DIM_VAR);

    EXPECT_STREQ("$ already declared", error_msg);

    // Code contains this error but there is no code path to get to it that
    // does not hit "$ already declared" first.
    // EXPECT_STREQ("Cannot re dimension array", error_msg);
}

TEST_F(MmBasicCoreTest, FindVar_GivenArrayDimensionOutOfBounds) {
    sprintf(m_program, "foo%%(2,4) = ...");
    void *int_array = findvar(m_program, V_DIM_VAR);

    sprintf(m_program, "foo%%(3,4) = ...");
    void *actual = findvar(m_program, V_FIND);

    EXPECT_STREQ("Index out of bounds", error_msg);
}

TEST_F(MmBasicCoreTest, FindVar_GivenArrayDimensionMismatch) {
    sprintf(m_program, "foo%%(2,4) = ...");
    void *int_array = findvar(m_program, V_DIM_VAR);

    sprintf(m_program, "foo%%(1) = ...");
    void *actual = findvar(m_program, V_FIND);

    EXPECT_STREQ("Array dimensions", error_msg);
}

TEST_F(MmBasicCoreTest, FindVar_GivenDimStringArrayWithLength) {
    sprintf(m_program, "my_array$(5) Length 32");
    void *created = findvar(m_program, V_DIM_VAR);

    EXPECT_EQ(vartbl[0].val.fa, created);
    EXPECT_EQ(0, VarIndex);
    EXPECT_STREQ("", error_msg);
    EXPECT_EQ(5, vartbl[0].dims[0]);
    EXPECT_EQ(0, vartbl[0].dims[1]);
    EXPECT_STREQ("MY_ARRAY", vartbl[0].name);
    EXPECT_EQ(32, vartbl[0].size);
    EXPECT_EQ(T_STR, vartbl[0].type);

    sprintf(m_program, "my_array$(2)");
    void *found = findvar(m_program, V_FIND);

    EXPECT_EQ(created, (void *) ((uintptr_t) found - (32 + 1) * 2)); // Account for offset of element.
    EXPECT_EQ(0, VarIndex);
    EXPECT_STREQ("", error_msg);
    EXPECT_EQ(5, vartbl[0].dims[0]);
    EXPECT_EQ(0, vartbl[0].dims[1]);
    EXPECT_STREQ("MY_ARRAY", vartbl[0].name);
    EXPECT_EQ(32, vartbl[0].size);
    EXPECT_EQ(T_STR, vartbl[0].type);

    EXPECT_EQ(1, varcnt);
}

TEST_F(MmBasicCoreTest, FindVar_GivenDimStringArrayWithLengthTooLong) {
    sprintf(m_program, "my_array$(5) Length 256");
    (void) findvar(m_program, V_DIM_VAR);

    EXPECT_STREQ("% is invalid (valid is % to %)", error_msg);
}

TEST_F(MmBasicCoreTest, FindVar_GivenDimStringArrayWithUnexpectedText) {
    sprintf(m_program, "my_array$(5) Aardvark");
    (void) findvar(m_program, V_DIM_VAR);

    EXPECT_STREQ("Unexpected text: $", error_msg);
}

TEST_F(MmBasicCoreTest, FindVar_GivenFindEmptyArray_GivenDeclaredScalar) {
    sprintf(m_program, "foo = 1");
    (void) findvar(m_program, V_DIM_VAR);

    error_msg[0] = '\0';
    sprintf(m_program, "foo()");
    (void) findvar(m_program, V_EMPTY_OK);

    EXPECT_STREQ("Array dimensions", error_msg);
}

TEST_F(MmBasicCoreTest, FindVar_GivenTooManyDeclarations) {
    int ii;
    for (ii = 1; ; ++ii) {
        error_msg[0] = '\0';
        sprintf(m_program, "var_%d", ii);
        (void) findvar(m_program, V_DIM_VAR);
        if (*error_msg) break;
    }

    // The 1025'th request should fail.
    EXPECT_STREQ("Not enough memory", error_msg);
    EXPECT_EQ(1025, ii);
}

TEST_F(MmBasicCoreTest, FindVar_GivenArrayDimensionTooLarge) {
    sprintf(m_program, "my_array%%(32768)");
    (void) findvar(m_program, V_DIM_VAR);

    EXPECT_STREQ("Array bound exceeds maximum: %", error_msg);
}

TEST_F(MmBasicCoreTest, Tokenise_DimStatement) {
    sprintf(inpbuf, "Dim a = 1");

    tokenise(0);

    char expected[256];
    sprintf(
            expected,
            "%c%ca %c 1",
            T_NEWLINE,
            GetCommandValue("Dim"),
            GetTokenValue("="));
    EXPECT_STREQ(expected, tknbuf);
}

TEST_F(MmBasicCoreTest, Tokenise_RunStatement) {
    sprintf(inpbuf, "Run \"foo\", --base=1");

    tokenise(0);

    char expected[256];
    sprintf(
            expected,
            "%c%c\"foo\", %c%cbase%c1",
            T_NEWLINE,
            GetCommandValue("Run"),
            GetTokenValue("-"),
            GetTokenValue("-"),
            GetTokenValue("="));
    EXPECT_STREQ(expected, tknbuf);
}

extern "C" {

void cmd_autosave() { }
void cmd_call() { }
void cmd_case() { }
void cmd_cfunction() { }
void cmd_chdir() { }
void cmd_clear() { }
void cmd_close() { }
void cmd_cls() { }
void cmd_console() { }
void cmd_const() { }
void cmd_continue() { }
void cmd_copy() { }
void cmd_cursor() { }
void cmd_dim() { }
void cmd_do() { }
void cmd_dummy() { }
void cmd_edit() { }
void cmd_else() { }
void cmd_end() { }
void cmd_endfun() { }
void cmd_erase() { }
void cmd_error() { }
void cmd_execute() { }
void cmd_exit() { }
void cmd_exitfor() { }
void cmd_files() { }
void cmd_for() { }
void cmd_gosub() { }
void cmd_goto() { }
void cmd_if() { }
void cmd_inc() { }
void cmd_input() { }
void cmd_ireturn() { }
void cmd_kill() { }
void cmd_let() { }
void cmd_lineinput() { }
void cmd_list() { }
void cmd_load() { }
void cmd_loop() { }
void cmd_longstring() { }
void cmd_math() { }
void cmd_memory() { }
void cmd_mid() { }
void cmd_mkdir() { }
void cmd_new() { }
void cmd_next() { }
void cmd_null() { }
void cmd_on() { }
void cmd_open() { }
void cmd_option() { }
void cmd_pause() { }
void cmd_poke() { }
void cmd_print() { }
void cmd_quit() { }
void cmd_randomize() { }
void cmd_read() { }
void cmd_rename() { }
void cmd_restore() { }
void cmd_return() { }
void cmd_rmdir() { }
void cmd_run() { }
void cmd_seek() { }
void cmd_select() { }
void cmd_settick() { }
void cmd_settitle() { }
void cmd_sort() { }
void cmd_subfun() { }
void cmd_system() { }
void cmd_timer() { }
void cmd_trace() { }
void cmd_troff() { }
void cmd_tron() { }
void cmd_xmodem() { }
void fun_abs() { }
void fun_acos() { }
void fun_asc() { }
void fun_asin() { }
void fun_at() { }
void fun_atn() { }
void fun_bin() { }
void fun_bound() { }
void fun_call() { }
void fun_choice() { }
void fun_chr() { }
void fun_cint() { }
void fun_cos() { }
void fun_cwd() { }
void fun_date() { }
void fun_datetime() { }
void fun_day() { }
void fun_deg() { }
void fun_dir() { }
void fun_eof() { }
void fun_epoch() { }
void fun_errmsg() { }
void fun_errno() { }
void fun_eval() { }
void fun_exp() { }
void fun_field() { }
void fun_fix() { }
void fun_format() { }
void fun_hex() { }
void fun_hres() { }
void fun_inkey() { }
void fun_inputstr() { }
void fun_instr() { }
void fun_int() { }
void fun_json() { }
void fun_lcase() { }
void fun_lcompare() { }
void fun_left() { }
void fun_len() { }
void fun_lgetbyte() { }
void fun_lgetstr() { }
void fun_linstr() { }
void fun_llen() { }
void fun_loc() { }
void fun_lof() { }
void fun_log() { }
void fun_math() { }
void fun_max() { }
void fun_mid() { }
void fun_min() { }
void fun_mmcmdline() { }
void fun_mmdevice() { }
void fun_mminfo() { }
void fun_oct() { }
void fun_peek() { }
void fun_pi() { }
void fun_pos() { }
void fun_rad() { }
void fun_rgb() { }
void fun_right() { }
void fun_rnd() { }
void fun_sgn() { }
void fun_sin() { }
void fun_space() { }
void fun_sqr() { }
void fun_str() { }
void fun_string() { }
void fun_tab() { }
void fun_tan() { }
void fun_time() { }
void fun_timer() { }
void fun_ucase() { }
void fun_val() { }
void fun_version() { }
void fun_vres() { }
void op_add() { }
void op_and() { }
void op_div() { }
void op_divint() { }
void op_equal() { }
void op_exp() { }
void op_gt() { }
void op_gte() { }
void op_inv() { }
void op_invalid() { }
void op_lt() { }
void op_lte() { }
void op_mod() { }
void op_mul() { }
void op_ne() { }
void op_not() { }
void op_or() { }
void op_shiftleft() { }
void op_shiftright() { }
void op_subtract() { }
void op_xor() { }

} // extern "C"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

extern "C" {

#include "../file.h"

int error_check() { return errno; }
int MMgetchar(void) { return -1; }
char console_putc(char c) { return c; }
void serial_close(int fnbr) { }
int serial_eof(int fnbr) { return -1; }
int serial_getc(int fnbr) { return -1; }
int serial_putc(int ch, int fnbr) { return -1; }
int serial_rx_queue_size(int fnbr) { return -1; }

}

TEST(FileTest, Exists) {
    EXPECT_EQ(file_exists("/bin/vi"), true);
    EXPECT_EQ(file_exists("/bin/does-not-exist"), false);
}

TEST(FileTest, IsEmpty) {
    EXPECT_EQ(file_is_empty("/bin/vi"), 0);

    char filename[] = "/tmp/is_empty_XXXXXX";
    int fd = mkstemp(filename);
    close(fd);
    EXPECT_EQ(file_exists(filename), true);
    EXPECT_EQ(file_is_empty(filename), true);
}

TEST(FileTest, IsRegular) {
    EXPECT_EQ(file_is_regular("/bin/vi"), true);
    EXPECT_EQ(file_is_regular("/bin"), false);
}

TEST(FileTest, GetExtension) {
    const char *filename = "foo.bas";
    const char *empty = "";
    const char *extension_only = ".bas";
    const char *no_extension = "foo";
    EXPECT_STREQ(file_get_extension(filename), ".bas");
    EXPECT_STREQ(file_get_extension(empty), "");
    EXPECT_STREQ(file_get_extension(extension_only), ".bas");
    EXPECT_STREQ(file_get_extension(no_extension), "");
}

TEST(FileTest, HasSuffix) {
    EXPECT_EQ(file_has_suffix("foo.bas", ".bas", false), true);
    EXPECT_EQ(file_has_suffix("foo.bas", ".BAS", false), false);
    EXPECT_EQ(file_has_suffix("foo.bas", ".BAS", true), true);
    EXPECT_EQ(file_has_suffix("foo.bas", ".inc", true), false);
}

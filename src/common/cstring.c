/*-*****************************************************************************

MMBasic for Linux (MMB4L)

cstring.c

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

#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

#include "cstring.h"
#include "utility.h"

#include "../Version.h"  // For the ENV32BIT definition.

int cstring_cat(char *dst, const char *src, size_t dst_sz) {
    size_t dst_len = strlen(dst);
    size_t src_len = strlen(src);
    size_t n = min(dst_sz - dst_len - 1, src_len);
    if (n > 0) {
        memmove(dst + dst_len, src, n);
        dst[dst_len + n] = '\0';
    }
    return dst_len + src_len < dst_sz ? 0 : -1;
}

int cstring_cat_int64(char *dst, int64_t src, size_t dst_sz) {
    char buf[32];
#if defined(ENV32BIT)
    sprintf(buf, "%lld", src);
#else
    sprintf(buf, "%ld", src);
#endif
    return cstring_cat(dst, buf, dst_sz);
}

int cstring_cpy(char *dst, const char *src, size_t dst_sz) {
    if (dst_sz == 0) return -1;
    const char *limit = dst + dst_sz - 1;
    while (dst < limit && *src) {
        *dst++ = *src++;
    }
    *dst = '\0';
    return *src ? -1 : 0;
}

int cstring_enquote(char *s, size_t n) {
    if (!s) return -1;
    size_t len = strlen(s);
    if (len + 2 >= n) return -1;
    memmove(s + 1, s, len);
    s[0] = '"';
    s[len + 1] = '"';
    s[len + 2] = '\0';
    return 0;
}

bool cstring_isquoted(const char *s) {
    size_t len = strlen(s);
    return len > 1 && s[0] == '"' && s[len - 1] == '"';
}

int cstring_replace(char *haystack, size_t haystack_sz, const char *needle,
                    const char *replacement) {
    if (*needle == '\0') return -1;

    char *p = strstr(haystack, needle);
    if (!p) return 0; // Nothing to do.

    size_t haystack_len = strlen(haystack);
    const size_t needle_len = strlen(needle);
    const size_t replacement_len = strlen(replacement);

    while (p) {
        if (haystack_len + replacement_len - needle_len >= haystack_sz) return -1;
        memmove(p + replacement_len, p + needle_len, haystack_len - needle_len + haystack - p);
        if (replacement_len != 0) memcpy(p, replacement, replacement_len);
        haystack_len += replacement_len - needle_len;
        if (p + replacement_len >= haystack + haystack_len) break;
        p = strstr(p + replacement_len, needle);
    }
    haystack[haystack_len] = '\0';

    return 0;
}

char *cstring_tolower(char *s) {
    char *p = s;
    while (*p) {
        *p = tolower(*p);
        p++;
    }
    return s;
}

char *cstring_toupper(char *s) {
    char *p = s;
    while (*p) {
        *p = toupper(*p);
        p++;
    }
    return s;
}

char *cstring_trim(char *s) {
    if (!s) return s;
    char *start;
    for (start = s; isspace(*start); start++);
    char *end;
    for (end = s + strlen(s) - 1; end >= start && isspace(*end); end--);
    if (end >= start) memmove(s, start, end - start + 1);
    s[end - start + 1] = '\0';
    return s;
}

char *cstring_unquote(char *s) {
    int len = strlen(s);
    if (len > 1 && s[0] == '\"' && s[len - 1] == '\"') {
        memmove(s, s + 1, len - 2);
        s[len - 2] = '\0';
    }
    return s;
}

/*-*****************************************************************************

MMBasic for Linux (MMB4L)

Version.h

Copyright 2021-2025 Geoff Graham, Peter Mather and Thomas Hugo Williams.

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

#if !defined(MMB4L_VERSION_H)
#define MMB4L_VERSION_H

#if defined(__linux__)
    #define __mmb4l__
    #if defined(__ANDROID__)
        #define MM_ARCH  "Android aarch64"
        #define ENV64BIT
    #elif defined(__x86_64)
        #define MM_ARCH  "Linux x86_64"
        #define ENV64BIT
    #elif defined(__aarch64__)
        #define MM_ARCH  "Linux aarch64"
        #define ENV64BIT
    #elif defined(__arm__)
        #define MM_ARCH  "Linux armv6l"
        #define ENV32BIT
    #elif defined(__i686__)
        #define MM_ARCH  "Linux i686"
        #define ENV32BIT
    #else
        #error This architecture is not supported
    #endif
#elif defined(__riscos__)
    #define __mmb4l__
    #define MM_ARCH "RISC OS"
    #define ENV32BIT
#else
    #error This device is not supported
#endif

#if defined(__mmb4l__)
    #define MM_DEVICE     "MMB4L"
    #define MM_MAJOR      0
    #define MM_MINOR      7
    #define MM_MICRO      1
    #define BUILD_NUMBER  0  // Currently always 0.
    #define MM_VERSION    (MM_MAJOR * 1000000000) + (MM_MINOR * 10000000) + (MM_MICRO) * 10000 + BUILD_NUMBER
    #define COPYRIGHT     "Copyright 2011-2025 Geoff Graham\r\n" \
                          "Copyright 2016-2025 Peter Mather\r\n" \
                          "Copyright 2021-2025 Thomas Hugo Williams\r\n"
#else
    #error __mmb4l__ is not defined
#endif

#endif // #if !defined(MMB4L_VERSION_H)

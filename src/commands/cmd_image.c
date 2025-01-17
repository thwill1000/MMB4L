/*-*****************************************************************************

MMBasic for Linux (MMB4L)

cmd_image.c

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

#include <stdlib.h>

#include "../common/mmb4l.h"
#include "../common/graphics.h"
#include "../common/utility.h"

static MmResult cmd_image_resize(const char *p) {
    ERROR_UNIMPLEMENTED("IMAGE RESIZE");
    return kUnimplemented;
}

// TODO: Replace use with graphics_blit() instead ?
static void read_buffer_fast(MmSurface *surface, uint32_t* dst, int x1, int y1, int x2,
                                        int y2) {
    if (x2 <= x1) SWAP(int, x1, x2);
    if (y2 <= y1) SWAP(int, y1, y2);
    for (int y = y1; y <= y2; ++y) {
        uint32_t *src = surface->pixels + y * surface->width + x1;
        for (int x = x1; x <= x2; ++x) {
            if (x >= 0 && x < (int) surface->width && y >=0 && y < (int) surface->height) {
                *dst++ = *src++;
            } else {
                dst++;
                src++;
            }
        }
    }
}

/**
 * IMAGE RESIZE_FAST x, y, width, height, new_x, new_y, new_width, new_height [, src_id] [, flag]
 */
static MmResult cmd_image_resize_fast(const char *p) {
    getargs(&p, 19, ",");
    if (argc < 15) return kArgumentCount;

    const MmSurfaceId src_id = (argc >= 17 && *argv[16])
            ? getint(argv[16], 0, GRAPHICS_MAX_ID)
            : -1;
    if (src_id != -1 && !graphics_surface_exists(src_id)) return kGraphicsInvalidReadSurface;
    MmSurface *src_surface = (src_id == -1) ? graphics_current : &graphics_surfaces[src_id];
    MmSurface *write_surface = graphics_current;

    const int x = getint(argv[0], 0, src_surface->width - 1);
    const int y = getint(argv[2], 0, src_surface->height - 1);
    const int width = getint(argv[4], 1, src_surface->width - x);
    const int height = getint(argv[6], 1, src_surface->height - y);
    const int new_x = getint(argv[8], 0, write_surface->width - 1);
    const int new_y = getint(argv[10], 0, write_surface->height - 1);
    const int new_width = getint(argv[12], 1, write_surface->width - new_x);
    const int new_height = getint(argv[14], 1, write_surface->height - new_y);
    const bool transparent_black = (argc == 19) && (getint(argv[18], 0, 1) == 1);

    if (width == new_width && height == new_height && src_surface != write_surface) {
        return graphics_blit(x, y, new_x, new_y, width, height, src_surface, write_surface,
                             transparent_black ? 4 : 0, RGB_BLACK);
    } else if (width == new_width && src_surface != write_surface) {
        const float y_ratio = ((float) height / (float) new_height);
        for (int yy = 0; yy < new_height; ++yy) {
            int py = (yy * y_ratio) + y;
            return graphics_blit(x, py, new_x, new_y + yy, width, 1, src_surface, write_surface,
                                 transparent_black ? 4 : 0, RGB_BLACK);
        }
    } else {
        const int x_ratio = (int)((float) width / (float) new_width * (float)65536.0);
        const int y_ratio = (int)((float) height / (float) new_height * (float)65536.0);
        uint32_t *buffer = calloc(width * height, sizeof(uint32_t));
        read_buffer_fast(src_surface, buffer, x, y, x + width - 1, y + height - 1);
        uint32_t *start1 = write_surface->pixels + new_y * write_surface->width + new_x;
        uint32_t *cout1;
        for (int i = 0; i < new_height; i++) {
            cout1 = start1;
            int py = (i * y_ratio) >> 16;
            for (int j = 0; j < new_width; j++) {
                int px = (j * x_ratio) >> 16;
                uint32_t c = buffer[px + py * width];
                if (!transparent_black || c) {
                    *cout1 = c;
                }
                cout1++;
            }
            start1 += write_surface->width;
        }
        free(buffer);
        write_surface->dirty = true;
    }

    return kOk;
}

static MmResult cmd_image_rotate(const char *p) {
    ERROR_UNIMPLEMENTED("IMAGE ROTATE");
    return kUnimplemented;
}

static MmResult cmd_image_rotate_fast(const char *p) {
    ERROR_UNIMPLEMENTED("IMAGE ROTATE FAST");
    return kUnimplemented;
}

static MmResult cmd_image_warp_h(const char *p) {
    ERROR_UNIMPLEMENTED("IMAGE WARP H");
    return kUnimplemented;
}

static MmResult cmd_image_warp_v(const char *p) {
    ERROR_UNIMPLEMENTED("IMAGE WARP V");
    return kUnimplemented;
}

void cmd_image(void) {
    if (!graphics_current) error_throw(kGraphicsInvalidWriteSurface);

    MmResult result = kOk;
    const char *p;
    if ((p = checkstring(cmdline, "RESIZE"))) {
        result = cmd_image_resize(p);
    } else if ((p = checkstring(cmdline, "RESIZE_FAST"))) {
        result = cmd_image_resize_fast(p);
    } else if ((p = checkstring(cmdline, "RESIZE FAST"))) {
        result = cmd_image_resize_fast(p);
    } else if ((p = checkstring(cmdline, "ROTATE"))) {
        result = cmd_image_rotate(p);
    } else if ((p = checkstring(cmdline, "ROTATE_FAST"))) {
        result = cmd_image_rotate_fast(p);
    } else if ((p = checkstring(cmdline, "ROTATE FAST"))) {
        result = cmd_image_rotate_fast(p);
    } else if ((p = checkstring(cmdline, "WARP_H"))) {
        result = cmd_image_warp_h(p);
    } else if ((p = checkstring(cmdline, "WARP H"))) {
        result = cmd_image_warp_h(p);
    } else if ((p = checkstring(cmdline, "WARP_V"))) {
        result = cmd_image_warp_v(p);
    } else if ((p = checkstring(cmdline, "WARP V"))) {
        result = cmd_image_warp_v(p);
    } else {
        ERROR_UNKNOWN_SUBCOMMAND("IMAGE");
        result = kSyntax;
    }
    ON_FAILURE_ERROR(result);
}

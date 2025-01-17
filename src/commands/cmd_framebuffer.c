/*-*****************************************************************************

MMBasic for Linux (MMB4L)

cmd_framebuffer.c

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

#include <strings.h>

#include "../common/graphics.h"
#include "../common/mmb4l.h"
#include "../common/utility.h"

/** FRAMEBUFFER CLOSE [{F|L}] */
static MmResult cmd_framebuffer_close(const char *p) {
    skipspace(p);
    MmResult result = kOk;
    MmSurface *surfaceF = &graphics_surfaces[GRAPHICS_SURFACE_F];
    MmSurface *surfaceL = &graphics_surfaces[GRAPHICS_SURFACE_L];
    if (parse_is_end(p)) {
        result = graphics_surface_destroy(surfaceF);
        if (SUCCEEDED(result)) result = graphics_surface_destroy(surfaceL);
    } else {
        MmSurfaceId page_id = -1;
        result = parse_page(p, &page_id);
        if (SUCCEEDED(result)) {
            switch (page_id) {
                case GRAPHICS_SURFACE_N:
                    result = kGraphicsInvalidSurface;
                    break;
                case GRAPHICS_SURFACE_F:
                    result = graphics_surface_destroy(surfaceF);
                    break;
                case GRAPHICS_SURFACE_L:
                    result = graphics_surface_destroy(surfaceL);
                    break;
                default:
                    result = kInternalFault;
                    break;
            }
        }
    }

    return result;
}

/** FRAMEBUFFER COPY from, to [, B] */
static MmResult cmd_framebuffer_copy(const char *p) {
    getargs(&p, 5, ",");
    if (argc != 3 && argc != 5) return kArgumentCount;

    MmSurfaceId src_id = -1;
    MmResult result = parse_read_page(argv[0], &src_id);
    if (FAILED(result)) return result;
    MmSurface* src_surface = &graphics_surfaces[src_id];

    MmSurfaceId dst_id = -1;
    result = parse_write_page(argv[2], &dst_id);
    if (FAILED(result)) return result;
    MmSurface* dst_surface = &graphics_surfaces[dst_id];

    // MMB4L ignores the background flag B for the moment.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
    bool background = false;
#pragma GCC diagnostic pop
    if (argc == 5) {
        const char *tp;
        if ((tp = checkstring(argv[4], "B"))) {
            background = true;
        } else { // Allow string expression.
            const char *s= getCstring(argv[4]);
            if (strcasecmp(s, "B")) {
                background = true;
            } else {
                return kSyntax;
            }
        }
    }

    if (src_surface->width != dst_surface->width || src_surface->height != dst_surface->height) {
        return kGraphicsSurfaceSizeMismatch;
    }

    if (src_surface == dst_surface) {
        return kGraphicsReadAndWriteSurfaceSame;
    }

    return graphics_blit(0, 0, 0, 0, src_surface->width, src_surface->height, src_surface,
                         dst_surface, 0x0, -1);
}

/** FRAMEBUFFER CREATE */
static MmResult cmd_framebuffer_create(const char *p) {
    skipspace(p);
    if (!parse_is_end(p)) return kUnexpectedText;
    return graphics_buffer_create(
        GRAPHICS_SURFACE_F,
        graphics_surfaces[GRAPHICS_SURFACE_N].width,
        graphics_surfaces[GRAPHICS_SURFACE_N].height);
}

/** FRAMEBUFFER LAYER [transparent] */
static MmResult cmd_framebuffer_layer(const char *p) {
    skipspace(p);
    uint8_t transparent = parse_is_end(p) ? 0 : getint(p, 0, 15);

    ON_FAILURE_RETURN(graphics_buffer_create(
        GRAPHICS_SURFACE_L,
        graphics_surfaces[GRAPHICS_SURFACE_N].width,
        graphics_surfaces[GRAPHICS_SURFACE_N].height));

    MmSurface *layer = &graphics_surfaces[GRAPHICS_SURFACE_L];
    layer->transparent = GRAPHICS_RGB121_COLOURS[transparent];
    return graphics_draw_rectangle(layer, 0, 0, layer->width - 1, layer->height - 1,
                                   layer->transparent);
}

/** FRAMEBUFFER MERGE [colour] [, mode] [, update rate] */
static MmResult cmd_framebuffer_merge(const char *p) {
    getargs(&p, 5, ",");
    if (argc == 2 || argc == 4) return kArgumentCount;

    uint8_t transparent = (argc > 0) ? getint(argv[0], 0, 15) : 0;
    // MMB4L ignores the 'mode' and 'update rate' arguments for the moment.

    // Copy F to N.
    MmSurface* src_surface = &graphics_surfaces[GRAPHICS_SURFACE_F];
    MmSurface* dst_surface = &graphics_surfaces[GRAPHICS_SURFACE_N];
    MmResult result = graphics_blit(0, 0, 0, 0, src_surface->width, src_surface->height,
                                    src_surface, dst_surface, 0x0, RGB_BLACK);
    if (FAILED(result)) return result;

    // Merge L with N.
    src_surface = &graphics_surfaces[GRAPHICS_SURFACE_L];
    return graphics_blit(0, 0, 0, 0, src_surface->width, src_surface->height, src_surface,
                         dst_surface, 0x4, GRAPHICS_RGB121_COLOURS[transparent]);
}

/** FRAMEBUFFER WAIT */
static MmResult cmd_framebuffer_wait(const char *p) {
    skipspace(p);
    if (!parse_is_end(p)) return kUnexpectedText;
    // Empty implementation for now.
    // On the real PicoMiteVGA this waits until the start of the next frame blanking period.
    return kOk;
}

/** FRAMEBUFFER WRITE {N|F|L} */
static MmResult cmd_framebuffer_write(const char *p) {
    getargs(&p, 1, ",");
    if (argc != 1) return kArgumentCount;

    MmSurfaceId dst_id = -1;
    MmResult result = parse_write_page(argv[0], &dst_id);
    if (SUCCEEDED(result)) result = graphics_surface_write(dst_id);
    return result;
}

void cmd_framebuffer(void) {
    if (mmb_options.simulate != kSimulateGameMite
            && mmb_options.simulate != kSimulatePicoMiteVga) {
        ON_FAILURE_ERROR(kUnsupportedOnCurrentDevice);
    }

    MmResult result = kOk;
    const char *p;
    if ((p = checkstring(cmdline, "CLOSE"))) {
        result = cmd_framebuffer_close(p);
    } else if ((p = checkstring(cmdline, "COPY"))) {
        result = cmd_framebuffer_copy(p);
    } else if ((p = checkstring(cmdline, "CREATE"))) {
        result = cmd_framebuffer_create(p);
    } else if ((p = checkstring(cmdline, "LAYER"))) {
        result = cmd_framebuffer_layer(p);
    } else if ((p = checkstring(cmdline, "MERGE"))) {
        result = cmd_framebuffer_merge(p);
    } else if ((p = checkstring(cmdline, "WAIT"))) {
        result = cmd_framebuffer_wait(p);
    } else if ((p = checkstring(cmdline, "WRITE"))) {
        result = cmd_framebuffer_write(p);
    } else {
        ERROR_UNKNOWN_SUBCOMMAND("FRAMEBUFFER");
    }
    ON_FAILURE_ERROR(result);
}

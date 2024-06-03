/*-*****************************************************************************

MMBasic for Linux (MMB4L)

graphics.c

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

#include "bitset.h"
#include "cstring.h"
#include "error.h"
#include "events.h"
#include "file.h"
#include "fonttbl.h"
#include "graphics.h"
#include "memory.h"
#include "mmb4l.h"
#include "path.h"
#include "sprite.h"
#include "upng.h"
#include "utility.h"
#include "../third_party/spbmp.h"

#include <assert.h>
#include <stdbool.h>

#include <SDL.h>

#define HRes graphics_current->width
#define VRes graphics_current->height

/** Sprite colours on CMM2. */
const MmGraphicsColour GRAPHICS_CMM2_SPRITE_COLOURS[] = {
    RGB_BLACK,
    RGB_BLUE,
    RGB_GREEN,
    RGB_CYAN,
    RGB_RED,
    RGB_MAGENTA,
    RGB_YELLOW,
    RGB_WHITE,
    RGB_MYRTLE,
    RGB_COBALT,
    RGB_MIDGREEN,
    RGB_CERULEAN,
    RGB_RUST,
    RGB_FUCHSIA,
    RGB_BROWN,
    RGB_LILAC,
};

/** Sprite colours in CMM2 order but adjusted to RGB121. */
const MmGraphicsColour GRAPHICS_CMM2_SPRITE_COLOURS_RGB121[] = {
    RGB_BLACK,
    RGB_BLUE,
    RGB_GREEN,
    RGB_CYAN,
    RGB_RED,
    RGB_MAGENTA_4BIT,
    RGB_YELLOW,
    RGB_WHITE,
    RGB_MYRTLE,
    RGB_COBALT,
    RGB_MIDGREEN,
    RGB_CERULEAN,
    RGB_RUST,
    RGB_FUCHSIA,
    RGB_BROWN_4BIT,
    RGB_LILAC,
};

/** PicoMite RGB121 colours. */
const MmGraphicsColour GRAPHICS_RGB121_COLOURS[] = {
    RGB_BLACK,
    RGB_BLUE,
    RGB_MYRTLE,
    RGB_COBALT,
    RGB_MIDGREEN,
    RGB_CERULEAN,
    RGB_GREEN,
    RGB_CYAN,
    RGB_RED,
    RGB_MAGENTA_4BIT,
    RGB_RUST,
    RGB_FUCHSIA,
    RGB_BROWN_4BIT,
    RGB_LILAC,
    RGB_YELLOW,
    RGB_WHITE,
};

typedef struct {
    uint32_t id;
    uint32_t width;
    uint32_t height;
    uint32_t num_pages;
    uint32_t font;
} ModeDefinition;

static const ModeDefinition CMM2_MODES[] = {
    { 0, 0, 0, 0, 0},
    { 1, 800, 600, 7, 1 },
    { 2, 640, 400, 14, 1 },
    { 3, 320, 200, 58, 7 },
    { 4, 480, 432, 18, 1 },
    { 5, 240, 216, 61, 1 },
    { 6, 256, 240, 61, 1 },
    { 7, 320, 240, 47, 1 },
    { 8, 640, 480, 11, 1 },
    { 9, 1024, 768, 4, 1 },
    { 10, 848, 480, 8, 1 },
    { 11, 1280, 720, 3, 1 },
    { 12, 960, 540, 5, 1 },
    { 13, 400, 300, 29, 1 },
    { 14, 960, 540, 7, 1 },
    { 15, 1280, 1024, 5, 1 },
    { 16, 1920, 1080, 3, 1 },
    { 17, 384, 240, 39, 1 },
};

static const ModeDefinition PICOMITE_VGA_MODES[] = {
    { 0, 0, 0, 0, 0 },
    { 1, 640, 480, 1, 1 },
    { 2, 320, 240, 1, 7 },
};

static const char* NO_ERROR = "";
static bool graphics_initialised = false;
MmSurface graphics_surfaces[GRAPHICS_MAX_SURFACES] = { 0 };
MmSurface* graphics_current = NULL;
MmGraphicsColour graphics_fcolour = RGB_WHITE;
MmGraphicsColour graphics_bcolour = RGB_BLACK;
uint32_t graphics_font = 1;
static uint64_t frameEnd = 0;

MmResult graphics_init() {
    if (graphics_initialised) return kOk;
    MmResult result = events_init();
    if (FAILED(result)) return result;
    for (MmSurfaceId id = 0; id <= GRAPHICS_MAX_ID; ++id) {
        memset(&graphics_surfaces[id], 0, sizeof(MmSurface));
        graphics_surfaces[id].id = id;
    }
    frameEnd = 0;
    result = sprite_init();
    if (FAILED(result)) return result;
    graphics_initialised = true;
    return kOk;
}

const char *graphics_last_error() {
    const char* emsg = SDL_GetError();
    return emsg && *emsg ? emsg : NO_ERROR;
}

MmResult graphics_term() {
    if (!graphics_initialised) return kOk;
    MmResult result = sprite_term();
    if (SUCCEEDED(result)) result = graphics_surface_destroy_all();
    if (SUCCEEDED(result)) {
        graphics_fcolour = RGB_WHITE;
        graphics_bcolour = RGB_BLACK;
        graphics_initialised = false;
    }
    return result;
}

MmSurfaceId graphics_find_window(uint32_t sdl_window_id) {
    for (MmSurfaceId id = 0; id <= GRAPHICS_MAX_ID; ++id) {
        if (graphics_surfaces[id].type == kGraphicsWindow
                && SDL_GetWindowID(graphics_surfaces[id].window) == sdl_window_id) {
            return id;
        }
    }
    return -1;
}

/** TODO */
static MmResult graphics_refresh_gamemite_window() {
    MmSurface *buffer_N = &graphics_surfaces[GRAPHICS_SURFACE_N];
    if (!buffer_N->dirty) return kOk;
    MmResult result = kOk;
    if (graphics_surface_exists(GRAPHICS_SURFACE_N)) {
        MmSurface *window = &graphics_surfaces[0];
        result = graphics_blit(0, 0, 0, 0, 320, 240, buffer_N, window, 0x0, RGB_BLACK);
    }
    if (SUCCEEDED(result)) buffer_N->dirty = false;
    return kOk;
}

/** TODO */
static MmResult graphics_refresh_picomite_vga_window() {
    MmSurface *buffer_N = &graphics_surfaces[GRAPHICS_SURFACE_N];
    MmSurface *buffer_L = &graphics_surfaces[GRAPHICS_SURFACE_L];

    if (!buffer_N->dirty && !buffer_L->dirty) return kOk;

    MmSurface *window = &graphics_surfaces[0];
    const uint32_t w = buffer_N->width;
    const uint32_t h = buffer_N->height;
    MmResult result = kOk;

    if (graphics_surface_exists(GRAPHICS_SURFACE_N)) {
        result = graphics_blit(0, 0, 0, 0, w, h, buffer_N, window, 0x0, RGB_BLACK);
    }

    if (SUCCEEDED(result) && graphics_surface_exists(GRAPHICS_SURFACE_L)) {
        result = graphics_blit(0, 0, 0, 0, w, h, buffer_L, window, 0x4, buffer_L->transparent);
    }

    if (SUCCEEDED(result)) {
        buffer_N->dirty = false;
        buffer_L->dirty = false;
    }

    return result;
}

void graphics_refresh_windows() {
    // if (SDL_GetTicks64() > frameEnd) {
    if (SDL_GetTicks() > frameEnd) {
        switch (mmb_options.simulate) {
            case kSimulateGameMite:
                ERROR_ON_FAILURE(graphics_refresh_gamemite_window());
                break;
            case kSimulatePicoMiteVga:
                ERROR_ON_FAILURE(graphics_refresh_picomite_vga_window());
                break;
            default:
                break;
        }

        // TODO: Optimise by using linked-list of windows.
        for (int id = 0; id <= GRAPHICS_MAX_ID; ++id) {
            MmSurface* s = &graphics_surfaces[id];
            if (s->type == kGraphicsWindow && s->dirty) {
                SDL_UpdateTexture((SDL_Texture *) s->texture, NULL, s->pixels, s->width * 4);
                SDL_RenderCopy((SDL_Renderer *) s->renderer, (SDL_Texture *) s->texture, NULL,
                               NULL);
                SDL_RenderPresent((SDL_Renderer *) s->renderer);
                s->dirty = false;
            }
        }
        // frameEnd = SDL_GetTicks64() + 15;
        frameEnd = SDL_GetTicks() + 15;
    }
}

static inline MmSurface *graphics_surface_from_id(MmSurfaceId id) {
    if (id == -1) return NULL;
    assert(id >= 0 && id <= GRAPHICS_MAX_ID);
    return &graphics_surfaces[id];
}

/** Gets the ID of the next active sprite. */
static MmSurfaceId graphics_next_active_sprite(MmSurfaceId start) {
    assert(start >= 0 && start <= GRAPHICS_MAX_ID);
    for (MmSurfaceId id = start; id <= GRAPHICS_MAX_ID; ++id) {
        if (graphics_surfaces[id].type == kGraphicsSprite) {
            return id;
        }
    }
    return -1;
}

static MmResult graphics_surface_create(MmSurfaceId id, GraphicsSurfaceType type, int width,
                                        int height) {
    if (!graphics_initialised) {
        MmResult result = graphics_init();
        if (FAILED(result)) return result;
    }

    if (id < 0 || id > GRAPHICS_MAX_ID) return kGraphicsInvalidId;
    if (graphics_surfaces[id].type != kGraphicsNone) return kGraphicsSurfaceAlreadyExists;
    if (width > WINDOW_MAX_WIDTH || height > WINDOW_MAX_HEIGHT) return kGraphicsSurfaceTooLarge;

    MmSurface *s = &graphics_surfaces[id];
    s->id = id;
    s->type = type;
    s->window = NULL;
    s->renderer = NULL;
    s->texture = NULL;
    s->dirty = false;
    s->pixels = calloc(width * height, sizeof(uint32_t));
    if (!s->pixels) return kOutOfMemory;
    s->background = NULL;
    s->height = height;
    s->width = width;
    s->interrupt_addr = NULL;
    s->transparent = -1;
    s->background = NULL;
    s->x = GRAPHICS_OFF_SCREEN;
    s->y = GRAPHICS_OFF_SCREEN;
    s->next_x = GRAPHICS_OFF_SCREEN;
    s->next_y = GRAPHICS_OFF_SCREEN;
    s->layer = 0xFF;
    s->edge_collisions = 0x0;
    bitset_reset(s->sprite_collisions, 256);

    return kOk;
}

MmResult graphics_buffer_create(MmSurfaceId id, int width, int height) {
    return graphics_surface_create(id, kGraphicsBuffer, width, height);
}

MmResult graphics_sprite_create(MmSurfaceId id, int width, int height) {
    MmResult result = graphics_surface_create(id, kGraphicsInactiveSprite, width, height);
    MmSurface *s = &graphics_surfaces[id];

    if (SUCCEEDED(result)) {
        s->background = calloc(width * height, sizeof(uint32_t));
        if (!s->background) result = kOutOfMemory;
    }

    if (FAILED(result)) {
        free(s->pixels);
        s->pixels = NULL;
        free(s->background);
        s->background = NULL;
    }

    return result;
}

MmResult graphics_window_create(MmSurfaceId id, int x, int y, int width, int height, int scale,
                                const char *title, const char *interrupt_addr) {
    MmResult result = graphics_surface_create(id, kGraphicsWindow, width, height);
    MmSurface *s = &graphics_surfaces[id];

    // Reduce scale to fit display.
    float fscale = scale;
    if (SUCCEEDED(result)) {
        SDL_DisplayMode dm;
        if (SUCCEEDED(SDL_GetCurrentDisplayMode(0, &dm))) {
            while (width * fscale > dm.w * 0.9 || height * fscale > dm.h * 0.9) {
                fscale -= fscale > 2.0 ? 1.0 : 0.1;
                if (fscale < 0.1) return kGraphicsSurfaceTooLarge;
            }
        } else {
            result = kGraphicsApiError;
        }
    }

    // Create SDL window.
    SDL_Window *window = NULL;
    if (SUCCEEDED(result)) {
        window = SDL_CreateWindow(title, x == -1 ? (int)SDL_WINDOWPOS_CENTERED : x,
                                            y == -1 ? (int)SDL_WINDOWPOS_CENTERED : y,
                                            width * fscale, height * fscale, SDL_WINDOW_SHOWN);
        if (!window) result = kGraphicsApiError;
    }

    // Create SDL renderer with V-Sync enabled.
    SDL_Renderer *renderer = NULL;
    if (SUCCEEDED(result)) {
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
        if (!renderer) result = kGraphicsApiError;
    }

    // Create SDL streaming texture.
    SDL_Texture *texture = NULL;
    if (SUCCEEDED(result)) {
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                    SDL_TEXTUREACCESS_STREAMING, width, height);
        if (!texture) result = kGraphicsApiError;
    }

    if (SUCCEEDED(result)) {
        result = SUCCEEDED(SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE))
                ? kOk : kGraphicsApiError;
    }

    if (SUCCEEDED(result)) {
        result = SUCCEEDED(SDL_RenderClear(renderer)) ? kOk : kGraphicsApiError;
    }

    if (SUCCEEDED(result)) SDL_RenderPresent(renderer);

    if (SUCCEEDED(result)) {
        s->window = window;
        s->renderer = renderer;
        s->texture = texture;
        s->interrupt_addr = interrupt_addr;
    } else {
        SDL_DestroyTexture((SDL_Texture *) s->texture);
        s->texture = NULL;
        SDL_DestroyRenderer((SDL_Renderer *) s->renderer);
        s->renderer = NULL;
        SDL_DestroyWindow((SDL_Window *) s->window);
        s->window = NULL;
        free(s->pixels);
        s->pixels = NULL;
        free(s->background);
        s->background = NULL;
    }

    return result;
}

MmResult graphics_surface_destroy(MmSurface *surface) {
    assert(surface);
    if (surface->type != kGraphicsNone) {
        SDL_DestroyTexture((SDL_Texture *) surface->texture);
        SDL_DestroyRenderer((SDL_Renderer *) surface->renderer);
        SDL_DestroyWindow((SDL_Window *) surface->window);
        free(surface->pixels);
        free(surface->background);

        // TODO: ensure active sprite linked list is maintained.
//        MmSurface *next_active_sprite = surface->next_active_sprite;
        memset(surface, 0, sizeof(MmSurface));
//        surface->next_active_sprite = next_active_sprite;

        if (graphics_current == surface) graphics_current = NULL;
    }
    return kOk;
}

MmResult graphics_surface_destroy_all() {
    MmResult result = kOk;
    for (MmSurfaceId id = 0; SUCCEEDED(result) && id <= GRAPHICS_MAX_ID; ++id) {
        result = graphics_surface_destroy(&graphics_surfaces[id]);
    }
    return result;
}

MmResult graphics_surface_write(MmSurfaceId id) {
    if (id == GRAPHICS_NONE) {
        graphics_current = NULL;
    } else if (!graphics_surface_exists(id)) {
        return kGraphicsSurfaceNotFound;
    } else {
        graphics_current = &graphics_surfaces[id];
    }
    return kOk;
}

static inline void graphics_set_pixel(MmSurface *surface, int x, int y, MmGraphicsColour colour) {
    surface->pixels[y*surface->width + x] = colour;
}

static inline void graphics_set_pixel_safe(MmSurface *surface, int x, int y, MmGraphicsColour colour) {
    if (x >= 0 && y >= 0 && x < surface->width && y < surface->height) {
        surface->pixels[y*surface->width + x] = colour;
    }
}

MmResult graphics_draw_pixel(MmSurface *surface, int x, int y, MmGraphicsColour colour) {
    graphics_set_pixel_safe(surface, x, y, colour);
    surface->dirty = true;
    return kOk;
}

MmResult graphics_draw_aa_line(MmSurface *surface, MMFLOAT x0, MMFLOAT y0, MMFLOAT x1, MMFLOAT y1,
                               MmGraphicsColour colour, int w) {
    ERROR_UNIMPLEMENTED("graphics_draw_aa_line");
    return kUnimplemented;
}

MmResult graphics_draw_bitmap(MmSurface *surface, int x1, int y1, int width, int height, int scale,
                              MmGraphicsColour fcolour, MmGraphicsColour bcolour,
                              const unsigned char* bitmap) {
    const int hres = surface->width;
    const int vres = surface->height;

    if (x1 >= hres
            || y1 >= vres
            || x1 + (width * scale) < 0
            || y1 + (height * scale) < 0) return kOk;

    uint32_t *dst = surface->pixels;
    for (int i = 0; i < height; i++) {             // step thru the bitmap scan line by line
        for (int j = 0; j < scale; j++) {          // repeat lines to scale the bitmap
            const int y = y1 + i * scale + j;
            for (int k = 0; k < width; k++) {      // step through each bit in a scan line
                for (int m = 0; m < scale; m++) {  // repeat pixels to scale in the x axis
                    const int x = x1 + k * scale + m;
                    if (x >= 0 && x < hres && y >= 0 && y < vres) {  // if the coordinates are valid
                        if ((bitmap[((i * width) + k) / 8] >> (((height * width) - ((i * width) + k) - 1) % 8)) & 1) {
                            dst[y * hres + x] = fcolour;
                        } else if (bcolour != -1) {
                            dst[y * hres + x] = bcolour;
                        }
                    }
                }
            }
        }
    }

    surface->dirty = true;

    return kOk;
}

MmResult graphics_draw_box(MmSurface *surface, int x1, int y1, int x2, int y2, int w,
                           MmGraphicsColour colour, MmGraphicsColour fill) {
    // Make sure the coordinates are in the right sequence.
    if (x1 > x2) SWAP(int, x1, x2);
    if (y1 > y2) SWAP(int, x1, x2);

    w = min(min(w, x2 - x1), y2 - y1);
    if (w > 0) {
        w--;
        graphics_draw_rectangle(surface, x1, y1, x2, y1 + w, colour);  // Draw the top horiz line.
        graphics_draw_rectangle(surface, x1, y2 - w, x2, y2, colour);  // Draw the bottom horiz line.
        graphics_draw_rectangle(surface, x1, y1, x1 + w, y2, colour);  // Draw the left vert line.
        graphics_draw_rectangle(surface, x2 - w, y1, x2, y2, colour);  // Draw the right vert line.
        w++;
    }

    if (fill >= 0) graphics_draw_rectangle(surface, x1 + w, y1 + w, x2 - w, y2 - w, fill);

    return kOk;
}

MmResult graphics_draw_buffered(MmSurface *surface, int xti, int yti, MmGraphicsColour colour, int complete) {
    static unsigned char pos = 0;
    static unsigned char movex, movey, movec;
    static short xtilast[8];
    static short ytilast[8];
    static MmGraphicsColour clast[8];
    xtilast[pos] = xti;
    ytilast[pos] = yti;
    clast[pos] = colour;
    if (complete == 1) {
        if (pos == 1) {
            graphics_set_pixel_safe(surface, xtilast[0], ytilast[0], clast[0]);
        } else {
            graphics_draw_line(surface, xtilast[0], ytilast[0], xtilast[pos - 1], ytilast[pos - 1],
                               1, clast[0]);
        }
        pos = 0;
    } else {
        if (pos == 0) {
            movex = movey = movec = 1;
            pos += 1;
        } else {
            if (xti == xtilast[0] && abs(yti - ytilast[pos - 1]) == 1)
                movex = 0;
            else
                movex = 1;
            if (yti == ytilast[0] && abs(xti - xtilast[pos - 1]) == 1)
                movey = 0;
            else
                movey = 1;
            if (colour == clast[0])
                movec = 0;
            else
                movec = 1;
            if (movec == 0 && (movex == 0 || movey == 0) && pos < 6)
                pos += 1;
            else {
                if (pos == 1) {
                    graphics_set_pixel_safe(surface, xtilast[0], ytilast[0], clast[0]);
                } else {
                    graphics_draw_line(surface, xtilast[0], ytilast[0], xtilast[pos - 1],
                                       ytilast[pos - 1], 1, clast[0]);
                }
                movex = movey = movec = 1;
                xtilast[0] = xti;
                ytilast[0] = yti;
                clast[0] = colour;
                pos = 1;
            }
        }
    }

    return kOk;
}

static void graphics_hline(int x0, int x1, int y, int f, int ints_per_line,
                           uint32_t* br) {  // draw a horizontal line
    uint32_t w1, xx1, w0, xx0, x, xn, i;
    const uint32_t a[] = {
        0xFFFFFFFF, 0x7FFFFFFF, 0x3FFFFFFF, 0x1FFFFFFF, 0xFFFFFFF, 0x7FFFFFF, 0x3FFFFFF, 0x1FFFFFF,
        0xFFFFFF,   0x7FFFFF,   0x3FFFFF,   0x1FFFFF,   0xFFFFF,   0x7FFFF,   0x3FFFF,   0x1FFFF,
        0xFFFF,     0x7FFF,     0x3FFF,     0x1FFF,     0xFFF,     0x7FF,     0x3FF,     0x1FF,
        0xFF,       0x7F,       0x3F,       0x1F,       0x0F,      0x07,      0x03,      0x01};
    const uint32_t b[] = {0x80000000, 0xC0000000, 0xe0000000, 0xf0000000, 0xf8000000, 0xfc000000,
                          0xfe000000, 0xff000000, 0xff800000, 0xffC00000, 0xffe00000, 0xfff00000,
                          0xfff80000, 0xfffc0000, 0xfffe0000, 0xffff0000, 0xffff8000, 0xffffC000,
                          0xffffe000, 0xfffff000, 0xfffff800, 0xfffffc00, 0xfffffe00, 0xffffff00,
                          0xffffff80, 0xffffffC0, 0xffffffe0, 0xfffffff0, 0xfffffff8, 0xfffffffc,
                          0xfffffffe, 0xffffffff};
    w0 = y * (ints_per_line);
    xx0 = 0;
    w1 = y * (ints_per_line) + x1 / 32;
    xx1 = (x1 & 0x1F);
    w0 = y * (ints_per_line) + x0 / 32;
    xx0 = (x0 & 0x1F);
    w1 = y * (ints_per_line) + x1 / 32;
    xx1 = (x1 & 0x1F);
    if (w1 == w0) {  // special case both inside same word
        x = (a[xx0] & b[xx1]);
        xn = ~x;
        if (f)
            br[w0] |= x;
        else
            br[w0] &= xn;  // turn on the pixel
    } else {
        if (w1 - w0 > 1) {  // first deal with full words
            for (i = w0 + 1; i < w1; i++) {
                // draw the pixel
                br[i] = 0;
                if (f) br[i] = 0xFFFFFFFF;  // turn on the pixels
            }
        }
        x = ~a[xx0];
        br[w0] &= x;
        x = ~x;
        if (f) br[w0] |= x;  // turn on the pixel
        x = ~b[xx1];
        br[w1] &= x;
        x = ~x;
        if (f) br[w1] |= x;  // turn on the pixel
    }
}

MmResult graphics_draw_filled_circle(int x, int y, int radius, int r,
                                     MmGraphicsColour fill, int ints_per_line, uint32_t* br,
                                     MMFLOAT aspect, MMFLOAT aspect2) {
    int a, b, P;
    int A, B, asp;
    x = (int)((MMFLOAT)r * aspect) + radius;
    y = r + radius;
    a = 0;
    b = radius;
    P = 1 - radius;
    asp = aspect2 * (MMFLOAT)(1 << 10);
    do {
        A = (a * asp) >> 10;
        B = (b * asp) >> 10;
        graphics_hline(x - A - radius, x + A - radius, y + b - radius, fill, ints_per_line, br);
        graphics_hline(x - A - radius, x + A - radius, y - b - radius, fill, ints_per_line, br);
        graphics_hline(x - B - radius, x + B - radius, y + a - radius, fill, ints_per_line, br);
        graphics_hline(x - B - radius, x + B - radius, y - a - radius, fill, ints_per_line, br);
        if (P < 0)
            P += 3 + 2 * a++;
        else
            P += 5 + 2 * (a++ - b--);

    } while (a <= b);

    return kOk;
}

#define RoundUptoInt(a) (((a) + (32 - 1)) & (~(32 - 1)))  // round up to the nearest whole integer

MmResult graphics_draw_circle(MmSurface *surface, int x, int y, int radius, int w,
                              MmGraphicsColour colour, MmGraphicsColour fill, MMFLOAT aspect) {
    if (!surface || surface->type == kGraphicsNone) return kGraphicsInvalidWriteSurface;
    int a, b, P;
    int A, B;
    int asp;
    MMFLOAT aspect2;
    if (w > 1) {
        if (fill >= 0) {  // thick border with filled centre
            graphics_draw_circle(surface, x, y, radius, 0, colour, colour, aspect);
            aspect2 = ((aspect * (MMFLOAT)radius) - (MMFLOAT)w) / ((MMFLOAT)(radius - w));
            graphics_draw_circle(surface, x, y, radius - w, 0, fill, fill, aspect2);
        } else {  // thick border with empty centre
            int r1 = radius - w, r2 = radius, xs = -1, xi = 0, i, j, k, m, ll = radius;
            if (aspect > 1.0) ll = (int)((MMFLOAT)radius * aspect);
            int ints_per_line = RoundUptoInt((ll * 2) + 1) / 32;
            uint32_t* br = (uint32_t*)GetTempMemory(((ints_per_line + 1) * ((r2 * 2) + 1)) * 4);
            graphics_draw_filled_circle(x, y, r2, r2, 1, ints_per_line, br, aspect, aspect);
            aspect2 = ((aspect * (MMFLOAT)r2) - (MMFLOAT)w) / ((MMFLOAT)r1);
            graphics_draw_filled_circle(x, y, r1, r2, 0, ints_per_line, br, aspect, aspect2);
            x = (int)((MMFLOAT)x + (MMFLOAT)r2 * (1.0 - aspect));
            for (j = 0; j < r2 * 2 + 1; j++) {
                for (i = 0; i < ints_per_line; i++) {
                    k = br[i + j * ints_per_line];
                    for (m = 0; m < 32; m++) {
                        if (xs == -1 && (k & 0x80000000)) {
                            xs = m;
                            xi = i;
                        }
                        if (xs != -1 && !(k & 0x80000000)) {
                            graphics_draw_rectangle(surface, x - r2 + xs + xi * 32, y - r2 + j,
                                                    x - r2 + m + i * 32, y - r2 + j, colour);
                            xs = -1;
                        }
                        k <<= 1;
                    }
                }
                if (xs != -1) {
                    graphics_draw_rectangle(surface, x - r2 + xs + xi * 32, y - r2 + j, x - r2 + m + i * 32,
                                            y - r2 + j, colour);
                    xs = -1;
                }
            }
        }

    } else {  // single thickness outline
        int w1 = w, r1 = radius;
        if (fill >= 0) {
            while (radius > 0) {
                a = 0;
                b = radius;
                P = 1 - radius;
                asp = aspect * (MMFLOAT)(1 << 10);

                do {
                    A = (a * asp) >> 10;
                    B = (b * asp) >> 10;
                    if (fill >= 0) {
                        graphics_draw_rectangle(surface, x - A, y + b, x + A, y + b, fill);
                        graphics_draw_rectangle(surface, x - A, y - b, x + A, y - b, fill);
                        graphics_draw_rectangle(surface, x - B, y + a, x + B, y + a, fill);
                        graphics_draw_rectangle(surface, x - B, y - a, x + B, y - a, fill);
                    }
                    if (P < 0)
                        P += 3 + 2 * a++;
                    else
                        P += 5 + 2 * (a++ - b--);

                } while (a <= b);
                if (w == 0) break;
                w--;
                radius--;
            }
        }
        if (colour != fill) {
            w = w1;
            radius = r1;
            while (radius > 0) {
                a = 0;
                b = radius;
                P = 1 - radius;
                asp = aspect * (MMFLOAT)(1 << 10);
                do {
                    A = (a * asp) >> 10;
                    B = (b * asp) >> 10;
                    if (w) {
                        graphics_set_pixel_safe(surface, A + x, b + y, colour);
                        graphics_set_pixel_safe(surface, B + x, a + y, colour);
                        graphics_set_pixel_safe(surface, x - A, b + y, colour);
                        graphics_set_pixel_safe(surface, x - B, a + y, colour);
                        graphics_set_pixel_safe(surface, B + x, y - a, colour);
                        graphics_set_pixel_safe(surface, A + x, y - b, colour);
                        graphics_set_pixel_safe(surface, x - A, y - b, colour);
                        graphics_set_pixel_safe(surface, x - B, y - a, colour);
                    }
                    if (P < 0)
                        P += 3 + 2 * a++;
                    else
                        P += 5 + 2 * (a++ - b--);

                } while (a <= b);
                if (w == 0) break;
                w--;
                radius--;
            }
        }
    }

    surface->dirty = true;
    return kOk;
}

MmResult graphics_draw_line(MmSurface *surface, int x1, int y1, int x2, int y2, int w,
                            MmGraphicsColour colour) {
    if (y1 == y2) {
        return graphics_draw_rectangle(surface, x1, y1, x2, y2 + w - 1, colour);  // horiz line
    }
    if (x1 == x2) {
        return graphics_draw_rectangle(surface, x1, y1, x2 + w - 1, y2, colour);  // vert line
    }
    int dx, dy, sx, sy, err, e2;
    dx = abs(x2 - x1);
    sx = x1 < x2 ? 1 : -1;
    dy = -abs(y2 - y1);
    sy = y1 < y2 ? 1 : -1;
    err = dx + dy;
    while (1) {
        graphics_draw_buffered(surface, x1, y1, colour, 0);
        e2 = 2 * err;
        if (e2 >= dy) {
            if (x1 == x2) break;
            err += dy;
            x1 += sx;
        }
        if (e2 <= dx) {
            if (y1 == y2) break;
            err += dx;
            y1 += sy;
        }
    }
    graphics_draw_buffered(surface, 0, 0, 0, 1);
    // if (Option.Refresh)Display_Refresh();
    surface->dirty = true;

    return kOk;
}

MmResult graphics_draw_polygon(MmSurface *surface, unsigned char *p, int close) {
    ERROR_UNIMPLEMENTED("graphics_draw_polygon");
    return kOk;
}

MmResult graphics_draw_rbox(MmSurface *surface, int x1, int y1, int x2, int y2, int radius,
                            MmGraphicsColour colour, MmGraphicsColour fill) {
    // Make sure the coordinates are in the right sequence.
    if (x1 > x2) SWAP(int, x1, x2);
    if (y1 > y2) SWAP(int, y1, y2);

    int f = 1 - radius;
    int ddF_x = 1;
    int ddF_y = -2 * radius;
    int xx = 0;
    int yy = radius;

    while (xx < yy) {
        if (f >= 0) {
            yy -= 1;
            ddF_y += 2;
            f += ddF_y;
        }
        xx += 1;
        ddF_x += 2;
        f += ddF_x;
        graphics_set_pixel_safe(surface, x2 + xx - radius, y2 + yy - radius, colour);  // Bottom Right Corner
        graphics_set_pixel_safe(surface, x2 + yy - radius, y2 + xx - radius, colour);  // ^^^
        graphics_set_pixel_safe(surface, x1 - xx + radius, y2 + yy - radius, colour);  // Bottom Left Corner
        graphics_set_pixel_safe(surface, x1 - yy + radius, y2 + xx - radius, colour);  // ^^^

        graphics_set_pixel_safe(surface, x2 + xx - radius, y1 - yy + radius, colour);  // Top Right Corner
        graphics_set_pixel_safe(surface, x2 + yy - radius, y1 - xx + radius, colour);  // ^^^
        graphics_set_pixel_safe(surface, x1 - xx + radius, y1 - yy + radius, colour);  // Top Left Corner
        graphics_set_pixel_safe(surface, x1 - yy + radius, y1 - xx + radius, colour);  // ^^^
        if (fill >= 0) {
            graphics_draw_line(surface, x2 + xx - radius - 1, y2 + yy - radius, x1 - xx + radius + 1,
                               y2 + yy - radius, 1, fill);
            graphics_draw_line(surface, x2 + yy - radius - 1, y2 + xx - radius, x1 - yy + radius + 1,
                               y2 + xx - radius, 1, fill);
            graphics_draw_line(surface, x2 + xx - radius - 1, y1 - yy + radius, x1 - xx + radius + 1,
                               y1 - yy + radius, 1, fill);
            graphics_draw_line(surface, x2 + yy - radius - 1, y1 - xx + radius, x1 - yy + radius + 1,
                               y1 - xx + radius, 1, fill);
        }
    }
    if (fill >= 0) graphics_draw_rectangle(surface, x1 + 1, y1 + radius, x2 - 1, y2 - radius, fill);
    graphics_draw_rectangle(surface, x1 + radius - 1, y1, x2 - radius + 1, y1, colour);  // top side
    graphics_draw_rectangle(surface, x1 + radius - 1, y2, x2 - radius + 1, y2, colour);  // bottom side
    graphics_draw_rectangle(surface, x1, y1 + radius, x1, y2 - radius, colour);          // left side
    graphics_draw_rectangle(surface, x2, y1 + radius, x2, y2 - radius, colour);          // right side
    // if (Option.Refresh) Display_Refresh();

    return kOk;
}

MmResult graphics_draw_rectangle(MmSurface *surface, int x1, int y1, int x2, int y2,
                                 MmGraphicsColour colour) {
    // Do not draw anything if entire rectangle is off the screen.
    if ((x1 < 0 && x2 < 0) || (y1 < 0 && y2 < 0) ||
        (x1 >= surface->width && x2 >= surface->width) ||
        (y1 >= surface->height && y2 >= surface->height)) {
        return kOk;
    }

    x1 = min(max(0, x1), HRes - 1);
    x2 = min(max(0, x2), HRes - 1);
    y1 = min(max(0, y1), VRes - 1);
    y2 = min(max(0, y2), VRes - 1);
    if (x1 > x2) SWAP(int, x1, x2);
    if (y1 > y2) SWAP(int, y1, y2);
    for (int y = y1; y <= y2; y++) {
        for (int x = x1; x <= x2; x++) graphics_set_pixel(surface, x, y, colour);
    }
    surface->dirty = true;
    return kOk;
}

MmResult graphics_draw_triangle(MmSurface *surface, int x0, int y0, int x1, int y1, int x2, int y2,
                                MmGraphicsColour colour, MmGraphicsColour fill) {
    if (x0 * (y1 - y2) + x1 * (y2 - y0) + x2 * (y0 - y1) == 0) {
        // points are co-linear i.e zero area
        if (y0 > y1) {
            SWAP(int, y0, y1);
            SWAP(int, x0, x1);
        }
        if (y1 > y2) {
            SWAP(int, y2, y1);
            SWAP(int, x2, x1);
        }
        if (y0 > y1) {
            SWAP(int, y0, y1);
            SWAP(int, x0, x1);
        }
        graphics_draw_line(surface, x0, y0, x2, y2, 1, colour);
    }
    else {
        if (fill == -1) {
            // draw only the outline
            graphics_draw_line(surface, x0, y0, x1, y1, 1, colour);
            graphics_draw_line(surface, x1, y1, x2, y2, 1, colour);
            graphics_draw_line(surface, x2, y2, x0, y0, 1, colour);
        }
        else {
            //we are drawing a filled triangle which may also have an outline
            int a, b, y, last;

            if (y0 > y1) {
                SWAP(int, y0, y1);
                SWAP(int, x0, x1);
            }
            if (y1 > y2) {
                SWAP(int, y2, y1);
                SWAP(int, x2, x1);
            }
            if (y0 > y1) {
                SWAP(int, y0, y1);
                SWAP(int, x0, x1);
            }

            // We only care about what is visible.
            y0 = min(max(0, y0), surface->height - 1);
            y1 = min(max(0, y1), surface->height - 1);
            y2 = min(max(0, y2), surface->height - 1);

            if (y1 == y2) {
                last = y1;                                          //Include y1 scanline
            }
            else {
                last = y1 - 1;                                      // Skip it
            }
            for (y = y0; y <= last; y++) {
                if (y1 == y0 || y2 == y0) continue; // Impose sanity.
                a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
                b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
                if (a > b)SWAP(int, a, b);
                graphics_draw_rectangle(surface, a, y, b, y, fill);
            }
            while (y <= y2) {
                if (y2 == y1 || y2 == y0) continue; // Impose sanity.
                a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
                b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
                if (a > b) SWAP(int, a, b);
                graphics_draw_rectangle(surface, a, y, b, y, fill);
                y = y + 1;
            }
            // we also need an outline but we do this last to overwrite the edge of the fill area
            if (colour != fill) {
                graphics_draw_line(surface, x0, y0, x1, y1, 1, colour);
                graphics_draw_line(surface, x1, y1, x2, y2, 1, colour);
                graphics_draw_line(surface, x2, y2, x0, y0, 1, colour);
            }
        }
    }

    return kOk;
}

void graphics_draw_buffer(MmSurface *surface, int x1, int y1, int x2, int y2,
                          const unsigned char* buffer, int skip) {
    const unsigned char *psrc = buffer;
    union colourmap
    {
        char rgbbytes[4];
        uint32_t rgb;
    } c;
    int scale = 1; // (PageTable[WritePage].expand ? 2 : 1);
    //if (optiony)y1=maxH-1-y1;
    //if (optiony)y2=maxH-1-y2;
    // make sure the coordinates are kept within the display area
    if (x2 <= x1) SWAP(int, x1, x2);
    if (y2 <= y1) SWAP(int, y1, y2);
    // int cursorhidden=0;
    // if (cursoron)
    //     if ( !(xcursor + wcursor < x1 ||
    //         xcursor > x2 ||
    //         ycursor + hcursor < y1 ||
    //         ycursor > y2)){
    //     hidecursor(0);
    //     cursorhidden=1;
    //     }
    if (scale==1){
        for (int y = y1; y <= y2; y++){
            // routinechecks(1);
            uint32_t *pdst = surface->pixels + (y * surface->width + x1);
            for (int x = x1; x <= x2; x++){
                if (x >= 0 && x < surface->width && y >= 0 && y < surface->height) {
                    if (skip & 2) {
                        c.rgbbytes[3] = 0xFF; //assume solid colour
                        c.rgbbytes[2] = *psrc++; //this order swaps the bytes to match the .BMP file
                        c.rgbbytes[1] = *psrc++;
                        c.rgbbytes[0] = *psrc++;
                        if (skip & 1) c.rgbbytes[3] = *psrc++; //ARGB8888 so set transparency
                    } else {
                        c.rgbbytes[3] = 0;
                        c.rgbbytes[0] = *psrc++; //this order swaps the bytes to match the .BMP file
                        c.rgbbytes[1] = *psrc++;
                        c.rgbbytes[2] = *psrc++;
                        if (skip & 1) psrc++;
                    }
                    *pdst = c.rgb;
                } else {
                    psrc += (skip & 1) ? 4 : 3;
                }
                pdst++;
            }
        }
    }
    // } else {
    //     uint32_t *s1;
    //     for(y=y1*2;y<=y2*2;y+=2){
    //         routinechecks(1);
    //         sc=(uint32_t *)((y * maxW + x1) * 4 + wpa);
    //         s1=(uint32_t *)(((y+1) * maxW + x1) * 4 + wpa);
    //         for(x=x1;x<=x2;x++){
    //             if (x>=0 && x<maxW && y>=0 && y<maxH*2){
    //                 if (skip & 2){
    //                     c.rgbbytes[3]=0xFF;
    //                     c.rgbbytes[2]=*p++; //this order swaps the bytes to match the .BMP file
    //                     c.rgbbytes[1]=*p++;
    //                     c.rgbbytes[0]=*p++;
    //                     if (skip & 1)c.rgbbytes[3]=*p++; //ARGB8888 so set transparency
    //                 } else {
    //                     c.rgbbytes[3]=0;
    //                     c.rgbbytes[0]=*p++; //this order swaps the bytes to match the .BMP file
    //                     c.rgbbytes[1]=*p++;
    //                     c.rgbbytes[2]=*p++;
    //                     if (skip & 1)p++;
    //                 }
    //                 *sc=c.rgb;
    //                 *s1=*sc;
    //             } else {
    //                 p+=(skip & 1) ? 4 : 3;
    //             }
    //             sc++;
    //             s1++;
    //         }
    //     }
    // }
    // if (cursorhidden)showcursor(0, xcursor,ycursor);
}

static size_t spbmp_file_read_cb(void *file, void *buffer, size_t size, size_t count,
                                 void *userdata) {
    return fread(buffer, size, count, (FILE *) file);
}

static void spbmp_set_pixel_cb(int x, int y, SpColourRgba colour, void *userdata) {
    graphics_set_pixel_safe((MmSurface *) userdata, x, y, (MmGraphicsColour) colour);
}

static int spbmp_abort_check_cb(void *userdata) {
    CheckAbort();
    return 0;
}

MmResult graphics_load_bmp(MmSurface *surface, char *filename, int x, int y) {
    MmResult result = kOk;

    if (!path_has_suffix(filename, ".BMP", true)) {
        // TODO: What if the file-extension is ".bmp" ?
        result = cstring_cat(filename, ".BMP", STRINGSIZE);
        if (FAILED(result)) return result;
    }

    int fnbr = file_find_free();
    result = file_open(filename, "rb", fnbr);
    if (FAILED(result)) return result;
    spbmp_init(spbmp_file_read_cb, spbmp_set_pixel_cb, spbmp_abort_check_cb);
    SpBmpResult bmp_result = spbmp_load(file_table[fnbr].file_ptr, x, y, surface);
    surface->dirty = true;
    if (FAILED(bmp_result)) {
        (void) file_close(fnbr);
        result = kGraphicsLoadBitmapFailed;
    } else {
        result = file_close(fnbr);
    }

    return result;
}

MmResult graphics_load_png(MmSurface *surface, char *filename, int x, int y, int transparent,
                           int force) {
    if (strchr(filename, '.') == NULL) cstring_cat(filename, ".PNG", STRINGSIZE);
    upng_t *upng = upng_new_from_file(filename);
    // routinechecks(1);
    upng_header(upng);
    const int w = upng_get_width(upng);
    const int h = upng_get_height(upng);
    if (x + w > graphics_current->width || y + h > graphics_current->height) {
        upng_free(upng);
        return kImageTooLarge;
    }
    if (!(upng_get_format(upng)==1 || upng_get_format(upng)==3)){
        upng_free(upng);
        return kImageInvalidFormat;
    }
    // routinechecks(1);
    upng_decode(upng);
    // routinechecks(1);
    const unsigned char *buffer = upng_get_buffer(upng);
    // int savey = optiony;
    // optiony = 0;
    if (upng_get_format(upng) ==3) {
        graphics_draw_buffer(surface, x, y, x + w - 1, y + h - 1, buffer, 3 | transparent | force);
    } else {
        graphics_draw_buffer(surface, x, y, x + w - 1, y + h - 1, buffer, 2 | transparent | force);
    }
    // optiony = savey;
    upng_free(upng);
    // clearrepeat();
    surface->dirty = true;
    return kOk;
}

MmResult graphics_load_sprite(const char *filename_in, uint8_t start_sprite_id, uint8_t colour_mode) {
    char filename[STRINGSIZE];
    cstring_cpy(filename, filename_in, STRINGSIZE);
    if (!path_has_suffix(filename, ".spr", true)) {
        // TODO: What if the file-extension is ".SPR" ?
        MmResult result = cstring_cat(filename, ".spr", STRINGSIZE);
        if (FAILED(result)) return result;
    }

    int fnbr = file_find_free();
    MmResult result = file_open(filename, "r", fnbr);
    if (FAILED(result)) return result;

    const bool is_picomite = (mmb_options.simulate == kSimulateGameMite)
            || (mmb_options.simulate == kSimulatePicoMiteVga);
    const MmGraphicsColour *sprite_colours = (colour_mode == 0)
            ? (is_picomite) ? GRAPHICS_CMM2_SPRITE_COLOURS_RGB121 : GRAPHICS_CMM2_SPRITE_COLOURS
            : GRAPHICS_RGB121_COLOURS;

    char buf[256] = { 0 };
    MMgetline(fnbr, buf);
    while (buf[0] == 39) MMgetline(fnbr, buf);  // Skip lines beginning with single quote.
    const char *z = buf;

    getargs(&z, 5, ", ");
    unsigned width = getinteger(argv[0]);
    MmSurfaceId number = getinteger(argv[2]);
    unsigned height = (argc == 5) ? getinteger(argv[4]) : width;

    if (mmb_options.simulate != kSimulateMmb4l) {
        if (start_sprite_id + number > CMM2_SPRITE_BASE + CMM2_SPRITE_COUNT) {
            (void) file_close(fnbr);
            return kGraphicsTooManySprites;
            // error((char *)"Maximum of % sprites",MAXBLITBUF);
        }
    }

    bool new_sprite = true;
    uint8_t lc = 0;
    uint32_t *p = NULL;
    MmSurfaceId surface_id = start_sprite_id;
    while (!file_eof(fnbr) && surface_id <= number + start_sprite_id) {
        if (new_sprite) {
            new_sprite = false;
            result = graphics_sprite_create(surface_id, width, height);
            if (FAILED(result)) {
                (void) file_close(fnbr);
                return result;
            }
            lc = height;
            p = graphics_surfaces[surface_id].pixels;
        }

        while (lc--) {
            MMgetline(fnbr, buf);
            while (buf[0] == 39) MMgetline(fnbr, buf);
            if (strlen(buf) < width) memset(&buf[strlen(buf)], 32, width - strlen(buf));
            for (size_t i = 0; i < width; i++) {
                uint8_t idx = 0;
                if (buf[i] >= '0' && buf[i] <= '9') {
                    idx = buf[i] - '0';
                } else if (buf[i] >= 'A' && buf[i] <= 'F') {
                    idx = buf[i] - 'A';
                } else if (buf[i] >= 'a' && buf[i] <= 'f') {
                    idx = buf[i] - 'a';
                }
                *p++ = sprite_colours[idx];
            }
        }

        surface_id++;
        new_sprite = true;
    }

    return file_close(fnbr);
}

static void *pixelcpy_transparent(void* dst, const void* src, size_t count, uint32_t transparent) {
    assert(count % 4 == 0);
    uint32_t *dst_word = (uint32_t *)dst;
    const uint32_t *src_word = (const uint32_t *)src;
    const size_t word_count = count / 4;
    for (size_t i = 0; i < word_count; ++i) {
        if (*src_word != transparent) {
            *dst_word++ = *src_word++;
        } else {
            dst_word++;
            src_word++;
        }
    }
    return dst;
}

MmResult graphics_blit(int src_x, int src_y, int dst_x, int dst_y, int w, int h,
                       MmSurface *src_surface, MmSurface *dst_surface, unsigned flags,
                       MmGraphicsColour transparent) {
    // printf("graphics_blit: src_x = %d, src_y = %d, dst_x = %d, dst_y = %d, w = %d, h = %d, src_id = %d, dst_id = %d\n",
    //       src_x, src_y, dst_x, dst_y, w, h, src_surface->id, dst_surface->id);

    if (src_x < 0) {
        w = max(0, w + src_x);
        dst_x -= src_x;
        src_x = 0;
    }
    if (src_x + w >= src_surface->width) w = max(0, src_surface->width - src_x);

    if (src_y < 0) {
        h = max(0, h + src_y);
        dst_y -= src_y;
        src_y = 0;
    }
    if (src_y + h >= src_surface->height) h = max(0, src_surface->height - src_y);

    if (dst_x < 0) {
        w = max(0, w + dst_x);
        src_x -= dst_x;
        dst_x = 0;
    }
    if (dst_x + w >= dst_surface->width) w = max(0, dst_surface->width - dst_x);

    if (dst_y < 0) {
        h = max(0, h + dst_y);
        src_y -= dst_y;
        dst_y = 0;
    }
    if (dst_y + h >= dst_surface->height) h = max(0, dst_surface->height - dst_y);

    // printf("graphics_blit: src_x = %d, src_y = %d, dst_x = %d, dst_y = %d, w = %d, h = %d, src_id = %d, dst_id = %d\n",
    //       src_x, src_y, dst_x, dst_y, w, h, src_surface->id, dst_surface->id);

    if (w == 0 || h == 0) return kOk;

    uint32_t *src = src_surface->pixels + (src_y * src_surface->width) + src_x;
    uint32_t *dst = dst_surface->pixels;
    int pdelta = 0; // Added to dst after writing each pixel.
    int ldelta = 0; // Added to dst after writing each line.

    dst_surface->dirty = true;

    // TODO: If read and write surfaces overlap then copy read surface to temporary buffer.

    switch (flags & 0x3) {
        case kBlitNormal: {
            dst += (dst_y * dst_surface->width) + dst_x;
            if (!(flags & kBlitWithTransparency)) {
                // Optimised using memcpy.
                for (int i = 0; i < h; i++) {
                    memcpy(dst, src, w << 2);
                    src += src_surface->width;
                    dst += dst_surface->width;
                }
                return kOk;
            }
            pdelta = 1;
            ldelta = dst_surface->width - w;
            break;
        }

        case kBlitHorizontalFlip:
        {
            dst += (dst_y * dst_surface->width) + dst_x + w - 1;
            pdelta = -1;
            ldelta = dst_surface->width + w;
            break;
        }

        case kBlitVerticalFlip:
        {
            // TODO: could be optimised using memcpy.
            dst += ((dst_y + h - 1) * dst_surface->width) + dst_x;
            pdelta = 1;
            ldelta = -(dst_surface->width + w);
            break;
        }

        case kBlitHorizontalFlip | kBlitVerticalFlip:
        {
            dst += ((dst_y + h - 1) * dst_surface->width) + dst_x + w - 1;
            pdelta = -1;
            ldelta = 0;
            break;
        }

        default:
            return kInternalFault;
    }

    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            if ((flags & kBlitWithTransparency) && *src == transparent) {
                src++;
            } else {
                *dst = *src++;
            }
            dst += pdelta;
        }
        src += src_surface->width - w;
        dst += ldelta;
    }

    return kOk;
}

MmResult graphics_blit_memory_compressed(MmSurface *surface, char *data, int x, int y, int w,
                                         int h, unsigned transparent) {
    unsigned count = 0;
    unsigned colour;
    for (int yy = y; yy < y + h; ++yy) {
        for (int xx = x; xx < x + w; ++xx) {
            if (count == 0) {
                count = *data & 0x0F;
                colour = *data >> 4;
                data++;
            }
            if (colour != transparent) {
                graphics_set_pixel_safe(surface, xx, yy, GRAPHICS_RGB121_COLOURS[colour]);
            }
            count--;
        }
    }

    surface->dirty = true;

    return kOk;
}

MmResult graphics_blit_memory_uncompressed(MmSurface *surface, char *data, int x, int y, int w,
                                           int h, unsigned transparent) {
    unsigned count = 0;
    unsigned colour;
    for (int yy = y; yy < y + h; ++yy) {
        for (int xx = x; xx < x + w; ++xx) {
            switch (count) {
                case 0:
                    count = 2;
                    colour = *data & 0xF;
                    break;
                case 1:
                    colour = *data >> 4;
                    data++;
                    break;
                default:
                    return kInternalFault;
            }
            if (colour != transparent) {
                graphics_set_pixel_safe(surface, xx, yy, GRAPHICS_RGB121_COLOURS[colour]);
            }
            count--;
        }
    }

    surface->dirty = true;

    return kOk;
}

MmResult graphics_draw_char(MmSurface *surface,  int *x, int *y, uint32_t font,
                            MmGraphicsColour fcolour, MmGraphicsColour bcolour, char c,
                            TextOrientation orientation) {
    unsigned char *p, *fp, *np = NULL;
    int BitNumber, BitPos, newx, newy, modx, mody, scale = font & 0b1111;
    MmResult result = kOk;

    uint32_t PrintPixelMode = 0; // Currently always 0.
    switch (PrintPixelMode) {
        case 0:
            break;
        case 1:
            bcolour = -1;
            break;
        case 2:
            SWAP(int64_t, fcolour, bcolour);
            break;
        case 5:
            fcolour = bcolour;
            bcolour = -1;
            break;
        default:
            return kInternalFault;
    }

    // To get the +, - and = chars for font 6 we fudge them by scaling up font 1.
    if ((font & 0xf0) == 0x60 && (c == '-' || c == '+' || c == '=')) {
        fp = (unsigned char *) FontTable[1];
        scale = scale * 4;
    }
    else
        fp = (unsigned char *) FontTable[font >> 4];

    int height = fp[1];
    int width = fp[0];
    modx = mody = 0;
    if (orientation > kOrientVert) {
        np = (unsigned char *)GetTempMemory(width * height);
        if (orientation == kOrientInverted) {
            modx -= width * scale - 1;
            mody -= height * scale - 1;
        } else if (orientation == kOrientCounterClock) {
            mody -= width * scale;
        } else if (orientation == kOrientClockwise) {
            modx -= height * scale - 1;
        }
    }

    if (c >= fp[2] && c < fp[2] + fp[3]) {
        p = fp + 4 + (int)(((c - fp[2]) * height * width) / 8);

        if (orientation > kOrientVert) {                             // non-standard orientation
            if (orientation == kOrientInverted) {
                for (int y = 0; y < height; y++) {
                    newy = height - y - 1;
                    for (int x = 0; x < width; x++) {
                        newx = width - x - 1;
                        if ((p[((y * width) + x) / 8] >> (((height * width) - ((y * width) + x) - 1) % 8)) & 1) {
                            BitNumber = ((newy * width) + newx);
                            BitPos = 128 >> (BitNumber % 8);
                            np[BitNumber / 8] |= BitPos;
                        }
                    }
                }
            }
            else if (orientation == kOrientCounterClock) {
                for (int y = 0; y < height; y++) {
                    newx = y;
                    for (int x = 0; x < width; x++) {
                        newy = width - x - 1;
                        if ((p[((y * width) + x) / 8] >> (((height * width) - ((y * width) + x) - 1) % 8)) & 1) {
                            BitNumber = ((newy * height) + newx);
                            BitPos = 128 >> (BitNumber % 8);
                            np[BitNumber / 8] |= BitPos;
                        }
                    }
                }
            }
            else if (orientation == kOrientClockwise) {
                for (int y = 0; y < height; y++) {
                    newx = height - y - 1;
                    for (int x = 0; x < width; x++) {
                        newy = x;
                        if ((p[((y * width) + x) / 8] >> (((height * width) - ((y * width) + x) - 1) % 8)) & 1) {
                            BitNumber = ((newy * height) + newx);
                            BitPos = 128 >> (BitNumber % 8);
                            np[BitNumber / 8] |= BitPos;
                        }
                    }
                }
            }
        }
        else np = p;

        if (orientation < kOrientCounterClock) {
            result = graphics_draw_bitmap(surface, *x + modx, *y + mody, width, height, scale,
                                          fcolour, bcolour, np);
        } else {
            result = graphics_draw_bitmap(surface, *x + modx, *y + mody, height, width, scale,
                                          fcolour, bcolour, np);
        }
    }
    else {
        if (orientation < kOrientCounterClock) {
            result = graphics_draw_rectangle(surface, *x + modx, *y + mody,
                                             *x + modx + (width * scale),
                                             *y + mody + (height * scale), bcolour);
        } else {
            result = graphics_draw_rectangle(surface, *x + modx, *y + mody,
                                             *x + modx + (height * scale),
                                             *y + mody + (width * scale), bcolour);
        }
    }

    if (FAILED(result)) return result;

    // To get the . and degree symbols for font 6 we draw a small circle.
    if ((font & 0xf0) == 0x60) {
        if (orientation > kOrientVert) {
            if (orientation == kOrientInverted) {
                if (c == '.') {
                    result = graphics_draw_circle(surface, *x + modx + (width * scale) / 2,
                                                 *y + mody + 7 * scale, 4 * scale, 0, fcolour,
                                                 fcolour, 1.0);
                } else if (c == 0x60) {
                    result = graphics_draw_circle(surface, *x + modx + (width * scale) / 2,
                                                  *y + mody + (height * scale) - 9 * scale,
                                                  6 * scale, 2 * scale, fcolour, -1, 1.0);
                }
            } else if (orientation == kOrientCounterClock) {
                if (c == '.') {
                    result = graphics_draw_circle(surface, *x + modx + (height * scale) - 7 * scale,
                                                  *y + mody + (width * scale) / 2, 4 * scale, 0,
                                                  fcolour, fcolour, 1.0);
                } else if (c == 0x60) {
                    result = graphics_draw_circle(surface, *x + modx + 9 * scale,
                                                  *y + mody + (width * scale) / 2, 6 * scale,
                                                  2 * scale, fcolour, -1, 1.0);
                }
            } else if (orientation == kOrientClockwise) {
                if (c == '.') {
                    result = graphics_draw_circle(surface, *x + modx + 7 * scale,
                                                  *y + mody + (width * scale) / 2, 4 * scale, 0,
                                                  fcolour, fcolour, 1.0);
                } else if (c == 0x60) {
                    result = graphics_draw_circle(surface, *x + modx + (height * scale) - 9 * scale,
                                                  *y + mody + (width * scale) / 2, 6 * scale,
                                                  2 * scale, fcolour, -1, 1.0);
                }
            }
        }
        else {
            if (c == '.') {
                result = graphics_draw_circle(surface, *x + modx + (width * scale) / 2,
                                              *y + mody + (height * scale) - 7 * scale, 4 * scale,
                                              0, fcolour, fcolour, 1.0);
            } else if (c == 0x60) {
                result = graphics_draw_circle(surface, *x + modx + (width * scale) / 2,
                                              *y + mody + 9 * scale, 6 * scale, 2 * scale, fcolour,
                                              -1, 1.0);
            }
        }
    }

    switch (orientation) {
        case kOrientNormal:
            *x += width * scale;
            break;
        case kOrientVert:
            *y += height * scale;
            break;
        case kOrientInverted:
            *x -= width * scale;
            break;
        case kOrientCounterClock:
            *y -= width * scale;
            break;
        case kOrientClockwise:
            *y += width * scale;
            break;
        default:
            return kInternalFault;
    }

    return result;
}

MmResult graphics_draw_string(MmSurface *surface, int x, int y, uint32_t font, TextHAlign jh,
                              TextVAlign jv, TextOrientation jo, MmGraphicsColour fcolour,
                              MmGraphicsColour bcolour, const char *s) {
    switch (jo) {
        case kOrientNormal:
            if (jh == kAlignCenter) x -= (strlen(s) * font_width(font)) / 2;
            if (jh == kAlignRight)  x -= (strlen(s) * font_width(font));
            if (jv == kAlignMiddle) y -= font_height(font) / 2;
            if (jv == kAlignBottom) y -= font_height(font);
            break;
        case kOrientVert:
            if (jh == kAlignCenter) x -= font_width(font) / 2;
            if (jh == kAlignRight)  x -= font_width(font);
            if (jv == kAlignMiddle) y -= (strlen(s) * font_height(font)) / 2;
            if (jv == kAlignBottom) y -= (strlen(s) * font_height(font));
            break;
        case kOrientInverted:
            if (jh == kAlignCenter) x += (strlen(s) * font_width(font)) / 2;
            if (jh == kAlignRight)  x += (strlen(s) * font_width(font));
            if (jv == kAlignMiddle) y += font_height(font) / 2;
            if (jv == kAlignBottom) y += font_height(font);
            break;
        case kOrientCounterClock:
            if (jh == kAlignCenter) x -= font_height(font) / 2;
            if (jh == kAlignRight)  x -= font_height(font);
            if (jv == kAlignMiddle) y += (strlen(s) * font_width(font)) / 2;
            if (jv == kAlignBottom) y += (strlen(s) * font_width(font));
            break;
        case kOrientClockwise:
            if (jh == kAlignCenter) x += font_height(font) / 2;
            if (jh == kAlignRight)  x += font_height(font);
            if (jv == kAlignMiddle) y -= (strlen(s) * font_width(font)) / 2;
            if (jv == kAlignBottom) y -= (strlen(s) * font_width(font));
            break;
        default:
            return kInternalFault;
    }

    MmResult result = kOk;
    while (*s && SUCCEEDED(result)) {
        result = graphics_draw_char(surface, &x, &y, font, fcolour, bcolour, *s++, jo);
    }

    return result;
}

MmResult graphics_set_font(uint32_t font_id, int scale) {
    if (font_id > FONT_TABLE_SIZE) {
        return kInvalidFont;
    } else if (!FontTable[font_id]) {
        return kInvalidFont;
    } else if (scale > 15) {
        return kInvalidFontScaling;
    }
    graphics_font = (font_id << 4) | scale;
    return kOk;
}

static MmResult graphics_destroy_surfaces_0_to_63() {
    MmResult result = kOk;
    for (MmSurfaceId id = 0; id < 64 && SUCCEEDED(result); ++id) {
        result = graphics_surface_destroy(&graphics_surfaces[id]);
    }
    return result;
}

static inline MmResult graphics_scroll_down(MmSurface *surface, int y, MmGraphicsColour fill) {
    assert(y > 0);
    const size_t width_in_bytes = surface->width * sizeof(uint32_t);

    uint32_t *buffer = NULL;
    if (fill == -2) {
        buffer = malloc(y * width_in_bytes);
        if (!buffer) return kOutOfMemory;
        memcpy(buffer,
               surface->pixels + (surface->height - y) * surface->width,
               y * width_in_bytes);
    }

    memmove(surface->pixels + y * surface->width,
            surface->pixels,
            (surface->height - y) * width_in_bytes);

    if (fill >= 0) {
        const uint32_t *limit = surface->pixels + y * surface->width;
        for (uint32_t *p = surface->pixels; p < limit; ++p) *p = fill;
    } else if (fill == -2) {
        memcpy(surface->pixels, buffer, y * width_in_bytes);
        free(buffer);
    }

    return kOk;
}

static inline MmResult graphics_scroll_up(MmSurface *surface, int y, MmGraphicsColour fill) {
    assert(y > 0);
    const size_t width_in_bytes = surface->width * sizeof(uint32_t);

    uint32_t *buffer = NULL;
    if (fill == -2) {
        buffer = malloc(y * width_in_bytes);
        if (!buffer) return kOutOfMemory;
        memcpy(buffer, surface->pixels, y * width_in_bytes);
    }

    memmove(surface->pixels,
            surface->pixels + y * surface->width,
            (surface->height - y) * width_in_bytes);

    if (fill >= 0) {
        const uint32_t *limit = surface->pixels + surface->height * surface->width;
        for (uint32_t *p = surface->pixels + (surface->height - y) * surface->width;
             p < limit;
             ++p) *p = fill;
    } else if (fill == -2) {
        memcpy(surface->pixels + (surface->height - y) * surface->width,
               buffer,
               y * width_in_bytes);
        free(buffer);
    }

    return kOk;
}

static inline MmResult graphics_scroll_left(MmSurface *surface, int x, MmGraphicsColour fill) {
    assert(x > 0);
    const size_t width_in_bytes = surface->width * sizeof(uint32_t);

    uint32_t *buffer = NULL;
    if (fill == -2) {
        buffer = malloc(x * sizeof(uint32_t));
        if (!buffer) return kOutOfMemory;
    }

    uint32_t *p = surface->pixels;
    const size_t num_bytes_to_scroll = x * sizeof(uint32_t);
    const size_t num_bytes_to_move = width_in_bytes - num_bytes_to_scroll;

    for (int y = 0; y < surface->height; ++y) {
        if (fill == -2) memcpy(buffer, p, num_bytes_to_scroll);
        memmove(p, p + x, num_bytes_to_move);
        if (fill >= 0) {
            const uint32_t *limit = p + surface->width;
            for (uint32_t *p2 = p + surface->width - x; p2 < limit; ++p2) *p2 = fill;
        } else if (fill == -2) {
            memcpy(p + surface->width - x, buffer, num_bytes_to_scroll);
        }
        p += surface->width;
    }

    if (fill == -2) free(buffer);
    return kOk;
}

static inline MmResult graphics_scroll_right(MmSurface *surface, int x, MmGraphicsColour fill) {
    assert(x > 0);
    const size_t width_in_bytes = surface->width * sizeof(uint32_t);

    uint32_t *buffer = NULL;
    if (fill == -2) {
        buffer = malloc(x * sizeof(uint32_t));
        if (!buffer) return kOutOfMemory;
    }

    uint32_t *p = surface->pixels;
    const size_t num_bytes_to_scroll = x * sizeof(uint32_t);
    const size_t num_bytes_to_move = width_in_bytes - num_bytes_to_scroll;

    for (int y = 0; y < surface->height; ++y) {
        if (fill == -2) memcpy(buffer, p + surface->width - x, num_bytes_to_scroll);
        memmove(p + x, p, num_bytes_to_move);
        if (fill >= 0) {
            const uint32_t *limit = p + x;
            for (uint32_t *p2 = p; p2 < limit; ++p2) *p2 = fill;
        } else if (fill == -2) {
            memcpy(p, buffer, num_bytes_to_scroll);
        }
        p += surface->width;
    }

    if (fill == -2) free(buffer);
    return kOk;
}

MmResult graphics_scroll(MmSurface *surface, int x, int y, MmGraphicsColour fill) {
    if (surface->type == kGraphicsNone) return kGraphicsInvalidSurface;
    // TODO: Sanity check x.
    // TODO: Sanity check y.

    MmResult result = kOk;

    if (x != 0) {
        result = (x > 0)
                ? graphics_scroll_right(surface, x, fill)
                : graphics_scroll_left(surface, -x, fill);
    }

    if (y != 0 && SUCCEEDED(result)) {
        result = (y > 0)
                ? graphics_scroll_up(surface, y, fill)
                : graphics_scroll_down(surface, -y, fill);
    }

    if (SUCCEEDED(result)) surface->dirty = true;
    return result;
}

static MmResult graphics_simulate_cmm2(uint8_t mode) {
    if (mode < MIN_CMM2_MODE || mode > MAX_CMM2_MODE) return kInvalidMode;
    const ModeDefinition *mode_def = &CMM2_MODES[mode];

    MmResult result = graphics_destroy_surfaces_0_to_63();
    if (SUCCEEDED(result)) {
        // Create window for page 0.
        char title[32];
        sprintf(title, "Colour Maximite 2 - Mode %d", mode);
        graphics_window_create(0, -1, -1, mode_def->width, mode_def->height, 10, title, NULL);
    }
    if (SUCCEEDED(result)) {
        // Create buffers for remaining pages.
        for (MmSurfaceId id = 1; id < (MmSurfaceId) mode_def->num_pages && SUCCEEDED(result); ++id) {
            result = graphics_buffer_create(id, mode_def->width, mode_def->height);
        }
    }
    if (SUCCEEDED(result)) {
        result = graphics_surface_write(0);
    }
    if (SUCCEEDED(result)) {
        graphics_fcolour = RGB_WHITE;
        graphics_bcolour = RGB_BLACK;
        result = graphics_set_font(mode_def->font, 1);
    }
    return result;
}

static MmResult graphics_simulate_gamemite(uint8_t mode) {
    if (mode != 0) return kInvalidMode;

    MmResult result = graphics_destroy_surfaces_0_to_63();
    if (SUCCEEDED(result)) {
        result = graphics_window_create(0, -1, -1, 320, 240, 10, "Game*Mite", NULL);
    }
    if (SUCCEEDED(result)) {
        result = graphics_buffer_create(GRAPHICS_SURFACE_N, 320, 240);
    }
    if (SUCCEEDED(result)) {
        result = graphics_surface_write(GRAPHICS_SURFACE_N);
    }
    if (SUCCEEDED(result)) {
        graphics_fcolour = RGB_WHITE;
        graphics_bcolour = RGB_BLACK;
        result = graphics_set_font(7, 1);
    }
    return result;
}

static MmResult graphics_simulate_picomite_vga(uint8_t mode) {
    if (mode < MIN_PMVGA_MODE || mode > MAX_PMVGA_MODE) return kInvalidMode;
    const ModeDefinition *mode_def = &PICOMITE_VGA_MODES[mode];

    MmResult result = graphics_destroy_surfaces_0_to_63();
    if (SUCCEEDED(result)) {
        char title[32];
        sprintf(title, "PicoMiteVGA - Mode %d", mode);
        result = graphics_window_create(0, -1, -1, mode_def->width, mode_def->height, 10, title,
                                        NULL);
    }
    if (SUCCEEDED(result)) {
        result = graphics_buffer_create(GRAPHICS_SURFACE_N, mode_def->width, mode_def->height);
    }
    if (SUCCEEDED(result)) {
        result = graphics_surface_write(GRAPHICS_SURFACE_N);
    }
    if (SUCCEEDED(result)) {
        graphics_fcolour = RGB_WHITE;
        graphics_bcolour = RGB_BLACK;
        result = graphics_set_font(mode_def->font, 1);
    }
    return result;
}

MmResult graphics_simulate_display(OptionsSimulate platform, uint8_t mode) {
    switch (platform) {
        case kSimulateCmm2:
            return graphics_simulate_cmm2(mode);
        case kSimulatePicoMiteVga:
            return graphics_simulate_picomite_vga(mode);
        case kSimulateGameMite:
            return graphics_simulate_gamemite(mode);
        default:
            return kInternalFault;
    }
}

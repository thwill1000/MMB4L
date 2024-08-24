/*-*****************************************************************************

MMBasic for Linux (MMB4L)

sprite.h

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

#if !defined(MMBASIC_SPRITE_H)
#define MMBASIC_SPRITE_H

#include "graphics.h"

#include <stdbool.h>

#define SPRITE_SCROLL_COLLISION  -1
#define SPRITE_NO_COLLISION  -2

typedef enum {
   kSpriteEdgeLeft = 0x01,
   kSpriteEdgeTop = 0x02,
   kSpriteEdgeRight = 0x04,
   kSpriteEdgeBottom = 0x08
} SpriteEdge;

/**
 * Id of the sprite involved in the last collision:
 *  SPRITE_NO_COLLISION     - if there are no active collisions.
 *  SPRITE_SCROLL_COLLISION - if the last collision was due to a SPRITE SCROLL or SPRITE RESTORE.
 */
extern MmSurfaceId sprite_last_collision;

extern MmGraphicsColour sprite_transparent_colour;

/** Initialises sprite module. */
MmResult sprite_init();

/** Terminates sprite module. */
MmResult sprite_term();

/** Gets the total number of active sprites. */
MmResult sprite_count(size_t *count);

/** Gets the total number of active sprites on a given layer. */
MmResult sprite_count_on_layer(unsigned layer, size_t *total);

/**
 * Closes/destroys the given sprite.
 *
 * @param[in]   sprite  Pointer to the sprite.
 */
MmResult sprite_destroy(MmSurface *sprite);

/**
 * Closes/destroys all sprites.
 */
MmResult sprite_destroy_all();

/**
 * Gets the number of collisions a given sprite is currently involved in ?
 *
 * @param[in]   sprite  Pointer to the sprite.
 * @param[out]  count   On exit, the number of collisions the sprite is involved in.
 */
MmResult sprite_get_num_collisions(MmSurface *sprite, uint32_t *count);

 /**
  * Gets the ID of the n'th sprite that a given sprite is currently collided with.
  *
  * @param[in]   sprite  Pointer to the sprite.
  * @param[in]   n       Number of the collision to interrogate.
  * @param[out]  id      On exit, the ID of the sprite collided with.
  *                      If the collision was with a surface edge then OR of
  *                      0xFFF1 (left), 0xFFF2 (top), 0xFFF4 (right), 0xFFF8 (bottom).
  *                      If n is greater than the number of collisions then -1.
  */
MmResult sprite_get_collision(MmSurface *sprite, uint32_t n, MmSurfaceId *id);

/**
 * Gets the total number of sprites currently involved in collisions.
 *
 * @param[out]  count   On exit, the total number of sprites currently involved in collisions.
 */
MmResult sprite_get_num_collided_sprites(uint32_t *count);

/**
 * Gets the ID of the n'th sprite currently involved in a collision.
 *
 * @param[in]   n   Number of the collision to interrogate.
 * @param[out]  id  ID of the sprite, or -1 if n is greater than the number of collisions.
 */
MmResult sprite_get_collided_sprite(uint32_t n, MmSurfaceId *id);

/** TODO */
MmResult sprite_hide(MmSurface *sprite);

/** TODO */
MmResult sprite_hide_safe(MmSurface *sprite);

/** TODO */
MmResult sprite_hide_all();

/** TODO */
MmResult sprite_move();

/** TODO */
MmResult sprite_update_all_collisions();

/** TODO */
MmResult sprite_update_collisions(MmSurface *sprite);

/**
 * Gets a 64-bit slice of the bitset of sprites that a given sprite is currently collided with.
 *
 * @param[in]   id      The ID of the sprite to query.
 * @param[in]   start   Bit to start the slice from; must be 0, 64, 128 or 192.
 * @param[out]  bitset  64-bit set. Bit x is set if the sprite being queried is currently
 *                      collided with sprite start_id + x.
 */
MmResult sprite_get_collision_bitset(MmSurface *sprite, uint8_t start, uint64_t *bitset);

// TODO
MmResult sprite_restore_all();

/**
 * Scrolls the background and any sprites on layer 0 'x' pixels to the right and 'y'
 * pixels up. 'x' can be any number between -MM.HRES-1 and MM.HRES-1,
 * 'y' can be any number between -MM.VRES-1 and MM.VRES-1.
 * Sprites on any layer other than zero will remain fixed in position on the
 * screen. By default the scroll wraps the image round. If 'col' is specified the
 * colour will replace the area behind the scrolled image. If 'col' is set to -1 the
 * scrolled area will be left untouched.
 *
 * @param  x       Number of pixels to scroll the background right (may be -ve).
 * @param  y       Number of pixels to scroll the background down (may be -ve).
 * @param  colour  Colour to replace the area behind the scrolled image.
 *                 -1 = leave scrolled area untouched.
 *                 -2 = wrap scrolled area around.
 */
MmResult sprite_scroll(int x, int y, MmGraphicsColour colour);

/**
 * Sets the transparent colour for sprites.
 *
 * @param  colour  The transparent colour.
 */
MmResult sprite_set_transparent_colour(MmGraphicsColour colour);

/**
 * Shows (blits and makes active) a sprite.
 *
 * @param  sprite       Pointer to the sprite.
 * @param  dst_surface  Surface to blit the sprite to.
 * @param  x, y         Top left coordinates on the destination surface.
 * @param  layer        'Layer' that the sprite is to be shown on.
 * @param  flags        Bitwise AND of:
 *                          0x01 = mirrored left to right.
 *                          0x02 = mirrored top to bottom.
 *                          0x04 = no transparency, all pixels opaque.
 */
MmResult sprite_show(MmSurface *sprite, MmSurface *dst_surface, int x, int y, unsigned layer,
                     unsigned flags);

/**
 * Shows (blits and makes active) a sprite compensating for other sprites that overlap it.
 *
 * @param  ontop  If true then the sprite will be moved up the stack to become the newest
 *                sprite so it will sit on top of any other sprite it overlaps.
 */
MmResult sprite_show_safe(MmSurface *sprite, MmSurface *dst_surface, int x, int y,
                          unsigned layer, unsigned flags, bool ontop);

#endif // #if !defined(MMBASIC_SPRITE_H)
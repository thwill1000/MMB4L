/*-*****************************************************************************

MMBasic for Linux (MMB4L)

stack.h

Copyright 2024 Geoff Graham, Peter Mather and Thomas Hugo Williams.

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

#if !defined(MMB4L_STACK_H)
#define MMB4L_STACK_H

#include "mmresult.h"

#include <stdlib.h>
#include <string.h>

typedef void *StackElement;

typedef struct {
  size_t element_size;
  char *storage;
  char *top;   // Insertion point for next element.
  char *limit; // Cannot insert once 'top' reaches this.
  bool (*equals_fn)(const void *, const void *);
} Stack;

/**
 * Initialises a Stack structure including allocating storage.
 *
 * @param  s          Pointer to the Stack structure to initialise.
 * @param  type       Element type, e.g. uint8_t, int32_t, void *
 * @param  capacity   Maximum number of elements that the stack should be able to hold.
 * @param  equals_fn  Function to use to compare elements for equality.
 *                    If NULL then uses memcmp() but this is not appropriate if elements
 *                    are structures due to potential random padding characters.
 */
#define stack_init(s, type, capacity, equals_fn)  \
    stack_init_internal(s, sizeof(type), capacity, equals_fn)

static MmResult stack_init_internal(Stack *s, size_t element_size, size_t capacity,
                                    bool (*equals_fn)(const void *, const void *)) {
    s->element_size = element_size;
    s->storage = (char *) malloc(capacity * element_size);
    if (!s->storage) return kOutOfMemory;
    s->top = s->storage;
    s->limit = s->storage + capacity * element_size;
    s->equals_fn = equals_fn;
    return kOk;
}

/**
 * Terminates a Stack structure freeing its internal storage.
 *
 * @param  s  Pointer to the Stack to terminate.
 */
static MmResult stack_term(Stack *s) {
    free(s->storage);
    memset(s, 0x0, sizeof(Stack));
    return kOk;
}

/**
 * Does the stack contain a given element?
 *
 * @param[in]  s  Pointer to the Stack.
 * @param[in]  e  The element to search for.
 * @return        true if the stack contains the element, otherwise false.
 */
#define stack_contains(s, e)  stack_contains_internal(s, (StackElement) &(e))

static inline bool stack_contains_internal(const Stack *s, StackElement element) {
    // For no particular reason searches from the top of the stack down.
    for (char *p = s->top - s->element_size;
        p >= s->storage;
        p -= s->element_size) {
        const bool equal = s->equals_fn
                ? s->equals_fn((void *) p, (void *) element)
                : (memcmp(p, element, s->element_size) == 0);
        if (equal) return true;
    }
    return false;
}

/**
 * Gets the top element of the stack WITHOUT removing it.
 *
 * @param[in]  s  Pointer to the Stack.
 * @param[out] e  On exit contains a shallow copy of the top element.
 * @return        kContainerEmpty if the stack is empty.
 */
static inline MmResult stack_peek(const Stack *s, StackElement element) {
    if (s->top == s->storage) return kContainerEmpty;
    memcpy(element, s->top - s->element_size, s->element_size);
    return kOk;
}

/**
 * Gets the top element of the stack AND removes it.
 *
 * @param[in]  s  Pointer to the Stack.
 * @param[out] e  On exit contains a shallow copy of the (now removed) top element.
 * @return        kContainerEmpty if the stack is empty.
 */
static inline MmResult stack_pop(Stack *s, StackElement element) {
    if (s->top == s->storage) return kContainerEmpty;
    s->top -= s->element_size;
    memcpy(element, s->top, s->element_size);
    return kOk;
}

/**
 * Adds an element to the stop of the stack.
 *
 * @param[in]  s  Pointer to the Stack.
 * @param[in]  e  The element to add.
 * @return        kContainerFull if the stack is full.
 */
#define stack_push(s, e)  stack_push_internal(s, (StackElement) &(e))

static inline MmResult stack_push_internal(Stack *s, const StackElement element) {
    if (s->top == s->limit) return kContainerFull;
    memcpy(s->top, element, s->element_size);
    s->top += s->element_size;
    return kOk;
}

/**
 * Gets an element by 0-based index from the stack.
 *
 * @param[in]  s  Pointer to the Stack.
 * @param[in]  i  Index of the element to get.
 * @param[out] e  On exit contains a shallow copy of the indexed element.
 * @return        kStackIndexOutOfBounds if i >= the number of elements in the stack.
 */
static inline MmResult stack_get(const Stack *s, size_t idx, StackElement element) {
    if (s->storage + idx * s->element_size >= s->top) return kStackIndexOutOfBounds;
    memcpy(element, s->storage + idx * s->element_size, s->element_size);
    return kOk;
}

/**
 * Removes the first matching element (starting from the top of the stack).
 *
 * @param[in]  s  Pointer to the Stack.
 * @param[in]  e  The element to remove.
 * @return        kStackElementNotFound if no matching element is found.
 */
#define stack_remove(s, e)  stack_remove_internal(s, (StackElement) &(e))

static inline MmResult stack_remove_internal(Stack *s, const StackElement element) {
    for (char *p = s->top - s->element_size;
        p >= s->storage;
        p -= s->element_size) {
        const bool equal = s->equals_fn
                ? s->equals_fn((void *) p, (void *) element)
                : (memcmp(p, element, s->element_size) == 0);
        if (equal) {
            s->top -= s->element_size;
            memmove(p, p + s->element_size, s->top - p);
            return kOk;
        }
    }
    return kStackElementNotFound;
}

/**
 * Replaces the first matching element (starting from the top of the stack).
 *
 * @param[in]  s  Pointer to the Stack.
 * @param[in]  f  The element to find.
 * @param[in]  r  The element to replace it with.
 * @return        kStackElementNotFound if no matching element is found.
 */
#define stack_replace(s, f, r) \
    stack_replace_internal(s, (StackElement) &(f), (StackElement) &(r))

static inline MmResult stack_replace_internal(Stack *s, const StackElement find,
                                              const StackElement replace) {
    for (char *p = s->top - s->element_size;
         p >= s->storage;
         p -= s->element_size) {
        const bool equal = s->equals_fn
                ? s->equals_fn((void *) p, (void *) find)
                : (memcmp(p, find, s->element_size) == 0);
        if (equal) {
            memcpy(p, replace, s->element_size);
            return kOk;
        }
    }
    return kStackElementNotFound;
}

/**
 * Gets the size of the stack.
 *
 * @param[in]  s  Pointer to the Stack.
 * @return        The size of the Stack.
 */
static inline size_t stack_size(const Stack *s) {
    return (s->top - s->storage) / s->element_size;
}

/**
 * Dumps the contents of the stack to STDOUT.
 */
static void stack_dump(const Stack *s) {
    printf("--- BASE ---\n");
    size_t size = (s->top - s->storage) / s->element_size;
    StackElement element;
    for (size_t idx = 0; idx < size; ++idx) {
        (void) stack_get(s, idx, &element);
        printf("  0x%p\n", element);
    }
    printf("--- TOP  ---\n");
}

#endif // #if !defined(MMB4L_STACK_H)

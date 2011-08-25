
/* stack.c - Implements a stack as an array and defines functions
 * to operate it. It is used in performing a code/equation balance
 * by checking for opening and closing braces in an expression.
 * Check on stack.h for various error codes.
 * (C) 2011 Raman Gopalan <ramangopalan@gmail.com>
 */

/*
  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the Free
  Software Foundation; either version 2 of the License, or (at your option)
  any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
  for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc., 59
  Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"

void stack_init(struct codebal *s)
{
    int i;
    s->top = NIL;
    for(i = 0; i < STACK_SIZE; i++)
        *((s->stack) + i) = 0;
}

int stack_push(struct codebal *s, int val)
{
    if(s->top == STACK_SIZE - 1)
        return STACK_OVERFLOW;
    ++(s->top);
    *((s->stack) + s->top) = val;
    return s->top;
}

int stack_pop(struct codebal *s)
{
    int temp;
    if(s->top == NIL)
	return STACK_UNDERFLOW;
    temp = *((s->stack) + s->top);
    *((s->stack) + s->top) = 0;
    --(s->top);
    return temp;
}

int stack_isempty(struct codebal* s)
{
    if(s->top == NIL)
        return 0;
    return STACK_NOT_EMPTY;
}

/* Checks for equation balance. */
int balance(struct codebal *b, char *bftext)
{
        int i;
        stack_init(b);
        for(i = 0; *(bftext + i); i++) {
                if(*(bftext + i) == OPEN_BRACE) {
                        if(stack_push(b, *(bftext + i)) == STACK_OVERFLOW)
				return STACK_OVERFLOW;
                } else if(*(bftext + i) == CLOSE_BRACE) {
                        if(stack_pop(b) == STACK_UNDERFLOW)
				return STACK_UNDERFLOW;
                }
        }
        if(stack_isempty(b) == STACK_NOT_EMPTY)
		return NOT_BALANCED;
        return BALANCED;
}

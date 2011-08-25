
/* stack.h - Stack implemented as an array to check
 * Brainfuck code balance.
 * (C) 2011 Raman Gopalan <ramangopalan@gmail.com>
 */

/*
 This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your option) any
later version.

  This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
details.

  You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc., 59
Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef STACK_H
#define STACK_H

/* Balance check for brackets. */
#define OPEN_BRACE      '['
#define CLOSE_BRACE     ']'

/* Error codes. */
#define STACK_SIZE       1000
#define NIL              -1
#define STACK_EMPTY       0
#define STACK_NOT_EMPTY   1
#define STACK_OVERFLOW   -2
#define STACK_UNDERFLOW   3
#define BALANCED          4
#define NOT_BALANCED	  5
#define STACK_ERROR	  6

/* Stack. */
struct codebal {
    char stack[STACK_SIZE];	
    int top;
};

/* Stack operations. */
void stack_init(struct codebal *);
int stack_push(struct codebal *, int);
int stack_pop(struct codebal *);
int stack_isempty(struct codebal *);
/* Checks for code balance. */
int balance(struct codebal *, char *);

#endif

/* vim: set encoding=latin1: */

/* gbf.h - some includes, prototype definitions and preprocessor abuse
 * for the BrainFuck interpreter.
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

#include <stdlib.h> /* malloc(), realloc() */
#include <string.h> /* memset() */
#include <unistd.h> /* getopt() etc. (not in Linux, though) */
#include <stdio.h>  /* *printf(), FILE * etc. */

/* Linux systems seem to require that getopt.h be explicitly included;
 * glibc will work only on Linux; ergo, if we are running glibc, we are
 * running Linux. */
#ifdef __GLIBC__
#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif
#include <getopt.h>
#endif // _GLIBC_

#define BUF 4096 /* input buffer size increment */

/* our tape's node */
struct fita
{
	int carga;
	struct fita * prox;
	struct fita * prev;
};

/* moves to the next node in the tape */
struct fita *fita_prox(struct fita *ft);
/* moves to the previous node in the tape */
struct fita *fita_prev(struct fita *ft);
/* cleans up the memory ere program exit */
void limpa_fita (struct fita *ft);
/* displays program's usage */
void uso (char *progname);


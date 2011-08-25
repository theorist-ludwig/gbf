
/* gbrainfuck.c - BrainFuck language interpreter. Inspired by Thomas Cort's
 * work. This interpreter improves other BrainFuck interpreters because it uses
 * a doubly linked lists, therefore eliminating hard-coded limits for a BF
 * program (other interpreters usually use an array of pre-defined size as
 * tape). Also, there is no left limit (except, of course, the one imposed by
 * the underlying system memory). Have fun!
 * (C) 2003, 2004 José de Paula <jose_de_paula@ig.com.br> Original author
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

#include <stdlib.h> /* malloc(), realloc() */
#include <string.h> /* memset() */
#include <stdio.h>  /* *printf(), FILE * etc. */
#include <unistd.h> /* getopt() etc. */
#include "stack.h"

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


/* goes to the next node in the tape */
struct fita * fita_prox(struct fita * ft)
{
	struct fita * tmp;

	/* if next tape's cell hasn't been allocated,
	 * do it...
	 */
	if (ft->prox == NULL)
	{
		if ((tmp = malloc(sizeof(struct fita*))) == NULL)
		{
			fprintf(stderr, "Error: insufficient memory for ft->prox in fita_prox()!\n");
			exit(EXIT_FAILURE);
		}
		tmp->prox = NULL;
		tmp->prev = ft;
		tmp->carga = 0;

		ft->prox = tmp;
		ft = tmp;
	}

	/* ... else, just move the pointer */
	else
	{
		tmp = ft;
		ft = tmp->prox;
	}
	return  ft;
}

/* moves to the previous tape node */
struct fita * fita_prev(struct fita * ft)
{
	struct fita * tmp;
	if (ft->prev == NULL)
	{
		if ((tmp = malloc(sizeof(*tmp))) == NULL)
		{
			fprintf(stderr, "Error: insufficient memory for ft->prev in fita_prev()!\n");
			exit(EXIT_FAILURE);
		}

		tmp->carga = 0;
		tmp->prev = NULL;
		tmp->prox = ft;

		ft->prev = tmp;
		ft = tmp;
	}
	else
	{
		tmp = ft;
		ft = tmp->prev;
	}

	return ft;
}

/* cleanup before program exit */
void limpa_fita (struct fita * ft)
{
	while (ft->prox != NULL)
		ft = ft->prox; 

 	while (ft->prev != NULL)
 	{
 		ft = ft->prev;
 		free(ft->prox);
	}
	
	free(ft);
}

void usage(FILE * stream, int status, char const * str)
{
	fprintf (stream, "%s [-f file] [-o file]\n", str);
	fprintf (stream, "%s -h\n", str);
	fprintf (stream, "\t-f file\tread the BrainFuck program from file.bf\n"
			"\t-o file\toutput the BrainFuck program results to file.\n"
			"\t-h\tshow this help and exit.\n");
	fprintf (stream, "\nIf invoked without arguments, the interpreter will run interactively.\n"
			"Type in your BrainFuck program and use 'Control-D' to terminate input.\n");
	exit(status);
}

int main (int argc, char *argv[])
{
	struct fita *ft = malloc(sizeof(*ft)); /* tape*/
	char *prog;	/* buffer where program text is stored */
	char * tmp;	/* helps growing *prog */
	FILE * entrada;	/* standard input of the BF program */
	FILE * texto;	/* our interpreter's input */
	FILE * saida;	/* standard output of the BF program */
	unsigned long int i = 0,/* BF program's text counter */
		bfsize = 0; /* size of the BF program */
	int a = BUF, 	/* will keep track of [] nesting */
		pages = 2, /* pages of input program text */
		brackets = 0, /* counts brackets */
		ch;  /* yet another auxiliary; helps with getopt() */
	int e;		/* temporarily stores characters from BF program */
	char *nometexto = NULL, *nomesaida = NULL; /* the names of input and output files */

	struct codebal b;
	
	/* FIXME: getopt() is not portable outside POSIX. */
	while ( (ch = getopt(argc, argv, "o:f:h")) != -1)
	{
		switch (ch)
		{
			case 'o':
				nomesaida = optarg;
				break;
			case 'f':
				nometexto = optarg;
				break;
			case 'h':
				usage(stdout, EXIT_SUCCESS, argv[0]);
				break;
			case '?':
			default:
				usage(stderr, EXIT_FAILURE, argv[0]);
				break;
		}
	}

	/* tape setup */
	ft->prev = NULL;
	ft->prox = NULL;
	ft->carga = 0;

	(nometexto == NULL) ? (texto = stdin) : (texto = fopen(nometexto, "r"));

	if (texto == NULL)
	{
		fprintf(stderr, "couldn't open file %s for reading. Bailing out.\n", nometexto);
		exit(EXIT_FAILURE);
	}

	(nomesaida == NULL) ? (saida = stdout) : (saida = fopen(nomesaida, "w"));
	if (saida == NULL)
	{
		fprintf(stderr, "couldn't open file %s for writing. Bailing out.\n", nomesaida);
		exit (EXIT_FAILURE);
	}


	prog = malloc (BUF * sizeof(*prog) );
	memset(prog, 0, (BUF) );

inicio:
	while ( (!feof(texto)) && (e = fgetc(texto)) && (!ferror(texto)) )
	{
		switch(e)
		{
			case '[':
				++brackets;
				prog[bfsize] = (char)e;
				break;
			case ']':
				prog[bfsize] = (char)e;
				break;
			case '+':
			case '-':
			case '>': 
			case '<':
			case '.':
			case ',':
				prog[bfsize] = (char)e;
				break;
			default:
				goto inicio; /* right before the beginning
						of this loop; I believe
						C++ allows a 'break inicio',
						but I'm not certain about C.*/
		}

		a--;
		/* when our initial 4KB buffer runs out, we allocate
		 * more 4KB */
		if (!a)

		{
			if ( (tmp = realloc(prog, sizeof(*prog) + BUF * pages)) == NULL)
			{
				fprintf (stderr, "realloc() failed reading BrainFuck program text!\n");
				exit(EXIT_FAILURE);
			}
			/* let's play it safe and zero out the newly
			 * allocated buffer.  */
			memset(&(tmp[bfsize+1]), 0, (BUF) );
			prog = tmp;
			a = BUF;
			pages++;
		}
		bfsize++;
	}

	/* having read the whole input, we check whether the 
	 * brackets match; if not, we bail out. (stack.c)
	 */
	
	brackets = balance(&b, prog);
	/* If stack not empty or underflow, indicate code imbalance. */
	if((brackets == NOT_BALANCED) || (brackets == STACK_UNDERFLOW)) {
		free(prog);
		fprintf(stderr, "Unmatched []! Please verify your program!\n");
		exit(EXIT_FAILURE);
	} 
	
	/* Overflow indicates stack size too small for the number of ['s. */
	if(brackets == STACK_OVERFLOW) { 
		free(prog);
		fprintf(stderr, "codebal module: Stack overflow!\n");
		exit(EXIT_FAILURE);
	}

	/* FIXME: this ain't portable; works on FreeBSD and perhaps on other
	 * Unices; clearerr() should do the trick, but seems to be broken on
	 * FreeBSD. */
	fclose (stdin);
	freopen("/dev/tty", "r", stdin);


	entrada = stdin;

	if (entrada == NULL)
	{
		fprintf (stderr, "erro em fdopen().\n");
		perror("");
	}

	if (feof(entrada) || ferror(entrada))
	{
		fprintf(stderr, "Error. End of file in stdin. That's not good.\n");
		exit(EXIT_FAILURE);
	}


	/* interpret the program */


	a = 0; /* we zero this because it will be used for
		  keeping track of [] */
	for (i = 0; (i <= bfsize) && (prog[i] != 0); i++)
	{
		switch(prog[i])
		{
			case '.':
				fputc(ft->carga, saida);
				break;
			case ',':
				ft->carga = getc(entrada);
				break;
			case '>':
				ft = fita_prox(ft);
				break;
			case '<':
				ft = fita_prev(ft);
				break;

			/* if current tape cell payload is 0, look for
			 * matching ] and jump to immediately following
			 * instruction */
			case '[':
				{
					if (ft->carga == 0) 
					{
						i++; 
						while ( (a > 0) || (prog[i] != ']' ))
						{
							if (prog[i] == '[')
							       a++;
							if (prog[i] == ']')
							       a--;
							if (++i > bfsize)
							{
								limpa_fita(ft);
								free(prog);
								exit(EXIT_FAILURE);
							}
						}
					}

				};
				break;

			/* loop closing: look for matching '[' instruction
			 * and evaluate it as previous rule */
			case ']':
				{
					i--;
					while ( (a > 0) || (prog[i] != '['))
					{
						if (prog[i] == ']')
						       a++;
						if (prog[i] == '[')
						       a--;
						i--;
					}
					i--;
				};
				break;
			case '+':
				(ft->carga)++;
				break;
			case '-':
				(ft->carga)--;
				break;
			default:
				limpa_fita(ft);
				free(prog);
				exit(EXIT_FAILURE);
				/* something horrible ocurred; anything that
				  isn't a keyword shouldn't be in prog[i] */
		}
	}

	/* cleanup */
	limpa_fita(ft);
	free(prog);
	return 0;
}


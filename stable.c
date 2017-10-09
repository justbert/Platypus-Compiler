/*	Filename: stable.c
 *	PURPOSE: A symbol table which stores variable identifiers for a	
 *		program
 *  SCANNER.C: Functions implementing a Symbol Table
 *		as required for CST8152, Assignment #3
 *  st_create(size) must be called before using the symbol table.
 *	Compiler: MS Visual Studio 2013, Update 5
 *	Course :	CST 8152 - Compiler, Lab Section: 011
 *	Assignment:  Assignment 3 - The Symbol Table
 *  Provided by: Svillen Ranev
 *	Modified by: Justin Bertrand, 040 786 592
 *  Version: 1.0
 *  Date: 18 November 2015
 *	Warnings:		Please note that external libraries use non-ANSI single 
					line comments which cause warnings
 */

/* The #define _CRT_SECURE_NO_WARNINGS should be used in MS Visual Studio projects
* to suppress the warnings about using "unsafe" functions like fopen()
* and standard sting library functions defined in string.h.
* The define does not have any effect in other compiler projects.
*/
#define _CRT_SECURE_NO_WARNINGS

/*Include headers*/
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "stable.h"

/*For debugging purposes*/
#define DEBUG
#undef DEBUG

/*Global objects - Variables*/
extern STD sym_table;

/*Local function prototypes*/
static void st_setsize(void);
static void st_incoffset(void);
static pBuffer st_setbuffer(pBuffer);
static int asc_compare(const void*, const void*);
static int desc_compare(const void*, const void*);

/*
Purpose:			Creates symbol table and allocates memory
						on the heap for required structures
Author:				Justin Bertrand
History/Versions:	1.0, Nov. 18th 2015
Called functions:	malloc()
					b_create()
Parameters:			size: an int which will be the amount of STVR
						that can be stored
Return value:		An STD structure
*/
STD st_create(int st_size)
{
	/*Local variables*/
	STD s_table;/*STD being created*/

	if (st_size <= 0)
	{
		s_table.st_size = 0;
		return s_table;
	}

	s_table.pstvr = (STVR*) malloc(sizeof(STVR)*st_size);

	if (!s_table.pstvr)
	{
		s_table.st_size = 0;
		return s_table;
	}
	s_table.st_offset = 0;

	s_table.plsBD = b_create(ST_BUFFER_INIT_CAP, ST_BUFFER_INC_FACTOR, ST_BUFFER_TYPE);

	if (!s_table.plsBD)
	{
		s_table.st_size = 0;
		free(s_table.pstvr);
	}
	else
		s_table.st_size = st_size;

#ifdef DEBUG
	printf("STable created: Offset = %i\n", s_table.st_offset);
#endif

	return s_table;
}

/*
Purpose:			Installs an STVR into the Symbol Table
Author:				Justin Bertrand
History/Versions:	1.0, Nov. 18th 2015
Called functions:	st_lookup()
					strlen()
					b_setmark()
					b_addc()
					b_rflag()
					st_incoffset()
Parameters:			sym_table: The symbol table to install the
						STVR to.
					lexeme: the string of the VID to be installed.
					type: A char representing the type of VID
					line: an integer representing the line of 
						the first appearance of the VID
Return value:		An int representing the location of the VID in 
						symbol table.
*/
int st_install(STD sym_table, char* lexeme, char type, int line)
{
#ifdef DEBUG
	printf("Installing %s to ST\n", lexeme);
#endif
	/*Local variables*/
	int lex_length;
	int index;
	int realloc_index;
	int lookup_result;

	/*Makes sure the pointer is not null*/
	if (!lexeme || !sym_table.st_size)
		return ST_FAIL_1;

		lookup_result = st_lookup(sym_table, lexeme);

		/*If lexeme is already in STD, return position*/
		if (lookup_result != ST_FAIL_1)
			return lookup_result;

	/*Check for room in table*/
	if (sym_table.st_offset == sym_table.st_size)
		return ST_FAIL_1;

	sym_table.pstvr[sym_table.st_offset].o_line = line;
	sym_table.pstvr[sym_table.st_offset].status_field = STATUS_DEFAULT_MASK;

	switch (type)
	{
	case 'S':
	case 's':
		sym_table.pstvr[sym_table.st_offset].status_field |= STATUS_STRING_MASK;
		sym_table.pstvr[sym_table.st_offset].status_field |= STATUS_UPDATE_FLAG_MASK;
		sym_table.pstvr[sym_table.st_offset].i_value.str_offset = IVALUE_STRING;
		break;
	case 'I':
	case 'i':
		sym_table.pstvr[sym_table.st_offset].status_field |= STATUS_INTEGER_MASK;
		sym_table.pstvr[sym_table.st_offset].i_value.int_val = IVALUE_NUMBER;
		break;
	case 'F':
	case 'f':
		sym_table.pstvr[sym_table.st_offset].status_field |= STATUS_FLOAT_MASK;
		sym_table.pstvr[sym_table.st_offset].i_value.fpl_val = IVALUE_FLOAT;
		break;
	}
	
	if (sym_table.st_offset == 0)
	{
		sym_table.pstvr[sym_table.st_offset].plex =
			b_setmark(sym_table.plsBD, 0);
	}
	else
	{
		sym_table.pstvr[sym_table.st_offset].plex =
			sym_table.pstvr[sym_table.st_offset - 1].plex +
			strlen(sym_table.pstvr[sym_table.st_offset - 1].plex)
			+ 1;
	}

	/*Add the lexeme to the buffer*/
	lex_length = strlen(lexeme);
	for (index = 0; index <= lex_length; ++index)
	{
		if (!b_addc(sym_table.plsBD, lexeme[index]))
		{
			return ST_FAIL_1;
		}

		if (b_rflag(sym_table.plsBD) == SET_R_FLAG)
		{
#ifdef DEBUG
			b_print(sym_table.plsBD);
#endif

			/*Set the first value in the Symbol Table to the first */
			sym_table.pstvr[0].plex = b_setmark(sym_table.plsBD, 0);
			if (sym_table.pstvr[0].plex == NULL)
			{
				return ST_FAIL_1;
			}

			for (realloc_index = 1; realloc_index <= sym_table.st_offset; ++realloc_index)
			{ 
				sym_table.pstvr[realloc_index].plex =
					sym_table.pstvr[realloc_index - 1].plex +
					strlen(sym_table.pstvr[realloc_index - 1].plex)
					+ 1;
			}
		}
	}
	
#ifdef DEBUG
	printf("Offset = %i\n", sym_table.st_offset);
#endif

	/*Pass by value causes the changes to STD to be lost,
		must use global modifiers */
	st_incoffset();
	return sym_table.st_offset;
}

/*
Purpose:			Looks in the symbol table to see if
						a VID of the same name as the paramater
						is already present.
Author:				Justin Bertrand
History/Versions:	1.0, Nov. 18th 2015
Called functions:	strcmp()
Parameters:			sym_table: The symbol table to perform the 
						lookup upon.
					lexeme: The name of the VID to search for.
Return value:		An int representing the location of the VID
						in the symbol table, or -1 for a failure.
*/
int st_lookup(STD sym_table, char* lexeme)
{
	/*Local variables*/
	int index; /*Loop index*/

	if (!sym_table.st_size || !lexeme || !sym_table.st_offset)
		return ST_FAIL_1;

#ifdef DEBUG
	printf("%s\n", lexeme);
	b_print(sym_table.plsBD);
#endif

	for (index = sym_table.st_offset-1; index >= 0; --index)
	{
		if (!sym_table.pstvr[index].plex)
			return ST_FAIL_1;

		if (!strcmp(lexeme, sym_table.pstvr[index].plex))
			return index;
	}
	
	return ST_FAIL_1;
}

/*
Purpose:			Updates the type of the VID if possible.
Author:				Justin Bertrand
History/Versions:	1.0, Nov. 18th 2015
Called functions:	none
Parameters:			sym_table: The symbol table to be used.
					vid_offset: The location of the VID to update
						in the sym_table
					type: The type of VID to update to.
Return value:		An int representing the location of the VID
						in the symbol table, or -1 for a failure.
*/
int st_update_type(STD sym_table, int vid_offset, char type)
{
	/*Local variables*/
	STVR* vid;/*Pointer to the current VID to be updated*/

	if (!sym_table.st_size)
		return ST_FAIL_1;

	vid = &sym_table.pstvr[vid_offset];

	/*if the type is the same as that requested, 
		don't update */
	if (type == st_get_type(sym_table, vid_offset))
		return vid_offset;

	/*Checks if the type has already been updated*/
	if (vid->status_field & STATUS_UPDATE_FLAG_MASK)
		return ST_FAIL_1;

	switch (type)
	{
	case 'i': case 'I': case 'f': case 'F':
		vid->status_field ^= STATUS_TYPE_UPDATE_MASK;
		return vid_offset;
	default:
		return ST_FAIL_1;
	}
}

/*
Purpose:			Updates the InitialValue of the VID if possible.
Author:				Justin Bertrand
History/Versions:	1.0, Nov. 18th 2015
Called functions:	none
Parameters:			sym_table: The symbol table to be used.
					vid_offset: The location of the VID to update
						in the sym_table
					InitialValue: The value of the VID to with which
						to be replaced.
Return value:		An int representing the location of the VID
						in the symbol table, or -1 for a failure.
*/
int st_update_value(STD sym_table, int vid_offset, InitialValue i_value)
{
	if (!sym_table.st_size || vid_offset >= sym_table.st_size)
		return ST_FAIL_1;

	sym_table.pstvr[vid_offset].i_value = i_value;

	return vid_offset;
}

/*
Purpose:			Returns the type of the VID specified by 
						the offset in the symbol table.
Author:				Justin Bertrand
History/Versions:	1.0, Nov. 18th 2015
Called functions:	none
Parameters:			sym_table: The symbol table to be used.
					vid_offset: The location of the VID to update
						in the sym_table
Return value:		A char, F for a float, S for a String, and I for an 
						Integer.
*/
char st_get_type(STD sym_table, int vid_offset)
{
	if (!sym_table.st_size || vid_offset >= sym_table.st_size)
		return ST_FAIL_1;

	switch (sym_table.pstvr[vid_offset].status_field & STATUS_TYPE_FLAGS_MASK)
	{
	case STATUS_FLOAT_MASK:
		return ST_CHAR_TYPE_FLOAT;
	case STATUS_INTEGER_MASK:
		return ST_CHAR_TYPE_INT;
	case STATUS_STRING_MASK:
		return ST_CHAR_TYPE_STRING;
	default:
		return ST_FAIL_1;
	}
}

/*
Purpose:			Frees the memory used for the symbol table.
Author:				Justin Bertrand
History/Versions:	1.0, Nov. 18th 2015
Called functions:	b_destroy()
					free()
					st_setsize()
Parameters:			sym_table: The symbol table to be destroyed.
Return value:		None.
*/
void st_destroy(STD sym_table)
{	
	if (sym_table.st_size)
	{
		b_destroy(sym_table.plsBD);
		free(sym_table.pstvr);
		st_setsize();
	}
}

/*
Purpose:			A function which formats and prints the 
						contents of the Symbol table to standard
						output.
Author:				Justin Bertrand
History/Versions:	1.0, Nov. 18th 2015
Called functions:	floor()
					log10()
					memset()
					printf()
Parameters:			sym_table: The symbol table to be printed.
Return value:		An int, -1 for a failure, or a number equal
						or larger than 0 representing the amount
						of entries printed.
*/
int st_print(STD sym_table)
{
	/*Local Variables*/
	int index = 0;
	int largest_line = 0;
	int line_number_padding = 0;
	char padding[ST_PRINT_LINE_SPACES + 1];

	/*check for a valid symbol table*/
	if (!sym_table.st_size)
		return ST_FAIL_1;

	/*If there are no entries, save processing power
		by skipping all the calculations*/
	if (sym_table.st_offset == 0)
	{
		printf("\nSymbol Table\n____________\n\n");
		printf("Line Number Variable Identifier\n");
		return index;
	}

	/*Determine the largest line number in the program*/
	for (index = 1, largest_line = sym_table.pstvr[0].o_line;
		index < sym_table.st_offset; ++index)
	{
		if (sym_table.pstvr[index].o_line > largest_line)
			largest_line = sym_table.pstvr[index].o_line;
	}

	/**/
	if (largest_line > 0)
		line_number_padding = (int) floor(log10(largest_line)) + 1;

	memset(padding, '\0', ST_PRINT_LINE_SPACES + 1);
	memset(padding, ' ', ST_PRINT_LINE_SPACES - line_number_padding);

	printf("\nSymbol Table\n____________\n\n");
	printf("Line Number Variable Identifier\n");

	for (index = 0; index < sym_table.st_offset; ++index)
	{
		printf("%*i%s %s\n", line_number_padding, sym_table.pstvr[index].o_line,
			padding, sym_table.pstvr[index].plex);
	}

	return index;
}

/*
Purpose:			Due to the use of a pass by value system,
						it is necessary to use this function 
						when reseting the size of the symbol
						table to 0.
Author:				Justin Bertrand
History/Versions:	1.0, Nov. 18th 2015
Called functions:	none
Parameters:			None.
Return value:		None.
*/
static void st_setsize(void)
{
	sym_table.st_size = 0;
}

/*
Purpose:			Due to the use of a pass by value system,
						it is necessary to use this function 
						when incrementing the inc_offset by 1.
Author:				Justin Bertrand
History/Versions:	1.0, Nov. 18th 2015
Called functions:	none
Parameters:			None.
Return value:		None.
*/
static void st_incoffset(void)
{
	++sym_table.st_offset;
}

/*
Purpose:			A function which formats and stores the 
						contents of the Symbol table to a file.
Author:				Justin Bertrand
History/Versions:	1.0, Nov. 18th 2015
Called functions:	fopen()
					fprintf()
					fclose()
					printf()
Parameters:			sym_table: The symbol table to be stored.
Return value:		An int, -1 for a failure, or a number equal
						or larger than 0 representing the amount
						of entries printed.
*/
int st_store(STD sym_table)
{
	if (!sym_table.st_size)
		return ST_FAIL_1;

	/*Local variables*/
	int index;
	FILE* file = fopen(ST_STORE_FILENAME, "w");/*Warning: Possibly unsafe*/

	fprintf(file, "%i", sym_table.st_size);

	for (index = 0; index < sym_table.st_offset; ++index)
	{
		fprintf(file, " %X ", sym_table.pstvr[index].status_field);

		fprintf(file, "%i ", strlen(sym_table.pstvr[index].plex));

		fprintf(file, "%s ", sym_table.pstvr[index].plex);

		fprintf(file, "%i ", sym_table.pstvr[index].o_line);

		switch (st_get_type(sym_table,index))
		{
		case ST_CHAR_TYPE_INT:
			fprintf(file, "%i", sym_table.pstvr[index].i_value.int_val);
			break;
		case ST_CHAR_TYPE_STRING:
			fprintf(file, "%i", sym_table.pstvr[index].i_value.str_offset);
			break;
		case ST_CHAR_TYPE_FLOAT:
			fprintf(file, "%.2f", sym_table.pstvr[index].i_value.fpl_val);
			break;
		}
	}

	fclose(file);
	printf("\nSymbol Table stored.\n");

	return index;
}

/*
Purpose:			A function which sorts the STVRs by ascending or
						descending order.
Author:				Justin Bertrand
History/Versions:	1.0, Nov. 18th 2015
Called functions:	qsort()
					b_create()
					b_capacity()
					asc_compare
					desc_compare
					strlen()
					b_add()
					b_setmark()
					st_setbuffer()
Parameters:			sym_table: The symbol table to be used.
					s_order: The order to be sorted in.
Return value:		An int, -1 for a failure, or a 1 for a success.
*/
int st_sort(STD sym_table, char s_order)
{
	/*Local variables*/
	pBuffer temp_buffer;
	int stvr_index;
	int plex_index;
	int str_length;

	if (!sym_table.st_size)
		return ST_FAIL_1;

	temp_buffer = b_create(b_capacity(sym_table.plsBD), ST_BUFFER_INC_FACTOR, ST_BUFFER_TYPE);

	if (!temp_buffer)
		return ST_FAIL_1;

	switch (s_order)
	{
	case ST_SORT_ASCENDING:
		qsort((void*)sym_table.pstvr, sym_table.st_offset, sizeof(STVR), asc_compare);
		break;
	case ST_SORT_DESCENDING:
		qsort((void*)sym_table.pstvr, sym_table.st_offset, sizeof(STVR), desc_compare);
		break;
	default:
		return ST_FAIL_1;
	}
	
	for (stvr_index = 0; stvr_index < sym_table.st_offset; ++stvr_index)
	{
		str_length = strlen(sym_table.pstvr[stvr_index].plex);

		if (!sym_table.pstvr[stvr_index].plex)
			return ST_FAIL_1;

		/*Move the plex from one buffer to another*/
		for (plex_index = 0; plex_index < str_length; ++plex_index)
		{
			if (!b_addc(temp_buffer, sym_table.pstvr[stvr_index].plex[plex_index]))
				return ST_FAIL_1;
		}

		if (!b_addc(temp_buffer, '\0'))
			return ST_FAIL_1;

		/*Reassign the plex pointer from the old buffer to the new one*/
		if (stvr_index == 0)
		{
			sym_table.pstvr[stvr_index].plex = b_setmark(temp_buffer, 0);
			if (!sym_table.pstvr[stvr_index].plex)
				return ST_FAIL_1;
		}
		else
		{
			sym_table.pstvr[stvr_index].plex =
				sym_table.pstvr[stvr_index - 1].plex +
				strlen(sym_table.pstvr[stvr_index - 1].plex)
				+ 1;
		}
	}
	
	if (!st_setbuffer(temp_buffer))
		return ST_FAIL_1;

	return SORT_SUCCESS;
}

/*
Purpose:			A function which compares two STVRs and returns
						an int allowing for the sorting them in
						ascending order.
Author:				Justin Bertrand
History/Versions:	1.0, Nov. 18th 2015
Called functions:	strcmp()
Parameters:			pElem1: The first element to be compared.
					pElem2: The second element to be compared.
Return value:		An int, 0 if they are equal, >0 if the second
						element follows the first, and >0 if the
						first element follows the second.
*/
static int asc_compare(const void* pElem1, const void* pElem2)
{
	STVR* elem1 = (STVR*)pElem1;
	STVR* elem2 = (STVR*)pElem2;

	return strcmp(elem1->plex, elem2->plex);
}

/*
Purpose:			A function which compares two STVRs and returns
						an int allowing for the sorting them in
						descending order.
Author:				Justin Bertrand
History/Versions:	1.0, Nov. 18th 2015
Called functions:	strcmp()
Parameters:			pElem1: The first element to be compared.
					pElem2: The second element to be compared.
Return value:		An int, 0 if they are equal, <0 if the second
						element follows the first, and >0 if the
						first element follows the second.
*/
static int desc_compare(const void* pElem1, const void* pElem2)
{
	STVR* elem1 = (STVR*)pElem1;
	STVR* elem2 = (STVR*)pElem2;

	return -strcmp(elem1->plex, elem2->plex);
}

/*
Purpose:			A function which allows for the replacement 
						of the buffer of the symbol table. Must be
						used because the pass by value of the Struct
						causes the changes to be lost when exiting local
						scope.
Author:				Justin Bertrand
History/Versions:	1.0, Nov. 18th 2015
Called functions:	b_destroy()
Parameters:			buffer: The new buffer to be replaced.
Return value:		Null on a failure, and a pointer to a 
						buffer on success.
*/
static pBuffer st_setbuffer(pBuffer buffer)
{
	if (!buffer)
		return NULL;

	if (!sym_table.plsBD)
		return NULL;

	b_destroy(sym_table.plsBD);

	sym_table.plsBD = buffer;

	return sym_table.plsBD;
}

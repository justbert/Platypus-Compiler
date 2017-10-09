/* File name:	buffer.c
Compiler:		MS Visual Studio 2013, Update 5
Author:			Justin Bertrand, 040 786 592
Course:			CST 8152 - Compilers, Lab Section: 011
Assignment:		Assignment 1 - The Buffer
Date:			Sept. 24th, 2015
Professor:		Sv. Ranev
Purpose:		This is a buffer which takes in chars, but which could easily
					be modified to accept any type of information.
Warnings:		Please note that external libraries use non-ANSI single 
					line comments which cause warnings
Function list:
	b_create()
	b_addc()
	b_reset()
	b_destroy()
	b_isfull()
	b_size()
	b_capacity()
	b_setmark()
	b_mark()
	b_mode()
	b_inc_mode()
	b_load()
	b_isempty()
	b_eob()
	b_getc()
	b_print()
	b_pack()
	b_rflag()
	b_retract()
	b_retract_to_mark()
	b_getc_offset()
*/
#include "buffer.h"

/*#define DEBUG*/


/*
Purpose:			Used as a constructor method to initialize a buffer with
					the specified parameters
Author:				J. Bertrand
History/Versions:	V1.1, Sept. 17th, 2015
Called functions:	calloc()
					malloc()
					free()
Parameters:			init_capacity: short, must be larger than 0
					inc_factor: short, 0  for fixed mode, between 1 and 255
						for additive mode, and between 1 and 100 for
						multiplicative mode.
					o_mode: char, f for fixed mode, a for additive mode,
						and m for multiplicative mode.
Return value:		Buffer*: a pointer to a Buffer structure upon successful
						creation of a buffer
					NULL: returns a NULL pointer upon any failure.
Algorithm:			Allocate buffer memory
					
					If init_capacity is non-negative, allocate memory
					equal to init_capacity for the array
					
					assign init_capacity to buffer capacity
					
					IF init_capacity > 0 AND (o_mode = f OR inc_factor = 0)
					OR IF init_capacity > 0 AND o_mode = f AND inc_factor > 0
						buffer mode = fixed
						buffer inc_factor = 0

					ELSE IF o_mode = a AND inc_factor is between 1 and 255
						buffer mode = additive
						buffer inc_factor = inc_factor
					
					ELSE IF o_mode = m AND inc_factor is between 1 and 100
						buffer mode = multiplicative
						buffer inc_factor = inc_factor

					ELSE
						free allocated memory for the array and the buffer
						return NULL

					return buffer
*/
Buffer * b_create(short init_capacity, char inc_factor, char o_mode)
{
	/*Local variables*/
	pBuffer pbuffer; /* A pointer to the Buffer struct being created */
	char *temp_character_buffer; /* A pointer used in the allocation of an array of chars */

	/* Allocate the memory for Buffer and check for a NULL pointer*/
	pbuffer = (pBuffer)calloc(BUFFER_AMOUNT, sizeof(Buffer));
	/* Check for a null pointer returned */
	if (!pbuffer)
	{
		return NULL;
	}

	/* Create the dynamic memory for the character buffer 
		if the init_capacity is a valid argument */
	if (init_capacity < 0)
	{
		return NULL;
	}
	
	temp_character_buffer = (char*)malloc(sizeof(char)*init_capacity);

	/* Check to see if the returned pointer is NULL */
	if (!temp_character_buffer)
	{
		free(pbuffer);
		pbuffer = NULL;
		return NULL;
	}
	else
	{
		pbuffer->cb_head = temp_character_buffer;
	}

	/* Assigns the value of init capacity */
	pbuffer->capacity = init_capacity;


	/* If o_mode is f or inc_factor is 0 -> mode and inc_factor set to 0 */
	if (init_capacity && (o_mode == 'f' || !inc_factor))
	{
		pbuffer->mode = FIXED_MODE;
		pbuffer->inc_factor = 0;
	}

	/* If o_mode is f and inc_factor is not 0 -> mode and inc_factor set to 0 */
	else if (init_capacity && o_mode == 'f' && inc_factor)
	{
		pbuffer->mode = FIXED_MODE;
		pbuffer->inc_factor = 0;
	}

	/* If o_mode is a AND inc_factor is > 0 && <= 255 -> mode set to 1, inc_factor set to inc_factor */
	else if (o_mode == 'a' && ((unsigned char) inc_factor) > 0 && ((unsigned char) inc_factor) <= MAX_INC_ADDITIVE)
	{
		pbuffer->mode = ADDITIVE_MODE;
		pbuffer->inc_factor = inc_factor;
	}

	/* If o_mode is m AND inc_factor > 0 AND inc_factor <= 100 -> mode set to -1, inc_factor set to inc_factor*/
	else if (o_mode == 'm' && inc_factor > 0 && inc_factor <= MAX_INC_MULTIPLICATIVE)
	{
		pbuffer->mode = MULTIPLICATIVE_MODE;
		pbuffer->inc_factor = inc_factor;
	}

	/* If the parameters do not correspond to a proper combination, free memory, return NULL */
	else
	{
		free(pbuffer->cb_head);
		pbuffer->cb_head = NULL;
		free(pbuffer);
		pbuffer = NULL;
		return NULL;
	}

	return pbuffer;
}

/*
Purpose:			Function which adds a symbol to the Buffer.
Author:				J. Bertrand
History/Versions:	V1.0, Sept. 17th, 2015
Called functions:	realloc()
Parameters:			pBD: pBuffer const, a pointer to a Buffer structure
					symbol: a symbol to be added to the Buffer
Return value:		pBuffer: a pointer to a Buffer structure upon successful
						addition of the symbol.
					NULL: returns a NULL pointer upon any failure.
Algorithm:			IF there is room in the buffer
						add symbol to buffer
						return the buffer
					ELSE
						IF mode is fixed
							return fail
						ELSE IF mode is additive
							calculate new capacity
							reallocate the memory for the expanded buffer
							add symbol to buffer
							update all the buffer properties
							return buffer
						ELSE IF mode is multiplicative
							calculate new capacity
							reallocate the memory for the expanded buffer
							add symbol to buffer
							update all the buffer properties
							return buffer
						ELSE
							return fail
*/ 
pBuffer b_addc(pBuffer const pBD, char symbol) {

	/* Local variable */
	char* temp; /* a placeholder for a pointer to a new char array*/
	short new_capacity = 0; /* the new calculated size, if expansion is necessary */
	float new_increment = 0; /* the new increment for the calculation of the multiplicative type */

	/* Verify that passed pointer is not NULL */
	if (!pBD) 
	{
#ifdef DEBUG
		err_printf("b_addc pointer parameter was NULL\n");
#endif
		return NULL;
	}

	/* Reset r_flag */
	pBD->r_flag = RESET_R_FLAG;

	/* Tries to add symbol to the current buffer
		addc_offset cannot be larger than capacity.
		Else , buffer must widen.*/
	if (pBD->addc_offset < pBD->capacity)
	{
		pBD->cb_head[pBD->addc_offset] = symbol;
		++(pBD->addc_offset);
		return pBD;
	}
	/* Buffer is Full. Try to resize.*/
	else
	{
		/* Buffer is fixed. Cannot expand. Return Null.*/
		if (pBD->mode == FIXED_MODE)
		{
			return NULL;
		}
		/* Buffer is additive. Expands by adding inc_factor to current capacity*/
		else if (pBD->mode == ADDITIVE_MODE)
		{
			new_capacity = pBD->capacity + (unsigned char)pBD->inc_factor;
			
			/* Verify for overflow */
			if (new_capacity < pBD->capacity || new_capacity < 0) 
			{
#ifdef DEBUG
				err_printf("Additive overflow: %i", new_capacity);
#endif
				/* short overflow. Capacity is exceeding short size*/
				return NULL;
			}

			/* Verify if no more characters can be added */
			if (pBD->addc_offset == new_capacity)
			{
				return NULL;
			}

			/* Reallocate the char array with the new capacity */
			temp = realloc(pBD->cb_head, new_capacity*sizeof(char)); 
			
			/* Check for null pointer from assigned memory */
			if (!temp)
			{
				return NULL;
			}
			
			if (pBD->cb_head != temp) 
			{
				pBD->r_flag = SET_R_FLAG;
				pBD->cb_head = temp;
			}

			pBD->cb_head[pBD->addc_offset] = symbol;
			++pBD->addc_offset;
			pBD->capacity = new_capacity;
			return pBD;
		}
		/* Buffer is multiplicative. */
		if (pBD->mode == MULTIPLICATIVE_MODE)
		{
			/* Calculate the new capacity */
			new_increment = (float)(((SHRT_MAX - pBD->capacity) * (unsigned char)pBD->inc_factor / 100)); 
			
			/* If the new capacity was calculated as 0, increment it by one */
			if ((short)new_increment == 0) {
#ifdef DEBUG
				err_printf("The new_capacity is: %i", new_capacity);
#endif
				new_capacity = SHRT_MAX;
			}
			else 
			{
				new_capacity = (short)new_increment + pBD->capacity;
			}

#ifdef DEBUG
			err_printf("The new_capacity is: %i", new_capacity);
#endif
			/* Verify for overflow */
			if (new_capacity < pBD->capacity || new_capacity < 0)
			{
				/* short overflow. new_capacity is exceeding short size*/
				new_capacity = SHRT_MAX;
			}

			/* If buffer is full, do not continue */
			if (pBD->addc_offset == new_capacity)
			{
				return NULL;
			}

			/* Reallocate symbol array with new capacity as size */
			temp = realloc(pBD->cb_head, new_capacity*sizeof(char));

			/* Check for NULL pointer */
			if (!temp)
			{
				return NULL;
			}
			
			/* Assign new values to Buffer */
			if (pBD->cb_head != temp)
			{
				pBD->r_flag = SET_R_FLAG;
				pBD->cb_head = temp;
			}

			pBD->cb_head[pBD->addc_offset] = symbol;
			++pBD->addc_offset;
			pBD->capacity = new_capacity;
			return pBD;
		}
		/* Buffer mode has been corrupted */
#ifdef DEBUG
		err_printf("Buffer has been corrupted.\n");
#endif
		return NULL;
	}
}

/*
Purpose:			Function which resets the Buffer, allowing it to be reused.
Author:				J. Bertrand
History/Versions:	V1.0, Sept. 17th, 2015
Called functions:	None
Parameters:			pBD: pBuffer const, a pointer to a Buffer structure
Return value:		int: 0 for success, 1 for failure
Algorithm:			If pointer is valid
						Set addc_offset, getc_offset, mark_offset,
						r_flag, and eob to 0
						return Success
					ELSE 
						return failure
*/
int b_reset(Buffer * const pBD)
{
	/* Pointer is not null*/
	if (pBD)
	{
		/* Reset values to defaults, allowing 
			the buffer to be reused */
		pBD->addc_offset = 0;
		pBD->getc_offset = 0;
		pBD->mark_offset = 0;
		pBD->r_flag = RESET_R_FLAG;
		pBD->eob = RESET_EOB_FLAG;
		return R_SUCCESS;
	}

	return R_FAIL_1;
}

/*
Purpose:			Function to release allocated memory.
Author:				J. Bertrand
History/Versions:	V1.0, Sept. 17th, 2015
Called functions:	free()
Parameters:			pBD: pBuffer const, a pointer to a Buffer structure
Return value:		None.
Algorithm:			If pointer is valid
						free the memory for the symbol array and
							set pointer to NULL
						free the memory for the Buffer structure
*/
void b_destroy(Buffer * const pBD)
{
	/* If a valid pointer, free the memory*/
	if (pBD)
	{ 
		free(pBD->cb_head);
		pBD->cb_head = NULL;
		free(pBD);
	}
}

/*
Purpose:			Function which returns an int representing if the 
						buffer is full.
Author:				J. Bertrand
History/Versions:	V1.0, Sept. 17th, 2015
Called functions:	None
Parameters:			pBD: pBuffer const, a pointer to a Buffer structure
Return value:		int: 0 if the buffer is not full, 1 if the buffer is full,
						and -1 for a failure.
Algorithm:			IF pointer to buffer is NULL
						return fail value
					IF addc is smaller than capacity
						return buffer not full
					ELSE IF addc is equal to capacity
						return buffer is full
					ELSE
						return fail value
*/
#ifndef B_FULL
int b_isfull(Buffer * const pBD)
{
	/* Check to make sure pointer is not NULL*/
	if (!pBD)
	{
		return R_FAIL_1;
	}
	
	/* If buffer isn't full, return 0 */
	if (sizeof(char)*pBD->addc_offset < (unsigned)pBD->capacity)
	{
		return BUFFER_NOT_FULL;
	}
	/* IF buffer is full return 1*/
	else if (sizeof(char)*pBD->addc_offset == (unsigned)pBD->capacity)
	{
		return BUFFER_IS_FULL;
	}

	/* Upon failure, return -1 */
	return R_FAIL_1;
}
#endif
#ifdef B_FULL
#define b_isfull(pBD) (pBD)?():(return R_FAIL_1;)
#endif

/*
Purpose:			Function which returns a short representing the size of the buffer.
Author:				J. Bertrand
History/Versions:	V1.0, Sept. 17th, 2015
Called functions:	None
Parameters:			pBD: pBuffer const, a pointer to a Buffer structure
Return value:		short: representing the size of the buffer.
Algorithm:			IF pointer is not NULL
						return size
					ELSE
						return -1
*/
short b_size(Buffer * const pBD)
{
	/* If pointer is not NULL, return size*/
	if (pBD) 
	{
		return pBD->addc_offset;
	} 
	
	/* IF NULL return -1 */
	return R_FAIL_1;
}

/*
Purpose:			Function which returns a short which represents
						the capacity of the symbol array in bytes.
Author:				J. Bertrand
History/Versions:	V1.0, Sept. 17th, 2015
Called functions:	None
Parameters:			pBD: pBuffer const, a pointer to a Buffer structure
Return value:		short: represents the size of the buffer in bytes,
							or -1 representing a failure.
Algorithm:			IF pointer is not NULL
						return capacity
					ElSE
						return fail
*/
short b_capacity(Buffer * const pBD)
{
	/* Testing to make sure the pointer is not NULL */
	if (pBD)
	{
		return pBD->capacity;
	}
	
	/* Upon failure, return fail (-1) */
	return R_FAIL_1;
}

/*
Purpose:			Function which sets the mark of the buffer.
Author:				J. Bertrand
History/Versions:	V1.0, Sept. 17th, 2015
Called functions:	None
Parameters:			pBD: pBuffer const, a pointer to a Buffer structure
					mark: short, a non-negative number to mark a location
							in the buffer.
Return value:		char*: a pointer to the location of the mark in the buffer
					NULL: return a NULL pointer on failure
Algorithm:			IF pointer AND mark is non-negative AND within the current buffer
						mark_offset <- mark
						return pointer to location of mark in the buffer 
					ElSE
						return NULL pointer
*/
char * b_setmark(Buffer * const pBD, short mark)
{
	/* Make sure the pointer is not NULL, and the mark
		is non-negative, and within the buffer */
	if (pBD && mark <= pBD->addc_offset && mark >= 0)
	{
		pBD->mark_offset = mark;
		return pBD->cb_head+pBD->mark_offset;
	}
	
	return NULL;
}

/*
Purpose:			Function which returns the mark location of the buffer.
Author:				J. Bertrand
History/Versions:	V1.0, Sept. 17th, 2015
Called functions:	None
Parameters:			pBD: pBuffer const, a pointer to a Buffer structure
Return value:		short: a non-negative number representing the location
							of the mark in the buffer; 
							or -1 representing a failure.
Algorithm:			IF pointer is not NULL
						return mark_offset
					ElSE
						return fail
*/
short b_mark(Buffer * const pBD)
{
	/* Verify that the parameter is valid */
	if (pBD) {
		return pBD->mark_offset;
	}
	
	return R_FAIL_1;
}

/*
Purpose:			Function which returns a number representing the
						mode of the buffer.
Author:				J. Bertrand
History/Versions:	V1.0, Sept. 17th, 2015
Called functions:	None
Parameters:			pBD: pBuffer const, a pointer to a Buffer structure
Return value:		short: -1 for multiplicative mode, 0 for fixed, 1 for
							additive, and -2 for a fail.
Algorithm:			IF pointer is not NULL
						return mode
					ElSE
						return fail
*/
int b_mode(Buffer * const pBD)
{
	/* Verify that the parameter is valid */
	if (pBD)
	{
		return pBD->mode;
	}
	
	return R_FAIL_2;
}

/*
Purpose:			Function which returns a number representing the
						amount the buffer will be incremented during an
						expansion.
Author:				J. Bertrand
History/Versions:	V1.0, Sept. 17th, 2015
Called functions:	None
Parameters:			pBD: pBuffer const, a pointer to a Buffer structure
Return value:		short: a non-negative number up to 100 for multiplicative
							mode; a non-negative number up to 255 for additive
							mode; and 256 for a failure.
Algorithm:			IF pointer is not NULL
						return increment factor
					ElSE
						return fail
*/
size_t b_inc_factor(Buffer * const pBD)
{
	/* Verify the parameter for a valid pointer */
	if (pBD)
	{
		/* return a cast inc_factor */
		return (size_t)((unsigned char)pBD->inc_factor);
	}

	return R_FAIL_256;
}

/*
Purpose:			Function which reads a file into the buffer.
Author:				J. Bertrand
History/Versions:	V1.0, Sept. 17th, 2015
Called functions:	fgetc())
					b_addc()
					feof() - macro
Parameters:			fi: FILE * const, a pointer to the file being read in
					pBD: pBuffer const, a pointer to a Buffer structure
Return value:		int: a number representing the number of symbols read in;
							-1 for a failure.
Algorithm:			UNTIL end of file is reached
						extract symbol from file
						add symbol to buffer
						increment symbol counter

					return symbol counter
*/
int b_load(FILE * const fi, Buffer * const pBD)
{
	/* Local variables */
	int temp_c = 0; /* a temp holder for the value which is loaded from the file */
	int number_symbols = 0; /* a counter of the total number of symbols loaded */

	/* Verifies that the parameters are not NULL*/
	if (fi == NULL || pBD == NULL)
	{
		return R_FAIL_1;
	}

	temp_c = fgetc(fi);

	while (!feof(fi)) 
	{
		if (!b_addc(pBD, (char)temp_c))
		{
			return LOAD_FAIL;
		}

		++number_symbols;
		temp_c = fgetc(fi);
	}

	return number_symbols;
}

/*
Purpose:			Function which returns a flag representing if the
						buffer is empty.
Author:				J. Bertrand
History/Versions:	V1.0, Sept. 17th, 2015
Called functions:	None
Parameters:			pBD: pBuffer const, a pointer to a Buffer structure
Return value:		int: 0 if the buffer is not empty; 1 if it is empty;
						-1 for a failure.
Algorithm:			IF pointer is NULL
						return failure
					IF 
						return fail
*/
int b_isempty(Buffer * const pBD)
{
	/* Check for a NULL pointer */
	if (!pBD)
	{
		return R_FAIL_1;
	}

	/* */
	if (pBD->addc_offset > 0)
	{
		return NOT_EMPTY;
	}
	else if (pBD->addc_offset == 0)
	{
		return IS_EMPTY;
	}
	
	return R_FAIL_1;
}

/*
Purpose:			Function which returns a flag representing if the
						end of the buffer has been reached.
Author:				J. Bertrand
History/Versions:	V1.0, Sept. 17th, 2015
Called functions:	None
Parameters:			pBD: pBuffer const, a pointer to a Buffer structure
Return value:		int: 0 if the end is not reached; 1 if it has reached the end;
						-1 for a failure.
Algorithm:			IF pointer is not NULL
						return end of buffer flag
					IF
						return fail
*/
int b_eob(Buffer * const pBD)
{
	/* Check for pointer validity */
	if (pBD)
	{
		return pBD->eob;
	}

	return R_FAIL_1;
}

/*
Purpose:			Function which fetches a symbol from the buffer.
Author:				J. Bertrand
History/Versions:	V1.0, Sept. 17th, 2015
Called functions:	None
Parameters:			pBD: pBuffer const, a pointer to a Buffer structure
Return value:		char: a byte of the symbol being fetched.
Algorithm:			IF end of buffer has not been reached
						reset end of buffer flag
						return symbol
						increment getc_offset
					ELSE IF end of buffer has been reached
						set end of buffer flag
						return fail 1
					ELSE
						return fail 2
*/
char b_getc(Buffer * const pBD)
{
	/* Check for NULL pointer */
	if (!pBD)
	{
		return R_FAIL_2;
	}

	if (pBD->getc_offset < pBD->addc_offset && pBD->getc_offset >= 0)
	{
		pBD->eob = RESET_EOB_FLAG;
		return pBD->cb_head[pBD->getc_offset++];
	}
	else if (pBD->getc_offset == pBD->addc_offset)
	{
		pBD->eob = SET_EOB_FLAG;
		return R_FAIL_1;
	}
	
	return R_FAIL_2;
}

/*
Purpose:			Function which prints all the symbols in the buffer.
Author:				J. Bertrand
History/Versions:	V1.0, Sept. 17th, 2015
Called functions:	b_isempty()
					b_getc()
					printf()
					b_eob()

Parameters:			pBD: pBuffer const, a pointer to a Buffer structure
Return value:		int: 0 if the end is not reached; 1 if it has reached the end;
						-1 for a failure.
Algorithm:			IF pointer is not NULL
						return end of buffer flag
					IF
						return fail
*/
int b_print(Buffer * const pBD)
{
	/* Local variables */
	int count = 0; /* The number of characters printed */
	char getc_temp = 0; /* A temp for the character that is fetched */
	
	/* Check paramater for NULL */
	if (!pBD)
	{
		return R_FAIL_1;
	}

	/* Check for the emptyness of the buffer before trying to pull characters */
	if (b_isempty(pBD))
	{
		printf("The buffer is empty.\n");
		return count;
	}
	
	/* Set getc to zero, to start printing from the beginning */
	pBD->getc_offset = 0;

	/* get the first symbol */
	getc_temp = b_getc(pBD);

	while(!b_eob(pBD))
	{
		/* print the symbol to the output stream */
		printf("%c", getc_temp);
		++count;
		/* get the next symbol */
		getc_temp = b_getc(pBD);
	}

	/* add end of line character to the output stream */
	printf("\n");
	pBD->getc_offset = 0;
	return count;
	
}

/*
Purpose:			Function which resizes the buffer to the current amount of
						symbols stored, and adds one storage location.
Author:				J. Bertrand
History/Versions:	V1.0, Sept. 17th, 2015
Called functions:	realloc()
Parameters:			pBD: pBuffer const, a pointer to a Buffer structure
Return value:		Buffer*: a pointer to the buffer which was packed.
Algorithm:			Reallocate array memory to capacity + 1
					set buffer properties to corrected values
*/
Buffer *b_pack(Buffer *const pBD)
{
	/* Local variables*/
	char* temp_array;	/* A temporary pointer for the new symbol array */
	
	/* Validity of the parameter */
	if (!pBD)
	{
		return NULL;
	}

	/* Check for possible overflow */
	if (pBD->addc_offset == SHRT_MAX) {
		return NULL;
	}
	
	/* Assign reallocated pointer to the temp variable */
	temp_array = (char*)realloc(pBD->cb_head, sizeof(char)*(pBD->addc_offset + 1));
		
	/* Test for NULL pointer from the reallocated memory */
	if (!temp_array) {
		return NULL;
	}

	/* Set the variables to the correct */
	if (pBD->cb_head != temp_array)
	{
		pBD->cb_head = temp_array;
		pBD->r_flag = SET_R_FLAG;
	}
	
	pBD->capacity = pBD->addc_offset + sizeof(char);
	pBD->eob = RESET_EOB_FLAG;
	return pBD;
}

/*
Purpose:			Function which returns a int representing the state of the
						reallocation flag.
Author:				J. Bertrand
History/Versions:	V1.0, Sept. 17th, 2015
Called functions:	None.
Parameters:			pBD: pBuffer const, a pointer to a Buffer structure
Return value:		char: 0 if it has not been reallocated; 1 if it has been 
						reallocated; -1 for a failure.
Algorithm:			IF pointer is not NULL
						return reallocation flag
					else
						return fail
*/
char b_rflag(Buffer * const pBD)
{
	/* Verify that parameter is not NULL */
	if (pBD)
	{
		return pBD->r_flag;
	}
	
	return R_FAIL_1;	
}

/*
Purpose:			Function which reduces the location of the getc_offset
						by 1.
Author:				J. Bertrand
History/Versions:	V1.0, Sept. 17th, 2015
Called functions:	None.
Parameters:			pBD: pBuffer const, a pointer to a Buffer structure
Return value:		short: a non-negative number representing the getc_offset
						which has been reduced by 1; -1 for a failure.
Algorithm:			IF pointer is not NULL
						reduce getc_offset by one
						return getc_offset
					ELSE
						return fail
*/
short b_retract(Buffer * const pBD)
{
	/* Verify for NULL pointer */
	if (pBD && pBD->getc_offset)
	{
		return --pBD->getc_offset;
	}

	return R_FAIL_1;	
}

/*
Purpose:			Function which sets the location of the getc_offset
						to the location of the mark_offset.
Author:				J. Bertrand
History/Versions:	V1.0, Sept. 17th, 2015
Called functions:	None.
Parameters:			pBD: pBuffer const, a pointer to a Buffer structure
Return value:		short: a non-negative number representing the getc_offset
						which has been set to the location of the mark; -1 for a failure.
Algorithm:			IF pointer is not NULL
						set getc_offset to mark_offset
						return getc_offset
					IF
						return fail
*/
short b_retract_to_mark(Buffer * const pBD)
{
	/* Verify for NULL pointer */
	if (!pBD)
	{
		return R_FAIL_1;
	}

	pBD->getc_offset = pBD->mark_offset;
	return pBD->getc_offset;
}

/*
Purpose:			Function which returns getc_offset.
Author:				J. Bertrand
History/Versions:	V1.0, Sept. 17th, 2015
Called functions:	None.
Parameters:			pBD: pBuffer const, a pointer to a Buffer structure
Return value:		short: a non-negative number representing the getc_offset;
							-1 for a failure.
Algorithm:			IF pointer is not NULL
						return getc_offset
					IF
						return fail
*/
short b_getc_offset(Buffer * const pBD)
{
	if (pBD)
	{
		return pBD->getc_offset;
	}
	
   return R_FAIL_1;
}

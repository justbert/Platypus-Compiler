/* File Name: buffer.h
 * Version: 1.15.2
 * Author: S^R, modified by Justin Bertrand
 * Date: 8 September 2015
 * Preprocessor directives, type declarations and prototypes necessary for buffer implementation 
 * as required for CST8152, Assignment #1, Fall 2015.
 */
#ifndef BUFFER_H_
#define BUFFER_H_

#pragma warning(1:4001) /*to enforce C89 type comments  - to make //comments an warning */

/*#pragma warning(error:4001)*//* to enforce C89 comments - to make // comments an error */

/* standard header files */
#include <stdio.h>  /* standard input/output */
#include <malloc.h> /* for dynamic memory allocation*/
#include <limits.h> /* implementation-defined data type ranges and limits */

/* constant definitions */
/* You may add your own constant definitions here */
#define R_FAIL_1 -1         /* fail return value */
#define R_FAIL_2 -2         /* fail return value */
#define R_FAIL_256 256		/* fail return value */
#define R_SUCCESS 0			/* sucess return value */
#define LOAD_FAIL -2		/* load fail error */
#define SET_R_FLAG 1		/* realloc flag set value */
#define RESET_R_FLAG 0		/* resets the r flag*/
#define FIXED_MODE 0		/* value for fixed mode  */
#define ADDITIVE_MODE 1		/* value for additive incrementing mode*/
#define MULTIPLICATIVE_MODE -1	/* value for multiplicative incrementing mode */
#define BUFFER_AMOUNT 1		/* number of buffers to allocate on buffer creation */
#define BUFFER_IS_FULL 1	/* indicate that the buffer is full */
#define BUFFER_NOT_FULL	0	/* indicates that the buffer is not full */
#define IS_EMPTY 1			/* indicates when the buffer is empty */
#define NOT_EMPTY 0			/* indicates when the buffer is not empty */
#define SET_EOB_FLAG 1		/* EOB flag set value */
#define RESET_EOB_FLAG 0	/* EOB flag reset value */
#define MAX_INC_MULTIPLICATIVE 100 /* The max inc_factor value for multiplicative mode */
#define MAX_INC_ADDITIVE 255	/* The max inc_factor for additive mode */

/* user data type declarations */
typedef struct BufferDescriptor {
    char *cb_head;   /* pointer to the beginning of character array (character buffer) */
    short capacity;    /* current dynamic memory size (in bytes) allocated to character buffer */
    short addc_offset;  /* the offset (in chars) to the add-character location */
    short getc_offset;  /* the offset (in chars) to the get-character location */
    short mark_offset; /* the offset (in chars) to the mark location */
    char  inc_factor; /* character array increment factor */
    char  r_flag;     /* character array reallocation flag */
    char  mode;       /* operational mode indicator*/
    int   eob;       /* end-of-buffer reached flag */
} Buffer, *pBuffer; /*typedef Buffer *pBuffer;*/

/* function declarations */
Buffer* b_create(short, char, char);

pBuffer b_addc(pBuffer const, char);

int b_reset(Buffer* const);

void b_destroy(Buffer* const);

int b_isfull(Buffer* const);

short b_size(Buffer* const);

short b_capacity(Buffer* const);

char* b_setmark(Buffer* const, short);

short b_mark(Buffer* const);

int b_mode(Buffer* const);

size_t b_inc_factor(Buffer* const);

int b_load(FILE* const, Buffer* const);

int b_isempty(Buffer* const);

int b_eob(Buffer* const);

char b_getc(Buffer* const);

int b_print(Buffer* const);

Buffer* b_pack(Buffer* const);

char b_rflag(Buffer* const);

short b_retract(Buffer* const);

short b_retract_to_mark(Buffer* const);

short b_getc_offset(Buffer* const);
#endif

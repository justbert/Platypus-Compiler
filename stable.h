/*	Filename: stable.h
 *	PURPOSE: A symbol table which stores variable identifiers for a	
 *		program
 *  SCANNER.H: Header for functions implementing a Symbol Table
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
#ifndef STABLE_H_
#define STABLE_H_

/*Include Statements*/
#include "buffer.h"

/*Hex masks used for flag manipulation*/
#define STATUS_DEFAULT_MASK 0xFFF8		/*The default mask for the Status*/
#define STATUS_UPDATE_FLAG_MASK 0x0001  /*The mask to acces the update flag*/
#define STATUS_STRING_MASK 0x0006		/*The mask to set the status of a String VID*/
#define STATUS_FLOAT_MASK 0x0002		/*Mask to set the status of a Float VID*/
#define STATUS_INTEGER_MASK 0x0004		/*Mask to set the status of an Integer VID*/
#define STATUS_TYPE_UPDATE_MASK 0x0007	/*Mask used when a status has been updated*/
#define STATUS_TYPE_FLAGS_MASK 0x0006	/*Mask used when identifying the type of status*/

/*Char values representing the type of a status field*/
#define ST_CHAR_TYPE_INT 'I'		/*Represents an integer*/
#define ST_CHAR_TYPE_STRING 'S'		/*Represents a String*/
#define ST_CHAR_TYPE_FLOAT 'F'		/*Represents a Float*/

/*Default values for different IVALUE types*/
#define IVALUE_STRING -1 /*Value for a string*/
#define IVALUE_NUMBER 0 /*Value for a number*/
#define IVALUE_FLOAT 0.0f /*Value for a float*/

/*Default name for the output file of the stored STD*/
#define ST_STORE_FILENAME "$stable.ste"

/* Values for the amount of spaces in the
line number section of the st_store function */
#define ST_PRINT_LINE_SPACES 11

/*Fail return values*/
#define ST_FAIL_1 -1 /* Fail return value */

/*The configuration values for the STDs buffer*/
#define ST_BUFFER_INIT_CAP 20
#define ST_BUFFER_INC_FACTOR 20
#define ST_BUFFER_TYPE 'a'

/*Values for the Sort*/
#define ST_SORT_ASCENDING 'A'
#define ST_SORT_DESCENDING 'D'
#define SORT_SUCCESS 1

typedef union InitialValue {
	int int_val; /* integer variable initial value */
	float fpl_val; /* floating-point variable initial value */
	int str_offset; /* string variable initial value (offset) */
} InitialValue;

typedef struct SymbolTableVidRecord {
	unsigned short status_field; /* variable record status field*/
	char * plex;  /* pointer to lexeme (VID name) in CA */
	int o_line;  /* line of first occurrence */
	InitialValue i_value; /* variable initial value */
	size_t reserved;  /*reserved for future use*/
} STVR;

typedef struct SymbolTableDescriptor {
	STVR *pstvr;  /* pointer to array of STVR */
	int st_size;  /* size in number of STVR elements */
	int st_offset; /*offset in number of STVR elements */
	Buffer *plsBD; /* pointer to the lexeme storage buffer descriptor */
} STD;

/*Function prototypes*/
STD st_create(int);
int st_install(STD, char*, char, int);
int st_lookup(STD, char*);
int st_update_type(STD, int, char);
int st_update_value(STD, int, InitialValue);
char st_get_type(STD, int);
void st_destroy(STD);
int st_print(STD);
int st_store(STD);
int st_sort(STD, char);
#endif

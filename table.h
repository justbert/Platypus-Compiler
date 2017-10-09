/* Filename: table.h
 * Transition Table and function declarations necessary for the scanner implementation  
 * as required for CST8152 - Assignment #2.
 *	Compiler: MS Visual Studio 2013, Update 5
 *	Course :	CST 8152 - Compiler, Lab Section: 011
 *	Assignment:  Assignment 2 - The Scanner
 *  Provided by: Svillen Ranev
 *	Modified by: Justin Bertrand, 040 786 592
 *  Version: 1.15.02
 *  Date: 27 October 2015
 */

#ifndef  TABLE_H_
#define  TABLE_H_ 

#ifndef NULL
#include <_null.h> /* NULL pointer constant is defined there */
#endif

#define ES  12 /* Error state */
#define ESWR 13 /* Error state with retraction */
#define IS -1    /* Inavalid state */

#define LETTER_COLUMN 0 /* The column in the table for letters*/
#define NzO_DIGITS_COLUMN 2 /*Column in the table for Non Zero Octal Digits*/
#define ZERO_COLUMN 1	/*Column for 0 in the table*/
#define EIGHT_NINE_COLUMN 3 /*Column for 8 and 9 in the table*/
#define DOT_COLUMN 4 /*Column for the dot in the table*/
#define STRING_VID_DELIMETER_COLUMN 5 /*Column for the string variable identifier delimeter column*/
#define OTHER_COLUMN 6 /*Column representing other input in the table*/

/* State transition table definition */

#define TABLE_COLUMNS 7
/*transition table - type of states defined in separate table */
int  st_table[][TABLE_COLUMNS] = {
	/* State 0 */{ 1, 6, 4, 4, IS, IS, IS }, /* last ES may be IS */
	/* State 1 */{ 1, 1, 1, 1, 2, 3, 2 }, 
	/* State 2 */{ IS, IS, IS, IS, IS, IS, IS },
	/* State 3 */{ IS, IS, IS, IS, IS, IS, IS },
	/* State 4 */{ ES, 4, 4, 4, 7, 5, 5 },
	/* State 5 */{ IS, IS, IS, IS, IS, IS, IS },
	/* State 6 */{ ES, ES, 9, ES, 7, ES, 5 },
	/* State 7 */{ 8, 7, 7, 7, 8, 8, 8 },
	/* State 8 */{ IS, IS, IS, IS, IS, IS, IS },
	/* State 9 */{ ES, 9, 9, ES, ES, 10, 10 },
	/* State 10*/{ IS, IS, IS, IS, IS, IS, IS },
	/* State 11*/{ IS, IS, IS, IS, IS, IS, IS },
	/* State 12*/{ IS, IS, IS, IS, IS, IS, IS },
	/* State 13*/{ IS, IS, IS, IS, IS, IS, IS }
};
 
/* Accepting state table definition */
#define ASWR 256 /* accepting state with retract */
#define ASNR 512 /* accepting state with no retract */
#define NOAS 1024 /* not accepting state */

int as_table[ ] = { NOAS,
					NOAS, 
					ASWR,
					ASNR,
					NOAS,
					ASWR,
					NOAS,
					NOAS,
					ASWR,
					NOAS,
					ASWR,
					ASNR,
					ASNR,
					ASWR
};

/* Accepting action function declarations */

Token aa_func02(char *lexeme); 
Token aa_func03(char *lexeme);
Token aa_func05(char *lexeme);
Token aa_func08(char *lexeme);
Token aa_func10(char *lexeme);
Token aa_func12(char *lexeme);

/* defining a new type: pointer to function (of one char * argument) 
   returning Token
*/  

typedef Token (*PTR_AAF)(char *lexeme);

PTR_AAF aa_table[ ] = {	NULL,
						NULL,
						aa_func02,
						aa_func03,
						NULL,
						aa_func05,
						NULL,
						NULL,
						aa_func08,
						NULL,
						aa_func10,
						NULL,
						aa_func12
};

#define KWT_SIZE  8

char * kw_table []= {
	"ELSE", /*0*/
	"IF", /*1*/
	"INPUT", /*2*/
	"OUTPUT", /*3*/
	"PLATYPUS", /*4*/
	"REPEAT", /*5*/
	"THEN", /*6*/
	"USING" /*7*/
	};

#endif
                     
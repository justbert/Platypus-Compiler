/*	Filename: scanner.c
 *	PURPOSE:
 *  SCANNER.C: Functions implementing a Lexical Analyzer (Scanner)
 *  as required for CST8152, Assignment #2
 *  scanner_init() must be called before using the scanner.
 *	Compiler: MS Visual Studio 2013, Update 5
 *	Course :	CST 8152 - Compiler, Lab Section: 011
 *	Assignment:  Assignment 2 - The Scanner
 *  Provided by: Svillen Ranev
 *	Modified by: Justin Bertrand, 040 786 592
 *  Version: 1.15.02
 *  Date: 27 October 2015
 *	Warnings:		Please note that external libraries use non-ANSI single 
					line comments which cause warnings
 */

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>   /* standard input / output */
#include <ctype.h>   /* conversion functions */
#include <stdlib.h>  /* standard library functions and constants */
#include <string.h>  /* string functions */
#include <limits.h>  /* integer types constants */

/*#define NDEBUG        to suppress assert() call */
#include <assert.h>  /* assert() prototype */

/* project header files */
#include "token.h"
#include "table.h"
#include "stable.h"

#define DEBUG  /* for conditional processing */
#undef  DEBUG

/* Global objects - variables */
/* This buffer is used as a repository for string literals.
   It is defined in platy_st.c */
extern Buffer * str_LTBL; /*String literal table */
int line; /* current line number of the source code */
extern int scerrnum;     /* defined in platy_st.c - run-time error number */
extern STD sym_table;

/* Local(file) global objects - variables */
static Buffer *lex_buf;/*pointer to temporary lexeme buffer*/

/* No other global variable declarations/definitiond are allowed */

/* scanner.c static(local) function  prototypes */ 
static int char_class(char c); /* character class function */
static int get_next_state(int, char, int *); /* state machine function */
static int iskeyword(char * kw_lexeme); /*keywords lookup functuion */
static long atool(char * lexeme); /* converts octal string to decimal value */
static int power(int, int); /* A function that calculates the power of x to y */

/*
Purpose:			Function which initializes the scanner
Author:				S. Ranev
History/Versions:	N/A
Called functions:	None
Parameters:			sc_buf: A pointer to a buffer structure
Return value:		int: 0 for success, 1 for failure
*/
int scanner_init(Buffer * sc_buf) 
{
  	if(b_isempty(sc_buf)) return EXIT_FAILURE;/*1*/
	/* in case the buffer has been read previously  */
	b_setmark(sc_buf, 0);
	b_retract_to_mark(sc_buf);
	b_reset(str_LTBL);
	line = 1;
	return EXIT_SUCCESS;/*0*/
/*   scerrnum = 0;  *//*no need - global ANSI C */
}

/*
Purpose:			Function which analyzes symbols in a buffer 
						and produces a token.
Author:				S. Ranev and Justin Bertrand
History/Versions:	N/A
Called functions:	aa_funXX(char *lexeme)
					get_next_state()
					char_class()
Parameters:			sc_buf: A pointer to a buffer structure
Return value:		A Token structure
*/
Token mlwpar_next_token(Buffer * sc_buf)
{
   Token t; /* token to return after recognition */
   unsigned char c; /* input symbol */
   int state; /* initial state of the FSM */
   short lexstart;  /*start offset of a lexeme in the input buffer */
   short lexend;    /*end   offset of a lexeme in the input buffer */
   int accept = NOAS; /* type of state - initially not accepting */                                     
        
   int index = 0; /* Used for loops */
   int count = 0; /* Used to count */
	
   while (1){ /* endless loop broken by token returns it will generate a warning */
        	
	c = b_getc(sc_buf);

	/* Process state transition table */
	if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c >= '0' && c <= '9')
	{
		/* Set mark to the beginning of this lexeme */
		b_setmark(sc_buf, b_getc_offset(sc_buf) - 1);

		state = 0;

		state = get_next_state(state, c, &accept);

		while (as_table[state] == NOAS) {
			c = b_getc(sc_buf);
			state = get_next_state(state, c, &accept);
		}

		/* If accepting state with retraction, retract buffer */
		if (accept == ASWR) /*as_table[state] to accept*/
		{
			b_retract(sc_buf);
		}

		/* Set lex variables for lexeme edges */
		lexstart = b_mark(sc_buf);
		lexend = b_getc_offset(sc_buf);

		/* Create temp buffer for lexeme*/
#ifdef DEBUG
		printf("lexend - lexstart = %i.\n", lexend - lexstart);
#endif
		lex_buf = b_create(lexend - lexstart + 1, 0, 'f'); /*FIXED: MADE STATIC*/
		/*FIXED: Added RTE checking*/
		if (!lex_buf)
		{
#ifdef DEBUG
			printf("lex_buf in state machine failed to create.\n");
#endif
			scerrnum = RTERROR;
			return aa_table[ES](RTE_STRING);
		}

		/*FIXED: Added RTE checking*/
		if (b_retract_to_mark(sc_buf) == R_FAIL_1)
		{
#ifdef DEBUG
			printf("retract to mark in state machine failed.\n");
#endif
			scerrnum = RTERROR;
			return aa_table[ES](RTE_STRING);
		}
		
		index = lexend - lexstart;

		/* Transfer the lexeme from one buffer to another */
		for (; index > 0; --index)
		{
			c = b_getc(sc_buf);
			if(b_addc(lex_buf, c) == NULL)
			{
#ifdef DEBUG
				printf("addc to lex_buf in state machine failed.\n");
#endif
				scerrnum = RTERROR;
				return aa_table[ES](RTE_STRING);
			}
		}

		/* Add a null terminator for easier ways to deal with strings */
		if(b_addc(lex_buf, '\0') == NULL)
		{
#ifdef DEBUG
			printf("addc of null terminator to lex_buf in state machine failed.\n");
#endif
			scerrnum = RTERROR;
			return aa_table[ES](RTE_STRING);
		}

		t = aa_table[state](b_setmark(lex_buf, 0));

		b_destroy(lex_buf);

		return t;
	}

	/* special cases or token driven processing */
	switch (c) {
	case ' ':
	case '\t':
	case '\v':
	case '\f':
		continue;
	case '\r':
		c = b_getc(sc_buf);
		if (c == '\n')
		{
			++line;
			continue;
		}
		b_retract(sc_buf);
		continue;
	case '\n':
		++line;
		continue;
	case '{':
		t.code = LBR_T;
		return t;
	case '}':
		t.code = RBR_T;
		return t;
	case '(':
		t.code = LPR_T;
		return t;
	case ')':
		t.code = RPR_T;
		return t;
	case '+':
		t.code = ART_OP_T;
		t.attribute.arr_op = PLUS;
		return t;
	case '-':
		t.code = ART_OP_T;
		t.attribute.arr_op = MINUS;
		return t;
	case '*':
		t.code = ART_OP_T;
		t.attribute.arr_op = MULT;
		return t;
	case '/':
		t.code = ART_OP_T;
		t.attribute.arr_op = DIV;
		return t;
	case '.':
		b_setmark(sc_buf, b_getc_offset(sc_buf) - 1);
		/* Get the next character */
		c = b_getc(sc_buf);

		/* Check to see if the next character is an
			actual logical operator*/
		if (c == 'A')
		{
			c = b_getc(sc_buf);
			if (c == 'N')
			{
				c = b_getc(sc_buf);
				if (c == 'D')
				{
					c = b_getc(sc_buf);
					if (c == '.')
					{
						t.code = LOG_OP_T;
						t.attribute.log_op = AND;
						return t;
					}
				}
			}
		}
		/* Check to see if lexeme is .OR. */
		else if (c == 'O')
		{
			c = b_getc(sc_buf);
			if (c == 'R')
			{
				c = b_getc(sc_buf);
				if (c == '.')
				{
					t.code = LOG_OP_T;
					t.attribute.log_op = OR;
					return t;
				}
			}
		}
			
		/* Last case, it is an error */
		b_retract_to_mark(sc_buf);
		t.code = ERR_T;
		t.attribute.err_lex[0] = b_getc(sc_buf);
		t.attribute.err_lex[1] = '\0';			
		return t;
	case '=':
		/* Verify if character is an egality operator
			or an assignment operator */
		c = b_getc(sc_buf);
			
		/* If another = follows, then it's a
			relational operator */
		if (c == '=')
		{
			t.code = REL_OP_T;
			t.attribute.rel_op = EQ;
			return t;
		}

		/* Otherwise, retract and it is an
			assignment operator */
		b_retract(sc_buf);
		t.code = ASS_OP_T;
		return t;
	case '<':
		/* Is definitely a relational operator */
		c = b_getc(sc_buf);
		t.code = REL_OP_T;
		/* Verify if it is unequal */
		if (c == '>')
		{
			t.attribute.rel_op = NE;
			return t;
		}
		/* Otherwise, it's a Lesser Than sign */
		b_retract(sc_buf);
		t.attribute.rel_op = LT;
		return t;
	case '>':
		t.code = REL_OP_T;
		t.attribute.log_op = GT;
		return t;
	case ';':
		t.code = EOS_T;
		return t;
	case '!':
		c = b_getc(sc_buf);
			
		/* Check if its a correct comment */
		if (c == '<')
		{
			/* Skip all characters until the newline is found */
			c = b_getc(sc_buf);

			while (c != '\n')
			{
				if (c == EOF || c == UCHAR_MAX || c == '\0')
				{
					t.code = SEOF_T;
					return t;
				}
				c = b_getc(sc_buf);
			}
				
			/* Retract back to newline */
			if (b_retract(sc_buf) == R_FAIL_1)
			{
#ifdef DEBUG
				printf("Retract in comment failed.\n");
#endif
				scerrnum = RTERROR;
				return aa_table[ES](RTE_STRING);
			}
			continue;
		}
		
		if (c == EOF || c == UCHAR_MAX || c == '\0')
		{
			t.code = SEOF_T;
			return t;
		}
		
		/* Error, not a comment */
		t.code = ERR_T;
				
		t.attribute.err_lex[0] = '!';
		t.attribute.err_lex[1] = c;
		t.attribute.err_lex[2] = '\0';

		/* Skip all characters until the newlines is found */
		c = b_getc(sc_buf);
		while (c != '\n')
		{
			c = b_getc(sc_buf);
		}
		
		++line;
		return t;
		
	case ',':
		t.code = COM_T;
		return t;
	case '#':
		t.code = SCC_OP_T;
		return t;
	case '"':
		lexstart = b_getc_offset(sc_buf);
		b_setmark(sc_buf, lexstart);
			
		c = b_getc(sc_buf);
		if (c == '\n')
			++line;

		/* Verify if it's an empty string */
		if (c == '"')
		{
			t.code = STR_T;
			t.attribute.str_offset = b_getc_offset(str_LTBL);

			b_addc(str_LTBL, '\0');

			b_setmark(str_LTBL, b_getc_offset(str_LTBL) + 1);
			b_retract_to_mark(str_LTBL);
				
			return t;
		}

		/* Skip over all characters until the string terminating character is found */
		while (c != '"')
		{
			c = b_getc(sc_buf);

			if (c == '\n')
				++line;

			if (c == EOF || c == UCHAR_MAX || c == '\0')
			{
				t.code = ERR_T;

				b_retract(sc_buf); /*Retract from SEOF*/
				lexend = b_getc_offset(sc_buf); /*Set lexend to last character*/

				lexstart = lexstart - 1; /* Include the first " into the edge of the lexeme */

				/* Create temp buffer for lexeme*/
				lex_buf = b_create(lexend - lexstart + 1, 0, 'f'); /*FIXED: MADE STATIC*/
				/*FIXED: Added RTE checking*/
				if (!lex_buf)
				{
#ifdef DEBUG
					printf("lex_buf in string failed to create.\n");
#endif
					scerrnum = RTERROR;
					return aa_table[ES](RTE_STRING);
				}

				b_retract_to_mark(sc_buf); /*Retract to the first letter of the String*/
				b_retract(sc_buf); /*Retract to include first " */

				index = lexend - lexstart;

				/* Transfer the lexeme from one buffer to another */
				for (; index > 0; --index)
				{
					c = b_getc(sc_buf);
					if (b_addc(lex_buf, c) == NULL)
					{
#ifdef DEBUG
						printf("Failed to add char to lexbuf in string error.\n");
#endif
						scerrnum = RTERROR;
						return aa_table[ES](RTE_STRING);
					}
				}

				/* Add a null terminator for easier ways to deal with strings */
				if (b_addc(lex_buf, '\0') == NULL)
				{
#ifdef DEBUG
					printf("Failed to add null terminator to error in string.\n");
#endif
					scerrnum = RTERROR;
					return aa_table[ES](RTE_STRING);
				}

				t = aa_table[ES](b_setmark(lex_buf, 0));

				b_destroy(lex_buf);

				return t;
			}
		}
			
		/* String is valid, return to begining of literal */
		b_retract_to_mark(sc_buf);

		t.code = STR_T;
		t.attribute.str_offset = b_getc_offset(str_LTBL);

		count = 0;

		c = b_getc(sc_buf);

		while (c != '"')
		{
			if (b_addc(str_LTBL, c) == NULL)
			{
#ifdef DEBUG
				printf("Failed to add char to string table.\n");
#endif
				scerrnum = RTERROR;
				return aa_table[ES](RTE_STRING);
			}
			++count;
			c = b_getc(sc_buf);
		}

		b_addc(str_LTBL, '\0');
		++count;

		b_setmark(str_LTBL, (short)(b_getc_offset(str_LTBL) + count));
		b_retract_to_mark(str_LTBL);

		return t;
	case '\0':
	case EOF:
	case UCHAR_MAX:
		t.code = SEOF_T;
		return t;
	default:
		/* Unrecognized symbol, causes error */
		t.code = ERR_T;
		t.attribute.err_lex[0] = c;
		t.attribute.err_lex[1] = '\0';
		return t;
		}   /*end of switch statement */
	}
}

/*
Purpose:			Function which determines the 
						next transition state to move
						to.
Author:				S. Ranev
History/Versions:	N/A
Called functions:	aa_funXX(char *lexeme)
					char_class()
Parameters:			int state: the current state
					char c: the current character
					int *accept:
Return value:		An integer representing the next state
*/
int get_next_state(int state, char c, int *accept)
{
	int col;
	int next;
	col = char_class(c);
	next = st_table[state][col];
#ifdef DEBUG
printf("Input symbol: %c Row: %d Column: %d Next: %d \n",c,state,col,next);
#endif

       assert(next != IS);

#ifdef DEBUG
	if(next == IS){
	  printf("Scanner Error: Illegal state:\n");
	  printf("Input symbol: %c Row: %d Column: %d\n",c,state,col);
	  exit(1);
	}
#endif
	*accept = as_table[next];
	return next;
}

/*
Purpose:			Function which returns an int
						representing the characters 
						column in the transition table.
Author:				Justin Bertrand
History/Versions:	1.0, Oct. 26th 2015
Called functions:	None
Parameters:			char c: the character to be processed.
Return value:		An integer representing the column.
*/
int char_class (char c)
{
		if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z')
		{
			return LETTER_COLUMN;
		}

		if (c >= '1' && c <= '7')
		{
			return NzO_DIGITS_COLUMN;
		}

		switch (c)
		{
		case '0':
			return ZERO_COLUMN;
		case '8':
		case '9':
			return EIGHT_NINE_COLUMN;
		case '.':
			return DOT_COLUMN;
		case '%':
			return STRING_VID_DELIMETER_COLUMN;
		default:
			return OTHER_COLUMN;
		}
}

/*
Purpose:			Function which determines if a lexeme
						is a Keyword or an Arithmetic Variable
						Identifier.
Author:				Justin Bertrand
History/Versions:	1.1, Oct. 26th 2015
Called functions:	strlen()
					iskeyword()
					memcpy()
Parameters:			lexeme: a pointer to an array of characters
Return value:		A Token structure
*/
Token aa_func02(char lexeme[]){
	Token t;
	int kw_result;
	char type;

	kw_result = iskeyword(lexeme);
	if (kw_result != -1)
	{
		t.code = KW_T;
		t.attribute.kwt_idx = kw_result;
		return t;
	}
	
	t.code = AVID_T;

	if (strlen(lexeme) > VID_LEN)
	{
		lexeme[VID_LEN] = '\0';
	}

	switch (lexeme[0])
	{
	case 'i': case 'o': case 'd': case 'w':
	case 'I': case 'O': case 'D': case 'W':
		type = ST_CHAR_TYPE_INT;
		break;
	default:
		type = ST_CHAR_TYPE_FLOAT;
	}

	t.attribute.vid_offset = st_install(sym_table, lexeme, type, line);
	
	/*Verify for runtime error
		CAUTION: The lexeme buffer will never be destroyed */
	if (t.attribute.vid_offset == ST_FAIL_1)
	{
		printf("\nError: The Symbol Table is full - install failed.\n");
		st_store(sym_table);
		exit(RTERROR_ST_INSTALL);
	}
		
	return t;
}

/*
Purpose:			Function which determines if a lexeme
						is a String Variable Identifier.
Author:				Justin Bertrand
History/Versions:	1.1, Oct. 26th 2015
Called functions:	strlen()
					memcpy()
Parameters:			lexeme: a pointer to an array of characters
Return value:		A Token structure
*/
Token aa_func03(char lexeme[]){
   
	Token t;
	int str_length;

	t.code = SVID_T;

	str_length = strlen(lexeme);
	str_length = str_length > VID_LEN-1 ? VID_LEN-1 : str_length-1;

	lexeme[str_length] = '%';
	lexeme[str_length + 1] = '\0';

	t.attribute.vid_offset = 
		st_install(sym_table, lexeme, ST_CHAR_TYPE_STRING, line);
	
	/*Verify for runtime error
		CAUTION: The lexeme buffer will never be destroyed */
	if (t.attribute.vid_offset == ST_FAIL_1)
	{
		printf("Error: The Symbol Table is full - install failed.\n");
		st_store(sym_table);
		exit(RTERROR_ST_INSTALL);
	}

	return t;
}

/*
Purpose:			Function which determines if a lexeme
						is a Floating Point Litteral.
Author:				Justin Bertrand
History/Versions:	1.1, Oct. 26th 2015
Called functions:	strlen()
					memcpy()
					atof()
Parameters:			lexeme: a pointer to an array of characters
Return value:		A Token structure
*/
Token aa_func08(char lexeme[]){

	Token t;
	double value;
	int str_length;

	str_length = strlen(lexeme);

	value = atof(lexeme);

	if (value > FPL_MAX || (float)value == 0.0 && value > 0.0)
	{
		return aa_table[ES](lexeme);
	}

	t.code = FPL_T;
	t.attribute.flt_value = (float)value;
	return t;
}

/*
Purpose:			Function which determines if a lexeme
						is a Decimal Integer Literal.
Author:				Justin Bertrand
History/Versions:	1.1, Oct. 26th 2015
Called functions:	strlen()
					atoi()
					memcpy()
Parameters:			lexeme: a pointer to an array of characters
Return value:		A Token structure
*/
Token aa_func05(char lexeme[])
{
	Token t;
	int value;
	/*int str_length;*/

	if (strlen(lexeme) > INL_LEN)
		return aa_table[ES](lexeme);

	value = atoi(lexeme);

	if (value > SHRT_MAX || value < 0)
	{
		/*str_length = strlen(lexeme);

		t.code = ERR_T;
		str_length = str_length > ERR_LEN ? ERR_LEN : str_length;
		memcpy(t.attribute.err_lex, lexeme, str_length);
		t.attribute.err_lex[str_length] = '\0';*/
		return aa_table[ES](lexeme);
	}
	
	t.code = INL_T;
	t.attribute.int_value = value;

	return t;
}

/*
Purpose:			Function which determines if a lexeme
						is an Octal Integer Literal.
Author:				Justin Bertrand
History/Versions:	1.1, Oct. 26th 2015
Called functions:	strlen()
					atool()
					memcpy()
Parameters:			lexeme: a pointer to an array of characters
Return value:		A Token structure
*/
Token aa_func10(char lexeme[]){

	Token t;

	int value;
	/*int str_length;*/

	if (strlen(lexeme) > OIL_LEN)
		return aa_table[ES](lexeme);

	value = atool(lexeme);

	if (value > IL_MAX || value < 0)
		/*str_length = strlen(lexeme);

		t.code = ERR_T;
		str_length = str_length > ERR_LEN ? ERR_LEN : str_length;
		memcpy(t.attribute.err_lex, lexeme, str_length*sizeof(char));
		t.attribute.err_lex[str_length] = '\0';*/
		return aa_table[ES](lexeme);

	t.code = INL_T;
	t.attribute.int_value = value;

	return t;
}

/*
Purpose:			Function which creates an Error Token
						from a lexeme.
Author:				Justin Bertrand
History/Versions:	1.1, Oct. 26th 2015
Called functions:	strlen()
					memcpy()
Parameters:			lexeme: a pointer to an array of characters
Return value:		A Token structure
*/
Token aa_func12(char lexeme[]){

	Token t;
	int str_length = strlen(lexeme);

	t.code = ERR_T;

	/*str_length = str_length > ERR_LEN ? ERR_LEN : str_length;
	memcpy(t.attribute.err_lex, lexeme, str_length);
	t.attribute.err_lex[str_length] = '\0';*/

	if (str_length > ERR_LEN)
	{
		memcpy(t.attribute.err_lex, lexeme, ERR_LEN - 3);
		t.attribute.err_lex[ERR_LEN - 3] = '.';
		t.attribute.err_lex[ERR_LEN - 2] = '.';
		t.attribute.err_lex[ERR_LEN - 1] = '.';
		t.attribute.err_lex[ERR_LEN] = '\0';
		
		return t;
	}

	memcpy(t.attribute.err_lex, lexeme, str_length + 1);

	return t;
}

/*
Purpose:			Function which converts an octal number
						to a decimal one.
Author:				Justin Bertrand
History/Versions:	1.1, Oct. 26th 2015
Called functions:	power()
					strlen()
Parameters:			lexeme: a pointer to an array of characters
Return value:		long: the converted octal.
*/
long atool(char * lexeme){

	int length = strlen(lexeme)-1;
	int index;
	long value = 0;

	for (index = 1; index < length+1; ++index) {
		value += (int)(lexeme[index] - '0') * power(8, length - index);
	}

	return value;
}

/*
Purpose:			Function calculates an exponential.
Author:				Justin Bertrand
History/Versions:	1.1, Oct. 26th 2015
Called functions:	none
Parameters:			int x: the base of the exponent
					int y: the exponent
Return value:		int: the value of the exponent
*/
int power(int x, int y)
{
	int index;
	int value = 1;

	for (index = 0; index < y; ++index)
	{
		value *= x;
	}
	
	return value;
}

/*
Purpose:			Function which determines if a lexeme
						is a KEYWORD.
Author:				Justin Bertrand
History/Versions:	1.1, Oct. 26th 2015
Called functions:	strcmp()
Parameters:			lexeme: a pointer to an array of characters
Return value:		int: the index of the keyword in the keyword table
*/
int iskeyword(char * kw_lexeme)
{
	int index;

	for (index = 0; index < KWT_SIZE; ++index)
	{
		if (strcmp(kw_lexeme, kw_table[index]) == 0)
		{
			return index;
		}
	}
	return -1;
}
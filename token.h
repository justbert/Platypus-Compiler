/* Filename: token.h
 * Token declarations necessary for the scanner implementation 
 * CST8152, Assignment #2
 * Version: 1.15.02
 * Date: 29 September 2015
 * Provided by: Svillen Ranev
 * The file is complete and MUST NOT be modified. 
 */
#ifndef TOKEN_H_
#define TOKEN_H_

/*#pragma warning(1:4001) *//*to enforce C89 type comments  - to make //comments an warning */

/*#pragma warning(error:4001)*//* to enforce C89 comments - to make // comments an error */

/* Constants */
#define VID_LEN 8   /* variable identifier length */
#define ERR_LEN 20  /* error message length */
#define INL_LEN 5   /* maximum number of digits for IL */
#define OIL_LEN 7	/* maximum length of octal literals */
#define IL_MAX 32767	/* maximum value of an IL*/
#define FPL_MAX 3.402823455e+38F	/*maximum value for FPL*/

/* Token codes */
#define ERR_T     0  /* Error token */
#define SEOF_T    1  /* Source end-of-file token */
#define AVID_T    2  /* Arithmetic Variable identifier token */
#define SVID_T    3  /* String Variable identifier token */
#define FPL_T     4  /* Floating point literal token */
#define INL_T     5  /* Integer literal token */
#define STR_T     6  /* String literal token */
#define SCC_OP_T  7  /* String concatenation operator token */
#define ASS_OP_T  8  /* Assignment operator token */
#define ART_OP_T  9  /* Arithmetic operator token */
#define REL_OP_T 10  /* Relational operator token */ 
#define LOG_OP_T 11  /* Logical operator token */
#define LPR_T    12  /* Left parenthesis token */
#define RPR_T    13  /* Right parenthesis token */
#define LBR_T    14  /* Left brace token */
#define RBR_T    15  /* Right brace token */
#define KW_T     16  /* Keyword token */
#define COM_T    17  /* Comma token */
#define EOS_T    18  /* End of statement *(semi - colon) */

/*KEYWORD INDEXES*/
#define KW_ELSE 0
#define KW_IF 1
#define KW_INPUT 2
#define KW_OUTPUT 3
#define KW_PLATYPUS 4
#define KW_REPEAT 5
#define KW_THEN 6
#define KW_USING 7

/*Run Time Error Constants*/
#define RTERROR 1 /*Value if a Run Time Error occurs*/
#define RTERROR_ST_INSTALL 2 /*Value of the run time error when a VID fails to be
								added to the symbol table*/
#define RTE_STRING "RUN TIME ERROR: " /*Message of a run time error*/

/* Operators token attributes */
typedef enum ArithmeticOperators  {PLUS, MINUS, MULT, DIV} Arr_Op;
typedef enum RelationalOperators  {EQ, NE, GT, LT} Rel_Op;
typedef enum LogicalOperators     {AND,OR} Log_Op;

#define NO_ATTR 1024

/* Structure declaring the token and its attributes */

typedef union TokenAttribute{
	int get_int;      /* integer attributes accessor */
	Arr_Op arr_op;    /* arithmetic operator attribute code */
	Rel_Op rel_op;    /* relational operator attribute code */
	Log_Op log_op;    /* logical operator attribute code */
	int int_value;    /* integer literal attribute (value) */
	int kwt_idx;      /* keyword index in the keyword table */	  
	short str_offset;   /* sring literal offset from the beginning of */
	                  /* the string literal buffer (str_LTBL->ca_head) */
	float flt_value;    /* floating-point literal attribute (value) */
    int vid_offset; /* variable identifier token attribute */
    char err_lex[ERR_LEN+1]; /* error token attribite */
  } TA;

typedef struct Token
{
	int code;                 /* token code */
	TA attribute; /* token attribute */
} Token;

#endif

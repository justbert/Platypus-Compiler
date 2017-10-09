/*	Filename: parser.h
 *	PURPOSE: The header file for the top-down parser of the 
 *				Platypus language.
 *	Compiler: MS Visual Studio 2013, Update 5
 *	Course :	CST 8152 - Compiler, Lab Section: 011
 *	Assignment:  Assignment 4 - The Parser 
 *	Created by: Justin Bertrand, 040 786 592
 *  Version: 1.0
 *  Date: 11 December 2015
 *	Warnings:		Please note that external libraries use non-ANSI single 
					line comments which cause warnings
 */
#ifndef PARSER_H_
#define PARSER_H_

#include "token.h"
#include "stable.h"

/*Global Variables*/
Token lookahead;/*The token which allows for predictability*/
pBuffer sc_buf; /*The buffer containing the scanner tokens*/
int synerrno; /*The number of syntax errors*/
extern int line; /*The current line in the program*/
extern STD sym_table; /*The symbol table hosting the variables*/
extern pBuffer str_LTBL; /*The string literal buffer*/
extern char * kw_table[]; /*Table containing all keywords*/

void parser(pBuffer);
void match(int, int);

/*PRODUCTION FUNCTION PROTOTYPES*/
void program(void);
void opt_statements(void);
void statements(void);
void statements_prime(void);
void statement(void);
void assignment_statement(void);
void assignment_expression(void);
void selection_statement(void);
void iteration_statement(void);
void input_statement(void);
void output_statement(void);
void output_list(void);
void variable_list(void);
void variable_list_prime(void);
void arithmetic_expression(void);
void unary_arithmetic_expression(void);
void additive_arithmetic_expression(void);
void additive_arithmetic_expression_prime(void);
void multiplicative_arithmetic_expression(void);
void multiplicative_arithmetic_expression_prime(void);
void primary_arithmetic_expression(void);
void string_expression(void);
void string_expression_prime(void);
void primary_string_expression(void);
void conditional_expression(void);
void logical_or_expression(void);
void logical_or_expression_prime(void);
void logical_and_expression(void);
void logical_and_expression_prime(void);
void variable_identifier(void);
void relational_expression(void);
void relational_expression_beta(void);
void relational_expression_alpha(void);
void primary_s_relational_expression(void);
void primary_a_relational_expression(void);

void gen_incode(char*);

/*Error handling prototypes*/
void syn_eh(int);
void syn_printe(void);

/*External functions used in the Parser*/
extern Token mlwpar_next_token(pBuffer); /*Scans in the next token from the input buffer*/

#endif

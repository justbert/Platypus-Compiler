/*	Filename: parser.h
 *	PURPOSE: Top-down parser of the Platypus language.
 *	Compiler: MS Visual Studio 2013, Update 5
 *	Course :	CST 8152 - Compiler, Lab Section: 011
 *	Assignment:  Assignment 4 - The Parser 
 *	Created by: Justin Bertrand, 040 786 592
 *  Version: 1.0
 *  Date: 11 December 2015
 *	Warnings:		Please note that external libraries use non-ANSI single 
					line comments which cause warnings
 */

/*INCLUDE STATEMENTS*/
#include <stdlib.h>
#include "parser.h"
#include "token.h"
#include "stable.h"

/*
Purpose:			Function which initializes the parser
Author:				S. Ranev
History/Versions:	N/A
Called functions:	None
Parameters:			in_buf: A pointer to a buffer structure
Return value:		None
*/
void parser(pBuffer in_buf)
{
	sc_buf = in_buf;
	lookahead = mlwpar_next_token(sc_buf);
	program(); match(SEOF_T, NO_ATTR);
	gen_incode("PLATY: Source file parsed\n");
}

/*
Purpose:			Function which matches the current lookahead
						token to the parameters passed.
Author:				J. Bertrand
History/Versions:	1.0
Called functions:	syn_printe()
					syn_eh()
					mlwpar_next_token(sc_buf)
Parameters:			int pr_token_code: The type of token
					int pr_token_attribute: The attribute of the token
Return value:		None.
*/
void match(int pr_token_code, int pr_token_attribute)
{
	if (lookahead.code == pr_token_code)
	{
		/*Switch case for tokens with attributes
			and SEOF which does not need to get the next token*/
		switch (lookahead.code)
		{
		case SEOF_T:
			return;
		case ART_OP_T:
		case REL_OP_T:
		case LOG_OP_T:
		case KW_T:
			if (lookahead.attribute.get_int != pr_token_attribute)
				syn_eh(pr_token_code);
		}

		lookahead = mlwpar_next_token(sc_buf);

		/*Move over the error tokens*/
		if (lookahead.code == ERR_T)
		{
			syn_printe();
			++synerrno;/*increment the number of errors*/
			lookahead = mlwpar_next_token(sc_buf);
		}
	}
	else
	{
		syn_eh(pr_token_code);
	}
}

/*PRODUCTION FUNCTIONS*/
void program(void)
{
	match(KW_T, KW_PLATYPUS);
	match(LBR_T, NO_ATTR);
	opt_statements();
	match(RBR_T, NO_ATTR);
	gen_incode("PLATY: Program parsed\n");
}

/*

Author:		S. Ranev
FIRST SET:	{AVID, SVID, KW_T(IF), KW_T(USING), KW_T(INPUT), KW_T(OUTPUT), epsilon}
*/
void opt_statements(void)
{
	switch (lookahead.code)
	{
	case AVID_T:
	case SVID_T:
		statements();
		break;
	case KW_T:
		if (lookahead.attribute.get_int != KW_PLATYPUS
			&& lookahead.attribute.get_int != KW_ELSE
			&& lookahead.attribute.get_int != KW_THEN
			&& lookahead.attribute.get_int != KW_REPEAT)
		{
			statements();
			break;
		}
	default: /*epsilon*/;
		gen_incode("PLATY: Opt_statements parsed\n");
	}
}

void statements(void)
{
	statement();
	statements_prime();
}

/*
FIRST SET: {AVID, SVID, KW_T(IF), KW_T(USING), KW_T(INPUT), KW_T(OUTPUT), epsilon}
*/
void statements_prime(void)
{
	switch (lookahead.code)
	{
	case AVID_T:
	case SVID_T:
		statements();
		statements_prime();
		break;
	case KW_T:
		if (lookahead.attribute.get_int != KW_PLATYPUS
			&& lookahead.attribute.get_int != KW_ELSE
			&& lookahead.attribute.get_int != KW_THEN
			&& lookahead.attribute.get_int != KW_REPEAT)
		{
			statements();
			statements_prime();
			break;
		}
	default:	
		/*epsilon*/;
		
	}
}

/*
FIRST SET:
FIRST(<statement>) = {FIRST(<assignment statement>), FIRST(<selection statement>), 
FIRST(<iteration statement>), FIRST(<input statement>), FIRST(<output statement>)} = 
{AVID, SVID, KW_T(IF), KW_T(USING), KW_T(INPUT), KW_T(OUTPUT)}

*/
void statement(void)
{
	switch (lookahead.code)
	{
	case AVID_T:
	case SVID_T:
		assignment_statement();
		break;
	case KW_T:
		switch(lookahead.attribute.get_int) 
		{
		case KW_IF:
			selection_statement();
			break;
		case KW_USING:
			iteration_statement();
			break;
		case KW_INPUT:
			input_statement();
			break;
		case KW_OUTPUT:
			output_statement();
		}
	}
	/*gen_incode("PLATY: Statement parsed\n");*/
}

/*
3.2.1	Assignment Statement

<assignment statement> ->
<assignment expression>;

FIRST(<assignment statement>) = {FIRST(<assignment expression>)} = {AVID, SVID}
*/
void assignment_statement(void)
{
	assignment_expression();
	match(EOS_T, NO_ATTR);
	gen_incode("PLATY: Assignment statement parsed\n");
}

/*
< assignment expression> ->
AVID = <arithmetic expression>
| SVID = <string expression>

FIRST(<assignment expression>) = {AVID, SVID}
*/
void assignment_expression(void)
{
	switch (lookahead.code)
	{
	case AVID_T:
		match(AVID_T, NO_ATTR);
		match(ASS_OP_T, NO_ATTR);
		arithmetic_expression();
		gen_incode("PLATY: Assignment expression (arithmetic) parsed\n");
		break;
	case SVID_T:
		match(SVID_T, NO_ATTR);
		match(ASS_OP_T, NO_ATTR);
		string_expression();
		gen_incode("PLATY: Assignment expression (string) parsed\n");
		break;
	default:
		syn_printe();
	}
}

/*
3.2.2  Selection Statement( the if statement)

<selection statement> ->
IF (<conditional expression>)  THEN  <opt_statements>
ELSE { <opt_statements> } ;

FIRST(<selection statement>) = {KW_T(IF)}
*/
void selection_statement(void)
{
	match(KW_T, KW_IF);
	match(LPR_T, NO_ATTR);
	conditional_expression();
	match(RPR_T, NO_ATTR);
	match(KW_T, KW_THEN);
	opt_statements();
	match(KW_T, KW_ELSE);
	match(LBR_T, NO_ATTR);
	opt_statements();
	match(RBR_T, NO_ATTR);
	match(EOS_T, NO_ATTR);
	gen_incode("PLATY: IF statement parsed\n");
}

/*
3.2.3  Iteration Statement (the loop statement)

<iteration statement> ->
USING  (<assignment expression> , <conditional expression> , <assignment  expression> )
REPEAT {
     < opt_statements>
	};

FIRST(<iteration statement>) = {KW_T(USING)}
*/
void iteration_statement(void)
{
	match(KW_T, KW_USING);
	match(LPR_T, NO_ATTR);
	assignment_expression();
	match(COM_T, NO_ATTR);
	conditional_expression();
	match(COM_T, NO_ATTR);
	assignment_expression();
	match(RPR_T, NO_ATTR);
	match(KW_T, KW_REPEAT);
	match(LBR_T, NO_ATTR);
	opt_statements();
	match(RBR_T, NO_ATTR);
	match(EOS_T, NO_ATTR);
	gen_incode("PLATY: USING statement parsed\n");
}

/*
3.2.4 Input Statement

<input statement> ->
INPUT (<variable list>);

FIRST(<input statement>) = {KW_T(INPUT)}
*/
void input_statement(void)
{
	match(KW_T, KW_INPUT);
	match(LPR_T, NO_ATTR);
	variable_list();
	gen_incode("PLATY: Variable list parsed\n");
	match(RPR_T, NO_ATTR);
	match(EOS_T, NO_ATTR);
	gen_incode("PLATY: INPUT statement parsed\n");
}

/*
ORIGINAL PRODUTION
<variable list> ->
<variable identifier> | <variable list>,<variable identifier>

REWORKED PRODUCTION

Eliminate Left-Recursion
<variable list> -> <variable identifier><variable list prime>

FIRST(<variable list>) = FIRST(<variable identifier>) = {AVID, SVID}
*/
void variable_list(void)
{
	variable_identifier();
	variable_list_prime();
}

/*
<variable list prime> -> COM_T <variable identifier><variable list prime>| epsilon

FIRST(<variable list prime>) = {COM_T, epsilon}
*/
void variable_list_prime(void)
{
	switch(lookahead.code)
	{
	case COM_T:
		match(COM_T, NO_ATTR);
		variable_identifier();
		variable_list_prime();
		break;
	default:
		/*epsilon*/;
	}
}

/*
3.2.5 Output Statement

ORIGINAL PRODUCTION
<output statement> ->
OUTPUT (<opt_variable list>);
| OUTPUT (STR_T);

REWORKED PRODUCTION
Remove predictability issue via Left-Factoring

<output statement> -> KW_T(OUTPUT)LPR_T<output list>RPR_T EOS_T

FIRST(<output statement>) = {KW_T(OUTPUT)}
*/
void output_statement(void)
{
	match(KW_T, KW_OUTPUT);
	match(LPR_T, NO_ATTR);
	output_list();
	match(RPR_T, NO_ATTR);
	match(EOS_T, NO_ATTR);
	gen_incode("PLATY: OUTPUT statement parsed\n");
}

/*
<output list> -> <variable list> | STR_T | epsilon
FIRST(<output list>) = {FIRST(<variable list>), STR_T, epsilon) = 
{ AVID, SVID, STR_T, epsilon}
*/
void output_list(void)
{
	switch (lookahead.code)
	{
	case AVID_T:
	case SVID_T:
		variable_list();
		gen_incode("PLATY: Variable list parsed\n");
		break;
	case STR_T:
		match(STR_T, NO_ATTR);
		gen_incode("PLATY: Output list (string literal) parsed\n");
		break;
	default:
		/*epsilon*/;
		gen_incode("PLATY: Output list (empty) parsed\n");
	}
}

/*
<arithmetic expression> - >
<unary arithmetic expression>
| <additive arithmetic expression>

FIRST(<arithmetic expression>) = {FIRST(<unary arithmetic expression>),	
FIRST(<additive arithmetic expression)} =
{ART_OP_T(MINUS), ART_OP_T(PLUS), AVID_T, FPL_T, INL_T, LPR_T}
*/
void arithmetic_expression()
{
	switch (lookahead.code)
	{
	case ART_OP_T:
		if (lookahead.attribute.get_int == PLUS
			|| lookahead.attribute.get_int == MINUS)
			unary_arithmetic_expression();
		break;
	case AVID_T:
	case FPL_T:
	case INL_T:
	case LPR_T:
		additive_arithmetic_expression();
		break;
	}
	gen_incode("PLATY: Arithmetic expression parsed\n");
}

/*
<unary arithmetic expression> ->
-  <primary arithmetic expression>
| + <primary arithmetic expression>

FIRST(<unary arithmetic expression>) = { ART_OP_T(MINUS), ART_OP_T(PLUS)}
*/
void unary_arithmetic_expression(void)
{
	switch (lookahead.attribute.get_int)
	{
	case PLUS:
		match(ART_OP_T, PLUS);
		primary_arithmetic_expression();
		break;
	case MINUS:
		match(ART_OP_T, MINUS);
		primary_arithmetic_expression();
		break;
	}
	gen_incode("PLATY: Unary arithmetic expression parsed\n");
}

/*
ORIGINAL PRODUCTION
<additive arithmetic expression> ->
<additive arithmetic expression> +  <multiplicative arithmetic expression>
| <additive arithmetic expression>  -  <multiplicative arithmetic expression>
| <multiplicative arithmetic expression>

REWORKED EXPRESSION
Left-Recursion removal and Left-Factoring
<additive arithmetic expression> ->
<multiplicative arithmetic expression><additive arithmetic expression prime>

FIRST(<additive arithmetic expression>) = {FIRST(<multiplicative arithmetic expression>)} =
{FIRST(<primary arithmetic expression>)} = {AVID_T, FPL_T, INL_T, LPR_T}
*/

void additive_arithmetic_expression(void)
{
	multiplicative_arithmetic_expression();
	additive_arithmetic_expression_prime();
}

/*
<additive arithmetic expression prime> ->
ASS_OP_T(PLUS)<multiplicative arithmetic expression>
<additive arithmetic expression prime>
| ASS_OP_T(MINUS)<multiplicative arithmetic expression>
<additive arithmetic expression prime>
| epsilon

FIRST(<additive arithmetic expression prime>) =
{ASS_OP_T(PLUS), ASS_OP_T(MINUS), epsilon}
*/
void additive_arithmetic_expression_prime(void)
{
	switch (lookahead.code)
	{
	case ART_OP_T:
		switch (lookahead.attribute.get_int)
		{
		case PLUS:
			match(ART_OP_T, PLUS);
			multiplicative_arithmetic_expression();
			additive_arithmetic_expression_prime();
			gen_incode("PLATY: Additive arithmetic expression parsed\n");
			break;
		case MINUS:
			match(ART_OP_T, MINUS);
			multiplicative_arithmetic_expression();
			additive_arithmetic_expression_prime();
			gen_incode("PLATY: Additive arithmetic expression parsed\n");
			break;
		}
		break;
	default:
		/*epsilon*/;
	}
}

/*
ORIGINAL PRODUCTIONS
<multiplicative arithmetic expression> ->
<multiplicative arithmetic expression> * <primary arithmetic expression>
| <multiplicative arithmetic expression> / <primary arithmetic expression>
| <primary arithmetic expression>

REWORKED PRODUCTION
Left-Recursion removal and Left-Factoring
<multiplicative arithmetic expression> ->
<primary arithmetic expression><multiplicative arithmetic expression prime>

FIRST(<multiplicative arithmetic expression) = 
{FIRST(<primary arithmetic expression>)} =
{AVID_T, FPL_T, INL_T, LPR_T}
*/

void multiplicative_arithmetic_expression(void)
{
	primary_arithmetic_expression();
	multiplicative_arithmetic_expression_prime();
}

/*
<multiplicative arithmetic expression prime> ->
ASS_OP_T(MULT)<primary arithmetic expression>
<multiplicative arithmetic expression prime>
| ASS_OP_T(DIV)<primary arithmetic expression>
<multiplicative arithmetic expression prime>
| epsilon

FIRST(<multiplicative arithmetic expression prime>) =
{ASS_OP_T(MULT), ASS_OP_T(DIV), epsilon}
*/
void multiplicative_arithmetic_expression_prime(void)
{
	switch (lookahead.code)
	{
	case ART_OP_T:
		switch (lookahead.attribute.get_int)
		{
		case MULT:
			match(ART_OP_T, MULT);
			primary_arithmetic_expression();
			multiplicative_arithmetic_expression_prime();
			gen_incode("PLATY: Multiplicative arithmetic expression parsed\n");
			break;
		case DIV:
			match(ART_OP_T, DIV);
			primary_arithmetic_expression();
			multiplicative_arithmetic_expression_prime();
			gen_incode("PLATY: Multiplicative arithmetic expression parsed\n");
			break;
		}
		break;
	default:
		/*epsilon*/
		;
	}
}

/*
<primary arithmetic expression> ->
AVID_T
| FPL_T
| INL_T
| (<arithmetic expression>)

FIRST(<primary arithmetic expression) = {AVID_T, FPL_T, INL_T, LPR_T}
*/
void primary_arithmetic_expression(void)
{
	switch (lookahead.code)
	{
	case AVID_T:
		match(AVID_T, NO_ATTR);
		break;
	case FPL_T:
		match(FPL_T, NO_ATTR);
		break;
	case INL_T:
		match(INL_T, NO_ATTR);
		break;
	case LPR_T:
		match(LPR_T, NO_ATTR);
		arithmetic_expression();
		match(RPR_T, NO_ATTR);
	}
	gen_incode("PLATY: Primary arithmetic expression parsed\n");
}

/*
3.3.2 String Expression
ORIGINAL PRODUCTION
<string expression> ->
<primary string expression>
| <string expression>  #  <primary string expression>

REWORKED PRODUCTION
Left-Recursion removal
<string expression>->
<primary string expression> <string expression prime>

FIRST(<string expression>) = {FIRST(<primary string expression>)} = {SVID_T, STR_T}
*/

void string_expression(void)
{
	primary_string_expression();
	string_expression_prime();
	gen_incode("PLATY: String expression parsed\n");
}

/*
<string expression prime> ->
SCC_OP_T <primary string expression> <string expression prime> | epsilon

FIRST(<string expression prime>) = { SCC_OP_T, epsilon }
*/
void string_expression_prime()
{
	switch (lookahead.code)
	{
	case SCC_OP_T:
		match(SCC_OP_T, NO_ATTR);
		primary_string_expression();
		string_expression_prime();
		break;
	default:
		/*epsilon*/;
	}
}

/*
<primary string expression> ->
SVID_T
| STR_T

FIRST(<primary string expression>) = {SVID_T, STR_T}
*/
void primary_string_expression(void)
{
	switch (lookahead.code)
	{
	case SVID_T:
		match(SVID_T, NO_ATTR);
		break;
	case STR_T:
		match(STR_T, NO_ATTR);
		break;
	default:
		syn_printe();
	}
	gen_incode("PLATY: Primary string expression parsed\n");
}

/*
3.3.3 Conditional Expression

<conditional expression> ->
<logical OR  expression>
*/
void conditional_expression(void)
{
	logical_or_expression();
	gen_incode("PLATY: Conditional expression parsed\n");
}

/*
ORIGINAL PRODUCTION
<logical  OR expression> ->
<logical AND expression>
|  <logical OR expression>  .OR.  <logical AND expression>

REWORKED PRODUCTION
Remove Left-Recursion
<logical OR expression> ->
<logical AND expression><logical OR expression prime>

FIRST(<logical OR expression>) = {FIRST(<logical AND expression>)} =
{ AVID_T, FPL_T, INL_T, SVID_T, STR_T}
*/
void logical_or_expression(void)
{
	logical_and_expression();
	logical_or_expression_prime();
}

/*
<logical OR expression prime> ->
LOG_OP_T(OR) <logical AND expression><logical OR expression prime> | epsilon

FIRST(<logical OR expression prime>) = {LOG_OP_T(OR), epsilon }
*/
void logical_or_expression_prime(void)
{
	switch (lookahead.code)
	{
	case LOG_OP_T:
		if (lookahead.attribute.get_int == OR)
		{
			match(LOG_OP_T, OR);
			logical_and_expression();
			logical_or_expression_prime();
			gen_incode("PLATY: Logical OR expression parsed\n");
		}
		break;
	default:
		/*epsilon*/;
	}
}

/*
ORIGINAL PRODUCTION
<logical AND expression> ->
<relational expression>
|  <logical AND expression> .AND.  <relational expression>

REWORKED PRODUCTION
Remove Left-Recursion
<logical AND expression> ->
<relational expression> <logical AND expression prime>

FIRST(<logical AND expression>) = {FIRST(<relational expression>)} =
{ AVID_T, FPL_T, INL_T, SVID_T, STR_T}
*/
void logical_and_expression(void)
{
	relational_expression();
	logical_and_expression_prime();
}

/*
<logical AND expression prime> ->
LOG_OP_T(AND) <relational expression> <logical AND expression prime> | epsilon

FIRST(<logical AND expression prime>) = {LOG_OP_T(AND), epsilon }
*/
void logical_and_expression_prime(void)
{
	switch (lookahead.code)
	{
	case LOG_OP_T:
		if (lookahead.attribute.get_int == AND)
		{
			match(LOG_OP_T, AND);
			relational_expression();
			logical_and_expression_prime();
			gen_incode("PLATY: Logical AND expression parsed\n");
		}
		break;
	default:
		/*epsilon*/;
	}
}

/*
REWORKED PRODUCTION
Remove predictability issues by Left-Factoring
<relational expression> ->
<primary a_relational expression><relational expression beta>
| <primary s_relational expression><relation expression alpha>

FIRST(<relational expression>) = {FIRST(<primary a_relational expression>),
FIRST(<primary s_relational expression>)} = {AVID_T, FPL_T, INL_T, SVID_T, STR_T}
*/
void relational_expression()
{
	switch (lookahead.code)
	{
	case AVID_T:
	case FPL_T:
	case INL_T:
		primary_a_relational_expression();
		relational_expression_beta();
		break;
	case SVID_T:
	case STR_T:
		primary_s_relational_expression();
		relational_expression_alpha();
		break;
	default:
		syn_printe();
	}
	gen_incode("PLATY: Relational expression parsed\n");
}

/*
<relational expression beta> ->
REL_OP_T(EQ) <primary a_relational expression>
| REL_OP_T(NE) <primary a_relational expression>
| REL_OP_T(GT) <primary a_relational expression>
| REL_OP_T(LT) <primary a_relational expression>

FIRST(<relational expression beta>) =
{REL_OP_T(EQ), REL_OP_T(NE), REL_OP_T(GT), REL_OP_T(LT)}
*/
void relational_expression_beta(void)
{
	if (lookahead.code == REL_OP_T)
	{
		switch (lookahead.attribute.get_int)
		{
		case EQ:
			match(REL_OP_T, EQ);
			primary_a_relational_expression();
			break;
		case NE:
			match(REL_OP_T, NE);
			primary_a_relational_expression();
			break;
		case GT:
			match(REL_OP_T, GT);
			primary_a_relational_expression();
			break;
		case LT:
			match(REL_OP_T, LT);
			primary_a_relational_expression();
			break;
		}
	}
}

/*
<relational expression alpha> ->
REL_OP_T(EQ) <primary s_relational expression>
| REL_OP_T(NE) <primary s_relational expression>
| REL_OP_T(GT) <primary s_relational expression>
| REL_OP_T(LT) <primary s_relational expression>

FIRST(<relational expression alpha>) =
{REL_OP_T(EQ), REL_OP_T(NE), REL_OP_T(GT), REL_OP_T(LT)}
*/
void relational_expression_alpha(void)
{
	if (lookahead.code == REL_OP_T)
	{
		switch (lookahead.attribute.get_int)
		{
		case EQ:
			match(REL_OP_T, EQ);
			primary_s_relational_expression();
			break;
		case NE:
			match(REL_OP_T, NE);
			primary_s_relational_expression();
			break;
		case GT:
			match(REL_OP_T, GT);
			primary_s_relational_expression();
			break;
		case LT:
			match(REL_OP_T, LT);
			primary_s_relational_expression();
			break;
		}
	}
}

/*<primary a_relational expression> ->
  AVID_T
| FPL_T
| INL_T

FIRST(<primary a_relational expression>) = {AVID_T, FPL_T, INL_T}
*/

void primary_a_relational_expression(void)
{
	switch (lookahead.code)
	{
	case AVID_T:
		match(AVID_T, NO_ATTR);
		break;
	case FPL_T:
		match(FPL_T, NO_ATTR);
		break;
	case INL_T:
		match(INL_T, NO_ATTR);
		break;
	default:
		syn_printe();
	}
	gen_incode("PLATY: Primary a_relational expression parsed\n");
}

/*
<primary s_relational expression> ->
<primary string expression>

FIRST(<primary s_relational expression>) = {FIRST(<primary string expression>)} =
{SVID_T, STR_T}
*/
void primary_s_relational_expression(void)
{
	primary_string_expression();
	gen_incode("PLATY: Primary s_relational expression parsed\n");
}

void variable_identifier(void)
{
	switch (lookahead.code)
	{
	case AVID_T:
		match(AVID_T, NO_ATTR);
		break;
	case SVID_T:
		match(SVID_T, NO_ATTR);
		break;
	default:
		syn_printe();
	}
}

/*Error handling functions*/
void syn_eh(int sync_token_code)
{
	syn_printe();
	++synerrno;

	lookahead = mlwpar_next_token(sc_buf);

	while (lookahead.code != sync_token_code)
	{
		if (lookahead.code == SEOF_T)
			exit(synerrno);

		lookahead = mlwpar_next_token(sc_buf);
	}

	if (lookahead.code != SEOF_T)
		lookahead = mlwpar_next_token(sc_buf);
}

/*
Purpose:			Function which prints when an error occurs
Author:				S. Ranev
History/Versions:	N/A
Called functions:	printf()
Parameters:			None
Return value:		None
*/
void syn_printe()
{
	Token t = lookahead;

	printf("PLATY: Syntax error:  Line:%3d\n", line);
	printf("*****  Token code:%3d Attribute: ", t.code);
	switch (t.code){
	case  ERR_T: /* ERR_T     0   Error token */
		printf("%s\n", t.attribute.err_lex);
		break;
	case  SEOF_T: /*SEOF_T    1   Source end-of-file token */
		printf("NA\n");
		break;
	case  AVID_T: /* AVID_T    2   Arithmetic Variable identifier token */
	case  SVID_T:/* SVID_T    3  String Variable identifier token */
		printf("%s\n", sym_table.pstvr[t.attribute.get_int].plex);
		break;
	case  FPL_T: /* FPL_T     4  Floating point literal token */
		printf("%5.1f\n", t.attribute.flt_value);
		break;
	case INL_T: /* INL_T      5   Integer literal token */
		printf("%d\n", t.attribute.get_int);
		break;
	case STR_T:/* STR_T     6   String literal token */
		printf("%s\n", b_setmark(str_LTBL, t.attribute.str_offset));
		break;
	case SCC_OP_T: /* 7   String concatenation operator token */
		printf("NA\n");
		break;
	case  ASS_OP_T:/* ASS_OP_T  8   Assignment operator token */
		printf("NA\n");
		break;
	case  ART_OP_T:/* ART_OP_T  9   Arithmetic operator token */
		printf("%d\n", t.attribute.get_int);
		break;
	case  REL_OP_T: /*REL_OP_T  10   Relational operator token */
		printf("%d\n", t.attribute.get_int);
		break;
	case  LOG_OP_T:/*LOG_OP_T 11  Logical operator token */
		printf("%d\n", t.attribute.get_int);
		break;
	case  LPR_T: /*LPR_T    12  Left parenthesis token */
		printf("NA\n");
		break;
	case  RPR_T: /*RPR_T    13  Right parenthesis token */
		printf("NA\n");
		break;
	case LBR_T: /*    14   Left brace token */
		printf("NA\n");
		break;
	case RBR_T: /*    15  Right brace token */
		printf("NA\n");
		break;
	case KW_T: /*     16   Keyword token */
		printf("%s\n", kw_table[t.attribute.get_int]);
		break;
	case COM_T: /* 17   Comma token */
		printf("NA\n");
		break;
	case EOS_T: /*    18  End of statement *(semi - colon) */
		printf("NA\n");
		break;
	default:
		printf("PLATY: Scanner error: invalid token code: %d\n", t.code);
	}/* end switch*/
}

void gen_incode(char* code)
{
	printf(code);
}

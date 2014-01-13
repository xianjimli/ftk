/*
 * File:    expr.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   express eval
 *
 * Copyright (c) 2009 - 2010  Li XianJing <xianjimli@hotmail.com>
 *
 * Licensed under the Academic Free License version 2.1
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 * History:
 * ================================================================
 * 2009-07-17 Li XianJing <xianjimli@hotmail.com> created
 * 2010-10-02 Jiao JinXing <jiaojinxing1987@gmail.com> add rt-thread support.
 *
 */

#include <stdlib.h>
#ifndef RT_THREAD
#include <stdio.h>
#endif
#include <ctype.h>
#include <assert.h>
#include "ftk_util.h"

typedef enum tagToken
{
	TOK_NONE,   
	TOK_ADD,      //'+'
	TOK_OR,      //'|'
	TOK_AND,      //'&'
	TOK_SUB,      //'-'
	TOK_MULTI,    //'*'
	TOK_DIV,      //'/'
	TOK_LPAREN,   //'('
	TOK_RPAREN,   //')'
	TOK_NUM,      //number
	TOK_NR
}Token;

typedef struct tagLex
{
	const char*  buffer;
	size_t read_pos;
	
	double val;
	Token  token_type;
	int    b_unget_token;
}Lex, *PLex;

static double EvalExpr(PLex pLex);

static void LexGetNumToken(PLex thiz)
{
	enum
	{
		STAT_NONE,
		STAT_AFTER_0,
		STAT_AFTER_X,
		STAT_FINISH
	}state = STAT_NONE;

	thiz->val = 0;
	for(;thiz->buffer[thiz->read_pos] != '\0' && state != STAT_FINISH; thiz->read_pos++)
	{
		char c = thiz->buffer[thiz->read_pos];
		switch(state)
		{
			case STAT_NONE:
			{
				if(c == '0')
				{
					state = STAT_AFTER_0;
				}
				else
				{
					thiz->val = ftk_atof(thiz->buffer+thiz->read_pos);
					state = STAT_FINISH;
				}
				break;
			}
			case STAT_AFTER_0:
			{
				if(c == 'x' || c == 'X')
				{
					state = STAT_AFTER_X;
				}
				else if(c == '.')
				{
					thiz->val = ftk_atof(thiz->buffer+thiz->read_pos);
					state = STAT_FINISH;
				}
				else 
				{
					thiz->val = ftk_strtol(thiz->buffer+thiz->read_pos, NULL, 8);
					state = STAT_FINISH;
				}
				break;
			}
			case STAT_AFTER_X:
			{
				thiz->val = ftk_strtol(thiz->buffer+thiz->read_pos, NULL, 16);
				state = STAT_FINISH;
				break;
			}
			default:break;
		}
	}

	while(thiz->buffer[thiz->read_pos] != '\0' && 
		(isdigit(thiz->buffer[thiz->read_pos]) || thiz->buffer[thiz->read_pos] == '.'))
	{
		++thiz->read_pos;
	}

	return;
}

static Token LexGetToken(PLex thiz)
{
	assert(thiz != NULL && thiz->buffer != NULL);

	if(thiz == NULL || thiz->buffer == NULL || thiz->buffer[thiz->read_pos] == '\0')
	{
		thiz->token_type = TOK_NONE;
		
		return thiz->token_type;
	}

	if(thiz->b_unget_token)
	{
		thiz->b_unget_token = !thiz->b_unget_token;
		
		return thiz->token_type;
	}

	thiz->token_type = TOK_NONE;

	while(thiz->buffer[thiz->read_pos] != '\0')
	{
		switch(thiz->buffer[thiz->read_pos])
		{
			case '+':
			{
				thiz->token_type = TOK_ADD;
				break;
			}
			case '|':
			{
				thiz->token_type = TOK_OR;
				break;
			}
			case '&':
			{
				thiz->token_type = TOK_AND;
				break;
			}
			case '-':
			{
				thiz->token_type = TOK_SUB;
				break;
			}
			case '*':
			{
				thiz->token_type = TOK_MULTI;
				break;
			}
			case '/':
			{
				thiz->token_type = TOK_DIV;
				break;
			}
			case '(':
			{
				thiz->token_type = TOK_LPAREN;
				break;
			}
			case ')':
			{
				thiz->token_type = TOK_LPAREN;
				break;
			}
			default:
			{
				if(isdigit(thiz->buffer[thiz->read_pos]))
				{
					LexGetNumToken(thiz);
					thiz->token_type = TOK_NUM;
				}
				else
				{
					//skip invalid cahr.
				}
				break;
			}
		}

		if(thiz->token_type != TOK_NUM)
		{
			++thiz->read_pos;
		}

		if(thiz->token_type != TOK_NONE)
		{
			break;
		}
	}
	
	return thiz->token_type;
}

static void LexUngetToken(PLex thiz)
{
	assert(thiz != NULL && thiz->buffer != NULL);

	thiz->b_unget_token = !thiz->b_unget_token;

	return;
}

//<FACTOR> ::= ( <EXPR> ) | number
static double EvalFactor(PLex pLex)
{
	double val = 0;
	Token token_type = LexGetToken(pLex);
	
	switch(token_type)
	{
		case TOK_LPAREN:
		{
			val = EvalExpr(pLex);
			break;
		}
		case TOK_NUM:
		{
			val = pLex->val;
			break;
		}
		default:
		{
			//unexpected token.
			break;
		}
	}

	return val;
}

//<TERM>  ::= <FACTOR>  { <MULOP> <FACTOR> }
//<MULOP> ::= * | /
static double EvalTerm(PLex pLex)
{
	double val = EvalFactor(pLex);
	Token token_type = LexGetToken(pLex);
	
	while(token_type == TOK_MULTI || token_type == TOK_DIV)
	{
		if(token_type == TOK_MULTI)
		{
			val *= EvalFactor(pLex);
		}
		else
		{
			val /= EvalFactor(pLex);
		}
		token_type = LexGetToken(pLex);
	}

	LexUngetToken(pLex);
	
	return val;
}

//EXPR  ::= TERM { ADDOP TERM }
//ADDOP ::= + | -
static double EvalExpr(PLex pLex)
{
	double val = EvalTerm(pLex);
	Token token_type = LexGetToken(pLex);
	
	while(token_type == TOK_ADD || token_type == TOK_SUB
		|| token_type == TOK_OR || token_type == TOK_AND)
	{
		switch(token_type)
		{
			case TOK_ADD:
			{
				val += EvalTerm(pLex);
				break;
			}
			case TOK_SUB:
			{
				val -= EvalTerm(pLex);
				break;
			}
			case TOK_OR:
			{
				int value = (unsigned int)val | (unsigned int)EvalTerm(pLex);
				val = value;
				break;
			}
			case TOK_AND:
			{
				int value = (unsigned int)val & (unsigned int)EvalTerm(pLex);
				val = value;
				break;
			}
			default:break;
		}
		token_type = LexGetToken(pLex);
	}

	//LexUngetToken(pLex);

	return val;
}
		
double ftk_expr_eval(const char* expr)
{
	Lex aLex = {0};
	aLex.buffer = expr;
	aLex.read_pos = 0;

	return EvalExpr(&aLex);
}


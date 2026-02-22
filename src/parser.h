#ifndef PARSER_H
#define PARSER_H

#include "base.h"

#include "tokenizer.h"
#include "ast.h"

typedef struct {
	Allocator arena;
	Token_List tokens;
	usize token_index;
} Parser;

internal Parser parser(Token_List list, Allocator arena);
internal void parse_program(Parser *p);

#endif

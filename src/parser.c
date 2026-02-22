#include "parser.h"

/////////////////////
// ~geb: helpers

internal Token
_parser_peek(Parser *p) {
	return dyn_arr_index(&p->tokens, Token, p->token_index);
}

internal Token
_parser_advance(Parser *p) {
	return dyn_arr_index(&p->tokens, Token, p->token_index++);
}

internal void
_parser_expect(Parser *p, Token_Kind kind) {
	Token peek = _parser_peek(p);
	if(peek.kind != kind) {
		log_error("Parser expected token '%d' but got '%d'", kind, peek.kind);
		Trap();
	}
}

internal bool
_parser_match(Parser *p, Token_Kind kind)
{
	if (_parser_peek(p).kind == kind) {
		_parser_advance(p);
		return true;
	}
	return false;
}

/////////////////////

internal Parser
parser(Token_List list, Allocator arena)
{
	Parser p = {
		.tokens = list,
		.arena = arena,
		.token_index = 0,
	};
	return p;
}

internal void
parse_program(Parser *p)
{
}

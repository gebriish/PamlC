#include "tokenizer.h"
#include <math.h>

//////////////////////////
// ~geb: helpers

internal bool
is_digit_base(rune c, int base)
{
	if (base <= 10) return (c >= '0' && c < '0' + base);
	return (is_digit(c) ||
	(c >= 'a' && c < 'a' + (base - 10)) ||
	(c >= 'A' && c < 'A' + (base - 10)));
}

internal int
digit_value(rune c)
{
	if (is_digit(c)) return c - '0';
	if (c >= 'a') return 10 + (c - 'a');
	return 10 + (c - 'A');
}

//////////////////////////

internal bool
tok_init(Tokenizer *tok, String8 src)
{
	MemZeroStruct(tok);

	tok->src = src;
	tok->pos = 0;
	tok->row = 1;
	tok->col = 1;
	return true;
}


internal rune
tok_peek(Tokenizer *tok)
{
	if (tok->pos >= tok->src.len) return cast(rune) 0;
	return cast(rune) tok->src.str[tok->pos];
}

internal rune
tok_next(Tokenizer *tok)
{
	if (tok->pos >= tok->src.len) return cast(rune) 0;

	UTF8_Error err = 0;
	rune cp = utf8_decode(tok->src.str + tok->pos++, &err);
	Assert(err == 0);

	if (cp == cast(rune) '\n') {
		tok->row += 1;
		tok->col =  1;
	} else {
		tok->col += 1;
	}

	return cp;
}


internal Token
tok_next_token(Tokenizer *tok)
{
	while (is_space(tok_peek(tok)) && tok_peek(tok) != '\n')
		tok_next(tok);

	u32 token_row = tok->row;
	u32 token_col = tok->col;
	usize start = tok->pos;

	rune c = tok_next(tok);

	if (!c) return (Token){ .kind = Tok_Eof, .row = token_row, .col = token_col };

	if (c == '/' && tok_peek(tok) == '/') {
		while (tok_peek(tok) && tok_peek(tok) != '\n')
			tok_next(tok);
		return tok_next_token(tok);
	}

	if (is_digit(c)) {
		int base = 10;
		bool is_float = false;

		if (c == '0') {
			rune p = tok_peek(tok);
			if      (p == 'x') { base = 16; tok_next(tok); }
			else if (p == 'b') { base = 2;  tok_next(tok); }
			else if (p == 'o') { base = 8;  tok_next(tok); }
		}

		u64 int_part = digit_value(c);
		bool last_underscore = false;

		while (is_digit_base(tok_peek(tok), base) || tok_peek(tok) == '_') {
			rune d = tok_next(tok);
			if (d == '_') {
				if (last_underscore) break;
				last_underscore = true;
				continue;
			}
			last_underscore = false;
			int v = digit_value(d);
			if (int_part > (UINT64_MAX - v) / base)
				return (Token){ .kind = Tok_Error, .row = tok->row, .col = tok->col };
			int_part = int_part * base + v;
		}

		if (last_underscore) return (Token) { 
			.kind = Tok_Error,
			.text = str8_slice(tok->src, start, tok->pos),
			.row = tok->row, .col = tok->col
		};

		f64 value = (f64)int_part;

		if (base == 10 && tok_peek(tok) == '.') {
			is_float = true;
			tok_next(tok);
			f64 frac = 0.0;
			f64 scale = 1.0;
			last_underscore = false;

			while (is_digit(tok_peek(tok)) || tok_peek(tok) == '_') {
				rune d = tok_next(tok);
				if (d == '_') {
					if (last_underscore) break;
					last_underscore = true;
					continue;
				}
				last_underscore = false;
				scale *= 10.0;
				frac += (d - '0') / scale;
			}

			if (last_underscore) return (Token) {
				.kind = Tok_Error ,
				.text = str8_slice(tok->src, start, tok->pos),
				.row = tok->row, .col = tok->col
			};

			value += frac;
		}

		if (base == 10 && (tok_peek(tok) == 'e' || tok_peek(tok) == 'E')) {
			is_float = true;
			tok_next(tok);
			int sign = 1;
			if (tok_peek(tok) == '+') tok_next(tok);
			else if (tok_peek(tok) == '-') { sign = -1; tok_next(tok); }

			int exp = 0;
			while (is_digit(tok_peek(tok)))
				exp = exp * 10 + (tok_next(tok) - '0');

			f64 scale = 1.0;
			for (int i = 0; i < exp; i++)
				scale *= 10.0;

			if (sign < 0) value /= scale;
			else value *= scale;
		}

		String8 text = str8_slice(tok->src, start, tok->pos);

		if (is_float)
			return (Token){ .kind = Tok_Decimal, .text = text, .float_value = value, .row = token_row, .col = token_col };
		else
			return (Token){ .kind = Tok_Integral, .text = text, .int_value = int_part, .row = token_row, .col = token_col };
	}

	if (is_letter(c)) {
		while (is_letter(tok_peek(tok)) || is_digit(tok_peek(tok)))
			tok_next(tok);

		String8 lex = str8_slice(tok->src, start, tok->pos);

		if (str8_equal(lex, S("fn")))     return (Token){ .kind = Tok_Keyword_Fn, .row = token_row, .col = token_col };
		if (str8_equal(lex, S("return"))) return (Token){ .kind = Tok_Keyword_Ret, .row = token_row, .col = token_col };

		return (Token){ .kind = Tok_Ident, .text = lex, .row = token_row, .col = token_col };
	}

	Token token = {0};
	switch (c) {
		case '+' : token = (Token){ .kind = Tok_Plus }; break;
		case '-' : token = (Token){ .kind = Tok_Minus }; break;
		case '*' : token = (Token){ .kind = Tok_Star }; break;
		case '/' : token = (Token){ .kind = Tok_Slash }; break;
		case '(' : token = (Token){ .kind = Tok_LParen }; break;
		case ')' : token = (Token){ .kind = Tok_RParen }; break;
		case '{' : token = (Token){ .kind = Tok_LBraces }; break;
		case '}' : token = (Token){ .kind = Tok_RBraces }; break;
		case '[' : token = (Token){ .kind = Tok_LBracket }; break;
		case ']' : token = (Token){ .kind = Tok_RBracket }; break;
		case '=' : token = (Token){ .kind = Tok_Eq }; break;
		case ':' : token = (Token){ .kind = Tok_Colon }; break;
		case ';' : token = (Token){ .kind = Tok_SemiColon }; break;
		case '.' : token = (Token){ .kind = Tok_Dot }; break;
		case ',' : token = (Token){ .kind = Tok_Comma }; break;
		case '|' : token = (Token){ .kind = Tok_Pipe }; break;
		case '<' : token = (Token){ .kind = Tok_LAngle }; break;
		case '>' : token = (Token){ .kind = Tok_RAngle }; break;
		case '&' : token = (Token){ .kind = Tok_And }; break;
		case '\'': token = (Token){ .kind = Tok_Quote }; break;
		case '\"': token = (Token){ .kind = Tok_DQuote }; break;
		case '\n': token = (Token){ .kind = Tok_Newline }; break;
	}
	if (token.kind != 0) {
		token.row = token_row;
		token.col = token_col;
		return token;
	}

	return (Token){ .kind = Tok_Error, .row = token_row, .col = token_col };
}


internal Token_List
token_list(Allocator alloc, usize initial_capacity)
{
	Token_List list = {0};

	list.alloc = alloc;
	Alloc_Error error = 0;

	list.array = alloc_array(alloc, Token, initial_capacity, &error); Assert(error == Alloc_Err_None);
	list.len = 0;
	list.capacity = initial_capacity;

	return list;
}

internal Alloc_Error
token_list_append(Token_List *list, Token token)
{
	if (list->len >= list->capacity) {
		Alloc_Error err = 0;
		usize new_capacity = list->capacity == 0 ? 32 : list->capacity * 2;
		
		list->array = mem_resize(
			list->alloc, 
			list->array, 
			list->capacity * sizeof(Token), 
			new_capacity * sizeof(Token),
			false,
			&err
		);
		
		if (err) { 
			printf("error(%d)\n", err);
			return err; 
		}
		
		list->capacity = new_capacity;
	}

	list->array[list->len] = token;
	list->len += 1;

	return Alloc_Err_None;
}

internal void
token_list_clear(Token_List *list)
{
	list->len = 0;
}





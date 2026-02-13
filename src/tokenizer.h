#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "base.h"

typedef u32 Token_Kind;
enum {
	Tok_Eof,
	Tok_Error,
	Tok_Newline,

	Tok_Integral,
	Tok_Decimal,
	Tok_Ident,
	
	Tok_Plus,     // +
	Tok_Minus,    // -
	Tok_Star,     // *
	Tok_Slash,    // /

	Tok_Eq,       // =
	Tok_Pipe,     // |

	Tok_Colon,    // :
	Tok_SemiColon,// ;
	Tok_LParen,   // (
	Tok_RParen,   // (
	Tok_LBracket, // [
	Tok_RBracket, // ]
	Tok_LAngle,   // <
	Tok_RAngle,   // >
	Tok_LBraces,  // {
	Tok_RBraces,  // }
	Tok_And,      // &
	
	Tok_Quote,    // '
	Tok_DQuote,   // "

	Tok_Dot,      // .
	Tok_Comma,    // ,
	

	// keywords

	Tok_Keyword_Fn,  // fn
	Tok_Keyword_Ret, // return

	Tok_Count,
};

global const String8
TOKEN_STRING[Tok_Count] = {
	[Tok_Eof]      = S(""),
	[Tok_Error]    = S(""),
	[Tok_Newline]  = S("\\n"),

	[Tok_Integral] = S(""),
	[Tok_Decimal]  = S(""),
	[Tok_Ident]    = S(""),

	[Tok_Plus]     = S("+"), 
	[Tok_Minus]    = S("-"),
	[Tok_Star]     = S("*"),
	[Tok_Slash]    = S("/"),

	[Tok_Eq]       = S("="),
	[Tok_Pipe]     = S("|"),

	[Tok_Colon]    = S(":"),
	[Tok_SemiColon]= S(";"),
	[Tok_LParen]   = S("("),  
	[Tok_RParen]   = S(")"),  
	[Tok_LBracket] = S("["),
	[Tok_RBracket] = S("]"),
	[Tok_LBraces]  = S("{"),
	[Tok_RBraces]  = S("}"),
	[Tok_LAngle]   = S("<"),
	[Tok_RAngle]   = S(">"),
	[Tok_Dot]      = S("."),
	[Tok_Comma]    = S(","),
	[Tok_And]      = S("&"),

	[Tok_Quote]      = S("\'"),
	[Tok_DQuote]     = S("\""),

	[Tok_Keyword_Fn]  = S("fn"),
	[Tok_Keyword_Ret] = S("return"),
};

typedef struct {
	Token_Kind kind;
	String8    text;
	union {
		f64 float_value;
		u64 int_value;
	};
	usize      row, col;
} Token;

typedef struct {
	String8 src;
	usize pos;

	usize col, row;
} Tokenizer;

typedef struct {
	Allocator alloc;
	Token *array;
	usize  len;
	usize  capacity;
} Token_List;

internal bool tok_init(Tokenizer *tok, String8 src);

internal rune tok_peek(Tokenizer *tok);
internal rune tok_next(Tokenizer *tok);

internal Token tok_next_token(Tokenizer *tok);

internal Token_List  token_list(Allocator alloc, usize initial_capacity);
internal Alloc_Error token_list_append(Token_List *list, Token token);
internal void        token_list_clear(Token_List *list);

#endif

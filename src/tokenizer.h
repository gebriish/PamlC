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
	Tok_String,
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

	Tok_Dot,      // .
	Tok_Comma,    // ,

	// keywords
	
	Tok_Keyword_Fn,  // fn
	Tok_Keyword_Ret, // return
	Tok_Keyword_Var,

	Tok_Count,
};


#if COMPILER_MSVC
// WHYYY, JUST WHYY MICROSOFT!!!!!!!!!
# define _string_macro(x)  { .len = cast(usize) sizeof(x) - 1, .str = cast(u8 *) x }
#else
# define _string_macro(x) S(x)
#endif

global const String8
TOKEN_STRING[Tok_Count] = {
	[Tok_Eof]         = _string_macro(""),
	[Tok_Error]       = _string_macro(""),
	[Tok_Newline]     = _string_macro("\\n"),

	[Tok_Integral]    = _string_macro(""),
	[Tok_Decimal]     = _string_macro(""),
	[Tok_Ident]       = _string_macro(""),

	[Tok_Plus]        = _string_macro("+"), 
	[Tok_Minus]       = _string_macro("-"),
	[Tok_Star]        = _string_macro("*"),
	[Tok_Slash]       = _string_macro("/"),

	[Tok_Eq]          = _string_macro("="),
	[Tok_Pipe]        = _string_macro("|"),

	[Tok_Colon]       = _string_macro(":"),
	[Tok_SemiColon]   = _string_macro(";"),
	[Tok_LParen]      = _string_macro("("),  
	[Tok_RParen]      = _string_macro(")"),  
	[Tok_LBracket]    = _string_macro("["),
	[Tok_RBracket]    = _string_macro("]"),
	[Tok_LBraces]     = _string_macro("{"),
	[Tok_RBraces]     = _string_macro("}"),
	[Tok_LAngle]      = _string_macro("<"),
	[Tok_RAngle]      = _string_macro(">"),
	[Tok_Dot]         = _string_macro("."),
	[Tok_Comma]       = _string_macro(","),
	[Tok_And]         = _string_macro("&"),

	[Tok_Keyword_Fn]  = _string_macro("fn"),
	[Tok_Keyword_Ret] = _string_macro("return"),
	[Tok_Keyword_Var] = _string_macro("var"),
};
#undef _string_macro

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

typedef Dynamic_Array Token_List;

internal bool tok_init(Tokenizer *tok, String8 src);

internal rune tok_peek(Tokenizer *tok);
internal rune tok_next(Tokenizer *tok);

internal Token tok_next_token(Tokenizer *tok);
internal Token_List tokenize_source(String8 source, Allocator arena);

#endif

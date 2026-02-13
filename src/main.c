#include "base.h"
#include "base.c"

#include "tokenizer.h"
#include "tokenizer.c"

#define PAMLC_VERSION "debug_build"

internal void
print_usage(void)
{
	fprintf(stderr,
		"PamlC - PAML Compiler\n"
		"\tbuild           : Compiles the first paml file of a project into executable.\n"
		"\t                  Must provide the source file as next argument.\n"
		"\t                  The first file should contain the main function.\n"
		"\tversion         : Prints compiler version.\n"
		"\thelp            : Pull up help page.\n"
	);
}

internal String8
args_get_output_name(String8_List args)
{
	for (usize i=2; i<args.size; ++i) {
		String8 arg = args.array[i];
		if (str8_equal(arg, S("-o"))) {
			if (i + 1 >= args.size) { return S(""); }
			return args.array[i + 1];
		}
	} 

	return S("");
}

internal Token_List
tokenize_source_file(String8 file_source, Allocator arena)
{
	Tokenizer tok;
	tok_init(&tok, file_source);

	Token_List tok_list = token_list(arena, 16);

	for (;;) {
		Token t = tok_next_token(&tok);
		if (t.kind == Tok_Eof) { break; }
		token_list_append(&tok_list, t);
	}
	
	return tok_list;
}

internal void
error(const char *fmt, ...) {

}

int main(int argc, const char **argv)
{
	Allocator arena   = arena_allocator(Mb(512));

	String8_List args = str8_make_list(argv, cast(usize)argc, arena);

	if (args.size < 2) {
		print_usage();
		return 1;
	}

	String8 cmd = args.array[1];

	if (args.size == 2) {
		if (str8_equal(cmd, S("version")) || str8_equal(cmd, S("-v"))) {
			printf("PamlC - Paml Compiler version %s\n", PAMLC_VERSION);
			return 0;
		}

		if (str8_equal(cmd, S("help")) || str8_equal(cmd, S("-h"))) {
			print_usage();
			return 0;
		}
	}

	if (str8_equal(cmd, S("build"))) {
		//////////////////////////////////////////
		// ~geb: Main Compiler Flow

		OS_Time_Stamp begin = os_time_now();

		if (args.size < 3) {
			fprintf(stderr, "error: no input file provided\n");
			return 1;
		}

		String8 file_path = args.array[2];
		String8 file_ext  = str8_file_extension(file_path);

		if (!str8_equal(file_ext, S("paml"))) {
			fprintf(stderr, "error: invalid file type '"STR"'\n", s_fmt(file_ext));
			return 1;
		}

		String8 out_name  = args_get_output_name(args);
		if (!out_name.len) {
			out_name = str8_file_name(file_path);
		}

		String8 src = os_data_from_path(file_path, arena, arena);

		if (src.len == 0) {
			fprintf(stderr,
				"error: file '"STR"' empty or missing.\n",
				s_fmt(file_path));
			return 1;
		}

		Token_List tokens = tokenize_source_file(src, arena);

		for (usize i=0; i<tokens.len; ++i) {
			Token token = tokens.array[i];

			if (token.kind == Tok_Newline) {
				printf("\n"); continue;
			}

			String8 lexeme = token.text;
			if (!lexeme.len) {
				lexeme = TOKEN_STRING[token.kind];
			}

			printf(STR " ", s_fmt(lexeme));
		}

		OS_Time_Duration delta = os_time_diff(begin, os_time_now());
		printf("compile time : %f ms.\n", delta.milliseconds);

		return 0;

		//////////////////////////////////////////
	}

	fprintf(stderr,
		"error: unknown command '"STR"'\n",
		s_fmt(cmd));
	fprintf(stderr, "hint: use 'pamlc help'\n");
	return 1;
}

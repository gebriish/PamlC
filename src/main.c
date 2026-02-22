#include "base.h"
#include "tokenizer.h"
#include "ast.h"
#include "parser.h"

#include "base.c"
#include "tokenizer.c"
#include "parser.c"
#include "ast.c"


#define PAMLC_VERSION "debug_build"

typedef struct {
	String8 out;
} Build_Config;


internal void
print_usage()
{
	fprintf(stdout,
		"PamlC - PAML Compiler\n"
		"\tbuild           : Compiles the first paml file of a project into executable.\n"
		"\t                  Must provide the source file as next argument.\n"
		"\t                  The first file should contain the main function.\n"
		"\tversion         : Prints compiler version.\n"
		"\thelp            : Pull up help page.\n"
	);
}

int main(int argc, const char **argv)
{
	Allocator arena   = arena_allocator(Mb(512));

	String8_List args = str8_make_list(argv, cast(usize)argc, arena);

	if (args.len < 2) {
		print_usage();
		return 1;
	}

	String8 cmd = str8_list_index(&args, 1);

	if (args.len == 2) {
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

		if (args.len < 3) {
			fprintf(stderr, "error: no input file provided\n");
			print_usage();
			return 1;
		}
	
		Build_Config build_cfg = {0};

		for (usize i = 2; i < args.len; ++i) {
			String8 arg = dyn_arr_index(&args, String8, i);

			switch (arg.len) {
				case 2:
					if (str8_equal(arg, S("-o"))) {
						if (i + 1 >= args.len) {
							fprintf(stderr, "error: output name not specified\n");
							return 1;
						}

						build_cfg.out = str8_list_index(&args, i + 1);
					}
			}
		}

		//////////////////////////////////////////
		// ~geb: Main Compiler Flow

		OS_Time_Stamp begin = os_time_now();

		String8 file_path = str8_list_index(&args, 2);
		String8 file_ext  = str8_file_extension(file_path);

		if (!str8_equal(file_ext, S("paml"))) {
			fprintf(stdout, "error: invalid file type '"STR"'\n", s_fmt(file_ext));
			return 1;
		}

		String8 src = os_data_from_path(file_path, arena);

		if (src.len == 0) {
			fprintf(stdout,
		   "error: file '"STR"' empty or missing.\n",
		   s_fmt(file_path));
			return 1;
		}

		Token_List tokens = tokenize_source(src, arena);

		Token *token_array = cast(Token *) tokens.data;
		for (usize i=0; i<tokens.len; ++i) {
			Token token = token_array[i];

			String8 lexeme = token.text;
			if (!lexeme.len) {
				lexeme = TOKEN_STRING[token.kind];
			}

			printf("\'" STR "\', ", s_fmt(lexeme));
		}
		printf("\n");

		OS_Time_Duration delta = os_time_diff(begin, os_time_now());
		printf("compile time : %f ms.\n", delta.milliseconds);

		return 0;

		//////////////////////////////////////////
	}

	fprintf(stdout,
		"error: unknown command '"STR"'\n",
		s_fmt(cmd));
	fprintf(stdout, "hint: use 'pamlc help'\n");
	return 1;
}

#ifndef AST_H
#define AST_H

#include "base.h"

typedef u32 Ast_Kind;
enum {
	Ast_Program,
};

typedef struct Ast_Node Ast_Node;
struct Ast_Node {
	Ast_Kind kind;	

	Ast_Node *next;

	union { // children
		Ast_Node *decls;
		Ast_Node *value;
	};
};

#endif

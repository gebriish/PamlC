#include "ast.h"


internal Ast_Node *
ast_node(Ast_Kind kind, Allocator arena)
{
	Alloc_Error err = 0;
	Ast_Node *node = alloc(arena, Ast_Node, &err);

	if (err) {
		log_error("failed to allocate AST_Node, err(%d)", err);
		Trap();
	}

	return node;
}

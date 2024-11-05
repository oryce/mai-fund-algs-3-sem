#include "task.h"

#include <stdlib.h>

#include "lib/chars.h"

node_t* node_create(void) {
	node_t* node = (node_t*)malloc(sizeof(node_t));
	if (!node) return NULL;

	if (!string_create(&node->value)) {
		free(node);
		return NULL;
	}

	node->sibling = NULL;
	node->child = NULL;

	return node;
}

void node_destroy(node_t* node) {
	if (!node) return;

	string_destroy(&node->value);

	if (node->child) {
		node_destroy(node->child);
		node->child = NULL;
	}
	if (node->sibling) {
		node_destroy(node->sibling);
		node->sibling = NULL;
	}

	free(node);
}

void node_print0(FILE* fp, node_t* node, unsigned level) {
	if (!node) return;

	for (unsigned i = 0; i != level; ++i) {
		fprintf(fp, " ");
	}

	fprintf(fp, "%s\n", string_to_c_str(&node->value));

	if (node->child) node_print0(fp, node->child, level + 1);
	if (node->sibling) node_print0(fp, node->sibling, level);
}

void node_print(FILE* fp, node_t* node) { node_print0(fp, node, 0); }

node_t* tree_from_expr_cleanup(node_t* root, node_t* current) {
	if (current && current != root) node_destroy(current);
	if (root) node_destroy(root);

	return NULL;
}

node_t* tree_from_expr0(const char** ptr) {
	if (!ptr) return NULL;

	node_t* root = node_create();
	if (!root) return NULL;

	node_t* previous = NULL;
	node_t* current = root;

	while (**ptr != '\0' && **ptr != ')') {
		if (**ptr == '(') {
			// Read a child node.
			++(*ptr);

			current->child = tree_from_expr0(ptr);
			if (!current->child) return tree_from_expr_cleanup(root, current);

			if (**ptr == ')') {
				++(*ptr);
				continue;
			} else {
				// Ended with a '\0', meaning there wasn't a matching closing
				// bracket.
				return tree_from_expr_cleanup(root, current);
			}
		} else if (**ptr == ',') {
			if (previous) {
				previous->sibling = current;
			}

			previous = current;
			current = node_create();

			if (!current) return tree_from_expr_cleanup(root, current);
		} else if (chars_is_alpha(**ptr)) {
			if (!string_append_char(&current->value, **ptr))
				return tree_from_expr_cleanup(root, current);
		}

		++(*ptr);
	}

	if (previous) {
		previous->sibling = current;
	}

	return root;
}

node_t* tree_from_expr(const char* expr) {
	if (!expr || *expr == '\0') return NULL;
	return tree_from_expr0(&expr);
}

#include "task.h"

#include <ctype.h>

#include "lib/collections/string.h"

int word_cmp(const char* a, const char* b) {
	if (!a || !b || a == b) return 0;

	int order = (int)strlen(a) - (int)strlen(b);
	if (order) return order;

	return strcmp(a, b);
}

char* str_lower(const char* str) {
	if (!str) return NULL;

	char* lower = strdup(str);
	if (!lower) return NULL;

	for (char* p = lower; *p != '\0'; ++p) {
		*p = (char)tolower(*p);
	}

	return lower;
}

node_t* node_create0(char* word) {
	if (!word) return NULL;

	node_t* node = (node_t*)calloc(1, sizeof(node_t));
	if (!node) return NULL;

	node->word = word;
	node->occurrences = 1;

	return node;
}

node_t* node_create(const char* word) {
	if (!word) return NULL;

	// Use a heap-allocated lowercase word.
	char* lower = str_lower(word);
	if (!lower) return NULL;

	node_t* node = node_create0(lower);
	if (!node) free(lower);

	return node;
}

void node_destroy(node_t* node) {
	if (!node) return;

	free(node->word);

	if (node->left) node_destroy(node->left);
	if (node->right) node_destroy(node->right);

	free(node);
}

typedef enum insert_result {
	INSERT_FAILED,
	INSERT_ACCOUNTED,
	INSERT_ADDED
} insert_result_t;

insert_result_t node_insert0(node_t** root, char* word) {
	if (!root || !word) return INSERT_FAILED;

	if (!(*root)) {
		*root = node_create0(word);
		return *root == NULL ? INSERT_FAILED : INSERT_ADDED;
	} else {
		int order = word_cmp(word, (*root)->word);

		if (order == 0) {
			++(*root)->occurrences;
			return INSERT_ACCOUNTED;
		} else if (order < 0) {
			return node_insert0(&(*root)->left, word);
		} else {
			return node_insert0(&(*root)->right, word);
		}
	}
}

bool node_insert(node_t** root, const char* word) {
	// Use a heap-allocated lowercase word.
	char* lower = str_lower(word);
	if (!lower) return false;

	insert_result_t result = node_insert0(root, lower);
	if (result != INSERT_ADDED) free(lower);

	return result;
}

bool node_write(const node_t* node, FILE* outFp) {
	if (!node) {
		ssize_t length = -1;
		return fwrite(&length, sizeof(ssize_t), 1, outFp) == 1;
	}

	ssize_t len = (ssize_t)strlen(node->word);

	// Write the word length.
	bool result = fwrite(&len, sizeof(ssize_t), 1, outFp) == 1;
	// Write occurrences.
	result &= fwrite(&node->occurrences, sizeof(unsigned long), 1, outFp) == 1;
	// Write the word without a null-terminator.
	result &= !len || fwrite(node->word, sizeof(char), len, outFp) == len;

	return result && node_write(node->left, outFp) &&
	       node_write(node->right, outFp);
}

bool node_read(node_t** node, FILE* inFp) {
	if (!node) return false;

	// Read word length. If a -1 is read, the tree node is a NULL.
	ssize_t length;
	if (fread(&length, sizeof(ssize_t), 1, inFp) != 1) {
		return false;
	}

	if (length == -1) {
		*node = NULL;
		return true;
	}

	// Read occurrences.
	unsigned long occurrences;
	if (fread(&occurrences, sizeof(occurrences), 1, inFp) != 1) {
		return false;
	}

	// Allocate |length + 1| for the word + null-terminator.
	char* word = (char*)malloc((length + 1) * sizeof(char));
	if (!word) return false;

	// Read the word without the null-terminator.
	if (fread(word, sizeof(char), length, inFp) != length) {
		free(word);
		return false;
	}

	word[length] = '\0';
	*node = node_create0(word);

	if (!(*node)) {
		free(word);
		return false;
	}

	(*node)->occurrences = occurrences;

	if (!(node_read(&(*node)->left, inFp) &&
	      node_read(&(*node)->right, inFp))) {
		node_destroy(*node);
		return false;
	}

	return true;
}

void node_print0(const node_t* node, unsigned level) {
	if (!node) return;

	if (node->right) {
		node_print0(node->right, level + 1);
	}

	for (unsigned i = 0; i < level; ++i) {
		printf("\t");
	}

	printf("%s\n", node->word);

	if (node->left) {
		node_print0(node->left, level + 1);
	}
}

void node_print(const node_t* node) {
	if (!node) return;
	node_print0(node, 0);
}

unsigned node_depth(const node_t* node) {
	if (!node) return 0;

	unsigned left = node_depth(node->left);
	unsigned right = node_depth(node->right);

	return (left > right ? left : right) + 1;
}

unsigned long word_occurrences0(const node_t* root, const char* word) {
	if (!root || !word) return 0;

	int order = word_cmp(word, root->word);

	if (order == 0) {
		return root->occurrences;
	} else if (order < 0) {
		return word_occurrences0(root->left, word);
	} else {
		return word_occurrences0(root->right, word);
	}
}

unsigned long word_tree_occurrences(const node_t* root, const char* word) {
	// Use a heap-allocated lowercase word.
	char* lower = str_lower(word);
	if (!lower) return 0;

	unsigned long count = word_occurrences0(root, lower);
	free(lower);

	return count;
}

node_t* word_tree_fail(node_t* root, string_t* word) {
	node_destroy(root);
	string_destroy(word);
	return NULL;
}

node_t* word_tree_from_file(FILE* inputFile, char sep) {
	node_t* root = NULL;

	string_t word;
	if (!string_create(&word)) return NULL;

	int ch;
	while ((ch = fgetc(inputFile)) > 0) {
		if (ch == sep) {
			if (!string_length(&word)) continue;

			// We reached a separator and have a word in our buffer, so
			// (1) push the word into the tree, (2) strip the line separators
			// and (3) clear the word buffer, preparing for another read.
			if (!string_strip(&word, "\r\n") ||
			    !node_insert(&root, string_to_c_str(&word)) ||
			    !string_clear(&word)) {
				return word_tree_fail(root, &word);
			}

			continue;
		} else {
			if (!string_append_char(&word, (char)ch))
				return word_tree_fail(root, &word);
		}
	}

	if (ferror(inputFile)) {
		return word_tree_fail(root, &word);
	}

	// If we've reached an EOF and have an incomplete word, insert it into the
	// tree.
	if (string_length(&word)) {
		if (!string_strip(&word, "\r\n") ||
		    !node_insert(&root, string_to_c_str(&word))) {
			return word_tree_fail(root, &word);
		}
		string_destroy(&word);
	}

	return root;
}

const char* word_tree_shortest(const node_t* root) {
	if (!root) return NULL;
	return root->left ? word_tree_shortest(root->left) : root->word;
}

const char* word_tree_longest(const node_t* root) {
	if (!root) return NULL;
	return root->right ? word_tree_longest(root->right) : root->word;
}

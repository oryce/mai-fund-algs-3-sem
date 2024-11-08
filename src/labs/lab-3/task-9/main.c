#include <stdlib.h>
#include <string.h>

#include "lib/convert.h"
#include "task.h"

#define INVALID_COMMAND "Unknown or incomplete command. Type 'h' for help.\n"

#define CHECK_TREE(tree)                             \
	do {                                             \
		if (!tree) {                                 \
			fprintf(stderr, "Tree is not built.\n"); \
			return;                                  \
		}                                            \
	} while (0);

void cmd_build_tree(FILE* input, char sep, node_t** tree) {
	if (!tree) return;

	if (*tree) {
		printf("Tree is already built -- rebuilding.\n");
		node_destroy(*tree);
	}

	fseek(input, 0, SEEK_SET);
	*tree = word_tree_from_file(input, sep);

	if (*tree) {
		printf("OK.\n");
	} else {
		fprintf(stderr, "Can't read the tree.\n");
	}
}

void cmd_word_count(const node_t* tree, const char* word) {
	CHECK_TREE(tree);

	unsigned long count = word_tree_occurrences(tree, word);
	printf("%s -- %ld\n", word, count);
}

void cmd_top_n_words(const node_t* tree, size_t n) {
	CHECK_TREE(tree);

	const node_t** topWords = NULL;
	size_t nTopWords;

	if (!word_tree_top_words(tree, n, &topWords, &nTopWords)) {
		free(topWords);

		fprintf(stderr, "Catastrophic failure.\n");
		return;
	}

	for (size_t i = 0; i != nTopWords; ++i) {
		printf("%s -- %zu\n", topWords[i]->word, topWords[i]->occurrences);
	}

	free(topWords);
}

void cmd_shortest_word(const node_t* tree) {
	CHECK_TREE(tree);

	printf("Shortest word: %s\n", word_tree_shortest(tree));
}

void cmd_longest_word(const node_t* tree) {
	CHECK_TREE(tree);

	printf("Longest word: %s\n", word_tree_longest(tree));
}

void cmd_tree_read(node_t** tree, const char* input) {
	if (*tree) {
		node_destroy(*tree);
	}

	FILE* inputFile = fopen(input, "r");
	if (!inputFile) {
		fprintf(stderr, "Can't open the input file for reading.\n");
		return;
	}

	if (node_read(tree, inputFile)) {
		printf("OK.\n");
	} else {
		fprintf(stderr, "Can't read the tree.\n");
	}
}

void cmd_tree_write(const node_t* tree, const char* output) {
	CHECK_TREE(tree);

	FILE* outputFile = fopen(output, "w");
	if (!outputFile) {
		fprintf(stderr, "Can't open the output file for writing.\n");
		return;
	}

	if (node_write(tree, outputFile) && fflush(outputFile) == 0) {
		printf("OK.\n");
	} else {
		fprintf(stderr, "Can't write the tree.\n");
	}
}

void cmd_tree_depth(const node_t* tree) {
	CHECK_TREE(tree);

	printf("Tree depth: %d\n", node_depth(tree));
}

void cmd_tree_print(const node_t* tree) {
	CHECK_TREE(tree);

	node_print(tree);
}

void cmd_loop(FILE* input, char* separators, size_t nTrees, node_t** trees) {
	printf("Input a command ('h' for help)\n");

	char* line = NULL;
	size_t size;

	while (getline(&line, &size, stdin) > 0) {
		char* newline = strrchr(line, '\n');
		if (newline) *newline = '\0';

		// Read the tree index if it's not a help message.
		size_t idx;

		if (line[0] != 'h') {
			char inIdx[65];
			sscanf(line, "%*c %64s", inIdx);

			if (str_to_ulong(inIdx, &idx)) {
				fprintf(stderr, INVALID_COMMAND);
				continue;
			}
			if (idx > nTrees) {
				fprintf(stderr, "Tree index out of range.\n");
				continue;
			}
		}

		switch (line[0]) {
			case 'h':
				printf(
				    "Command help (<i> specifies the sep. index):\n"
				    "  b <i>\n"
				    "      Builds a word tree.\n"
				    "  o <i> <tree file>\n"
				    "      Opens the word tree from <tree file>.\n"
				    "  w <i> <tree file>\n"
				    "      Writes the word tree to <tree file>.\n"
				    "  c <i> <word>\n"
				    "      Prints the amount of occurrences for the word.\n"
				    "  t <i> <n>\n"
				    "      Prints the top-N most occurring words.\n"
				    "  s <i>\n"
				    "      Prints the shortest word.\n"
				    "  l <i>\n"
				    "      Prints the longest word.\n"
				    "  d <i>\n"
				    "      Prints the tree depth.\n"
				    "  p <i>\n"
				    "      Prints the tree.\n");
				break;
			case 'b':
				cmd_build_tree(input, separators[idx], &trees[idx]);
				break;
			case 'o': {
				char inInputPath[1024];

				if (sscanf(line, "o %*zu %1023s", inInputPath) != 1) {
					fprintf(stderr, INVALID_COMMAND);
					break;
				}

				cmd_tree_read(&trees[idx], inInputPath);
				break;
			}
			case 'w': {
				char inOutputPath[1024];

				if (sscanf(line, "w %*zu %1023s", inOutputPath) != 1) {
					fprintf(stderr, INVALID_COMMAND);
					break;
				}

				cmd_tree_write(trees[idx], inOutputPath);
				break;
			}
			case 'c': {
				char inWord[1024];

				if (sscanf(line, "c %*zu %1023s", inWord) != 1) {
					fprintf(stderr, INVALID_COMMAND);
					break;
				}

				cmd_word_count(trees[idx], inWord);
				break;
			}
			case 't': {
				char inN[128];
				size_t n;

				if (sscanf(line, "t %*zu %127s", inN) != 1 ||
				    str_to_ulong(inN, &n)) {
					fprintf(stderr, INVALID_COMMAND);
					break;
				}

				cmd_top_n_words(trees[idx], n);
				break;
			}
			case 's':
				cmd_shortest_word(trees[idx]);
				break;
			case 'l':
				cmd_longest_word(trees[idx]);
				break;
			case 'd':
				cmd_tree_depth(trees[idx]);
				break;
			case 'p':
				cmd_tree_print(trees[idx]);
				break;
		}
	}

	free(line);
}

int main_cleanup(int ret, FILE* inputFile, char* separators, size_t nTrees,
                 node_t** trees) {
	fclose(inputFile);
	free(separators);

	if (trees) {
		for (size_t i = 0; i != nTrees; ++i) {
			node_destroy(trees[i]);
		}
		free(trees);
	}

	return ret;
}

int main(int argc, char** argv) {
	if (argc < 3) {
		fprintf(stderr, "usage: %s <input file> <separators...>", argv[0]);
		return 1;
	}

	FILE* inputFile = NULL;
	char* separators = NULL;
	size_t nTrees = argc - 2;
	node_t** trees = NULL;

	inputFile = fopen(argv[1], "r");
	if (!inputFile) {
		fprintf(stderr, "Can't open input file for reading.\n");
		return main_cleanup(2, inputFile, separators, nTrees, trees);
	}

	separators = (char*)malloc(nTrees * sizeof(char));
	if (!separators) {
		fprintf(stderr, "Can't allocate for word separators.\n");
		return main_cleanup(3, inputFile, separators, nTrees, trees);
	}

	trees = (node_t**)calloc(nTrees, sizeof(node_t*));
	if (!trees) {
		fprintf(stderr, "Can't allocate for word trees.\n");
		return main_cleanup(4, inputFile, separators, nTrees, trees);
	}

	for (size_t i = 0; i != nTrees; ++i) {
		separators[i] = *argv[2 + i];
	}

	cmd_loop(inputFile, separators, nTrees, trees);
	return main_cleanup(0, inputFile, separators, nTrees, trees);
}

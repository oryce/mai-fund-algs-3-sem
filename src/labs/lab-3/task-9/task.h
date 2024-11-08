#pragma once

#include <stdbool.h>
#include <stdio.h>

typedef struct node {
	char* word;
	unsigned long occurrences;
	struct node* left;
	struct node* right;
} node_t;

node_t* node_create(const char* word);

void node_destroy(node_t* node);

bool node_insert(node_t** root, const char* word);

bool node_write(const node_t* node, FILE* outFp);

bool node_read(node_t** node, FILE* inFp);

void node_print(const node_t* node);

unsigned node_depth(const node_t* node);

unsigned long word_tree_occurrences(const node_t* root, const char* word);

node_t* word_tree_from_file(FILE* inputFile, char sep);

const char* word_tree_shortest(const node_t* root);

const char* word_tree_longest(const node_t* root);

bool word_tree_top_words(const node_t* root, size_t n, const node_t*** topWords,
                         size_t* nTopWords);

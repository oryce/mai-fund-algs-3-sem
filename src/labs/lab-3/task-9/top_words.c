#include <stdlib.h>

#include "lib/utils.h"
#include "task.h"

typedef struct node_heap {
	const node_t** nodes;
	size_t size;
	size_t capacity;
} node_heap_t;

int node_cmp(const node_t* a, const node_t* b) {
	if (!a || !b || a == b) return 0;
	return (int)a->occurrences - (int)b->occurrences;
}

int heap_cmp(node_heap_t* heap, size_t i, size_t j) {
	if (!heap) return 0;
	return node_cmp(heap->nodes[i], heap->nodes[j]);
}

bool heap_create(node_heap_t* heap, size_t capacity) {
	if (!heap) return false;

	heap->size = 0;
	heap->capacity = capacity;

	heap->nodes = (const node_t**)malloc(sizeof(node_t*) * capacity);
	if (!heap->nodes) return false;

	return true;
}

void heap_sift_down(node_heap_t* heap, size_t i) {
	if (!heap) return;

	while (2 * i + 1 < heap->size) {
		size_t left = 2 * i + 1;
		size_t right = 2 * i + 2;

		size_t j = left;

		if (right < heap->size && heap_cmp(heap, right, left) < 0) j = right;
		if (heap_cmp(heap, i, j) <= 0) break;

		SWAP(heap->nodes[i], heap->nodes[j], const node_t*);

		i = j;
	}
}

void heap_sift_up(node_heap_t* heap, size_t i) {
	if (!heap) return;

	while (i != 0 && heap_cmp(heap, i, (i - 1) / 2) < 0) {
		SWAP(heap->nodes[i], heap->nodes[(i - 1) / 2], const node_t*);
		i = (i - 1) / 2;
	}
}

bool heap_insert(node_heap_t* heap, const node_t* node) {
	if (!heap || !node || heap->size == heap->capacity) return false;

	heap->nodes[heap->size] = node;
	heap_sift_up(heap, heap->size);

	heap->size++;
	return true;
}

bool heap_replace_min(node_heap_t* heap, const node_t* node) {
	if (!heap || !node || heap->size == 0) return false;

	heap->nodes[0] = node;
	heap_sift_down(heap, 0);

	return true;
}

void word_tree_top_words0(const node_t* root, node_heap_t* heap) {
	if (!root || !heap) return;

	if (heap->size == heap->capacity) {
		heap_replace_min(heap, root);
	} else {
		heap_insert(heap, root);
	}

	if (root->left) word_tree_top_words0(root->left, heap);
	if (root->right) word_tree_top_words0(root->right, heap);
}

bool word_tree_top_words(const node_t* root, size_t n, const node_t*** topWords,
                         size_t* nTopWords) {
	if (!root || !n || !topWords || !nTopWords) return false;

	node_heap_t heap;
	if (!heap_create(&heap, n)) return false;

	// Build a min-heap from the word tree.
	word_tree_top_words0(root, &heap);

	*topWords = heap.nodes;
	*nTopWords = heap.size;

	// Sort the words in descending order.
	for (size_t i = heap.size; i-- > 0;) {
		SWAP(heap.nodes[0], heap.nodes[i], const node_t*);
		--heap.size;
		heap_sift_down(&heap, 0);
	}

	return true;
}

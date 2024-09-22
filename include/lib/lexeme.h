#pragma once

#include <stdio.h>

#include "lib/error.h"
#include "lib/vector.h"

typedef struct lexeme {
	const char* value;
} lexeme_t;

/**
 * Destroys a list of lexemes and de-allocates them.
 *
 * @param lexemes list of lexemes
 */
void lexeme_destroy(vector_ptr_t* lexemes);

/**
 * Reads lexemes from a file into a vector of lexemes.
 *
 * @param file input file
 * @param out output vector of lexemes
 *
 * @return status code
 */
error_t lexeme_read(FILE* file, vector_ptr_t* out);

/**
 * Writes lexemes to a file.
 *
 * @param file output file
 * @param lexemes vector of lexemes
 * @param sep lexeme separator char
 *
 * @return status code
 */
error_t lexeme_write(FILE* file, vector_ptr_t* lexemes, char sep);

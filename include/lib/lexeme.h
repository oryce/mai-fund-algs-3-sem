#pragma once

#include <stdio.h>

#include "lib/collections/string.h"
#include "lib/error.h"

typedef struct lexeme {
	const char* value;
} lexeme_t;

/**
 * Destroys a list of lexemes and de-allocates them.
 *
 * @param lexemes list of lexemes
 */
void lexeme_destroy(vector_str_t* lexemes);

/**
 * Reads lexemes from a file into a vector of lexemes.
 *
 * @param file input file
 * @param out output vector of lexemes
 *
 * @return `ERR_IO`  if the lexeme could not be read.
 *         `ERR_MEM` if the lexeme could not be pushed.
 */
error_t lexeme_read(FILE* file, vector_str_t* out);

/**
 * Writes lexemes to a file.
 *
 * @param file output file
 * @param lexemes vector of lexemes
 * @param sep lexeme separator char
 *
 * @return `ERR_IO` if the lexeme could not be written.
 */
error_t lexeme_write(FILE* file, vector_str_t* lexemes, char sep);

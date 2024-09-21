#include <stdio.h>
#include "lib/error.h"

error_t remove_digits(FILE* in, FILE* out);

error_t count_letters(FILE* in, FILE* out);

error_t count_special_characters(FILE* in, FILE* out);

error_t encode_non_digits(FILE* in, FILE* out);

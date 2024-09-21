#include "tasks.h"

#include "lib/chars.h"

error_t remove_digits(FILE* in, FILE* out) {
	int ch;

	while ((ch = fgetc(in)) != EOF) {
		if (ferror(in)) return ERROR_IO;
		if (chars_is_digit(ch)) continue;

		fputc(ch, out);
		if (ferror(out)) return ERROR_IO;
	}

	return ERROR_SUCCESS;
}

error_t count_letters(FILE* in, FILE* out) {
	int count = 0;
	int ch;

	while ((ch = fgetc(in)) != EOF) {
		if (ferror(in)) return ERROR_IO;

		bool flag = chars_is_alpha(ch);
		if (flag) ++count;

		if (ch == '\n') {
			fprintf(out, " %d", count);
			if (ferror(out)) return ERROR_IO;

			count = 0;
		}

		fputc(ch, out);
		if (ferror(out)) return ERROR_IO;
	}

	fprintf(out, " %d\n", count);
	if (ferror(out)) return ERROR_IO;

	return ERROR_SUCCESS;
}

error_t count_special_characters(FILE* in, FILE* out) {
	int count = 0;
	int ch;

	while ((ch = fgetc(in)) != EOF) {
		if (ferror(in)) return ERROR_IO;

		bool flag = !chars_is_alpha(ch) && !chars_is_digit(ch) && ch != ' ' && ch != '\n' && ch != '\r';
		if (flag) ++count;

		if (ch == '\n') {
			fprintf(out, " %d", count);
			if (ferror(out)) return ERROR_IO;

			count = 0;
		}

		fputc(ch, out);
		if (ferror(out)) return ERROR_IO;
	}

	fprintf(out, " %d\n", count);
	if (ferror(out)) return ERROR_IO;

	return ERROR_SUCCESS;
}

error_t encode_non_digits(FILE* in, FILE* out) {
	int ch;

	while ((ch = fgetc(in)) != EOF) {
		if (ferror(in)) return ERROR_IO;

		if (!chars_is_digit(ch) && ch != '\n') {
			fprintf(out, "%02X", ch);
		} else {
			fputc(ch, out);
		}

		if (ferror(out)) return ERROR_IO;
	}

	return ERROR_SUCCESS;
}

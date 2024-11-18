#include "insn_parser.h"

#include "lib/chars.h"

error_t parse_stream_fail(error_t errcode, vector_insn_t* insns, insn_t* insn,
                          string_t* buffer) {
	if (insns) {
		for (size_t i = 0; i != vector_insn_size(insns); ++i) {
			insn_t* other = vector_insn_get(insns, i);

			// Check if |insn| is in |insns|, preventing double-frees on the
			// args array.
			if (insn && other->args.buffer == insn->args.buffer) {
				insn = NULL;
			}

			insn_destroy(other);
		}

		vector_insn_destroy(insns);
	}

	if (insn) {
		insn_destroy(insn);
	}

	if (buffer) {
		string_destroy(buffer);
	}

	return errcode;
}

typedef enum { PARSE_OP, PARSE_ARG, PARSE_SEMI } parse_st_t;

error_t insn_parse_stream(vector_insn_t* insns, FILE* stream) {
	if (!insns) return ERR_INVVAL;

	*insns = vector_insn_create();

	// Instruction being parsed. Since vector_str_create doesn't allocate
	// anything, it's fine if it's overwritten
	insn_t insn = {.args = vector_str_create()};

	// Temporary buffer to hold stuff being parsed.
	string_t buffer;
	if (!string_create(&buffer)) {
		return parse_stream_fail(ERR_MEM, insns, &insn, &buffer);
	}

	// Parsing state.
	parse_st_t st = PARSE_OP;
	// Value returned by fgetc.
	int ch0;
	// Current char being read.
	char ch;

	while ((ch0 = fgetc(stream)) > 0) {
		ch = (char)ch0;

		switch (st) {
			case PARSE_OP: {
				ch = chars_lower(ch);

				// Skip whitespaces and newlines if we hadn't started parsing
				// yet.
				if (string_length(&buffer) == 0 &&
				    (chars_is_space(ch) || ch == '\n')) {
					continue;
				}

				if (ch == '(' || ch == ' ' || ch == ';') {
					// Finish the opcode
					opcode_t op = opcode_from_string(string_to_c_str(&buffer));
					if (op == OP_INVALID) {
						return parse_stream_fail(ERR_INVOP, insns, &insn,
						                         &buffer);
					}
					// Ensure that only `Free(a);` is parsed with braces.
					if ((op == OP_FREE && ch != '(') ||
					    (op != OP_FREE && ch == '(')) {
						return parse_stream_fail(ERR_UNEXPTOK, insns, &insn,
						                         &buffer);
					}
					insn = insn_create(op);
					if (!string_clear(&buffer)) {
						return parse_stream_fail(ERR_MEM, insns, &insn,
						                         &buffer);
					}
				} else {
					if (!string_append_char(&buffer, ch)) {
						return parse_stream_fail(ERR_MEM, insns, &insn,
						                         &buffer);
					}
				}

				if (ch == '(' || ch == ' ') {
					st = PARSE_ARG;
				} else if (ch == ';') {
					if (!vector_insn_push_back(insns, insn)) {
						return parse_stream_fail(ERR_MEM, insns, &insn,
						                         &buffer);
					}
					st = PARSE_OP;
				}

				break;
			}
			case PARSE_ARG: {
				// Finish the argument.
				if (ch == ',' || ch == ';' ||
				    (ch == ')' && insn.op == OP_FREE)) {
					string_t arg = {.initialized = false};
					if (!string_copy(&buffer, &arg)) {
						return parse_stream_fail(ERR_MEM, insns, &insn,
						                         &buffer);
					}
					if (!vector_str_push_back(&insn.args, arg)) {
						string_destroy(&arg);
						return parse_stream_fail(ERR_MEM, insns, &insn,
						                         &buffer);
					}
					if (!string_clear(&buffer)) {
						return parse_stream_fail(ERR_MEM, insns, &insn,
						                         &buffer);
					}
				} else if (!chars_is_space(ch) && ch != '\n') {
					if (!string_append_char(&buffer, ch)) {
						return parse_stream_fail(ERR_MEM, insns, &insn,
						                         &buffer);
					}
				}

				if (ch == ',') {
					st = PARSE_ARG;
				} else if (ch == ';') {
					if (!vector_insn_push_back(insns, insn)) {
						return parse_stream_fail(ERR_MEM, insns, &insn,
						                         &buffer);
					}
					st = PARSE_OP;
				} else if (ch == ')' && insn.op == OP_FREE) {
					st = PARSE_SEMI;
				}

				break;
			}
			case PARSE_SEMI: {
				if (ch != ';') {
					return parse_stream_fail(ERR_UNEXPTOK, insns, &insn,
					                         &buffer);
				}
				if (!vector_insn_push_back(insns, insn)) {
					return parse_stream_fail(ERR_MEM, insns, &insn, &buffer);
				}
				st = PARSE_OP;
				break;
			}
		}
	}

	if (st != PARSE_OP) {
		return parse_stream_fail(ERR_UNEXPTOK, insns, &insn, &buffer);
	}
	if (ferror(stream)) {
		return parse_stream_fail(ERR_IO, insns, &insn, &buffer);
	}

	string_destroy(&buffer);
	return 0;
}

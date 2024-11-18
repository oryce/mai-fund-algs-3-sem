#include "insn_parser.h"
#include "interp.h"

error_t main_clean(error_t errcode, vector_insn_t* insns, interp_t* ip,
                   FILE* stream) {
	if (insns) {
		for (size_t i = 0; i != vector_insn_size(insns); ++i) {
			insn_destroy(vector_insn_get(insns, i));
		}
		vector_insn_destroy(insns);
	}

	if (ip) {
		interp_destroy(ip);
	}

	fclose(stream);
	return errcode;
}

int main(void) {
	error_t error;

	FILE* cmds = fopen("cmds.txt", "r");
	if (!cmds) {
		fprintf(stderr, "Can't open cmds.txt for reading.\n");
		return 1;
	}

	vector_insn_t insns;
	interp_t ip = interp_create();

	if ((error = insn_parse_stream(&insns, cmds))) {
		error_print(error);
		return main_clean(error, &insns, &ip, cmds);
	}

	printf("[debug] Parsed instruction list:\n");

	for (size_t i = 0; i != vector_insn_size(&insns); ++i) {
		string_t string;
		if ((error = insn_to_string(vector_insn_get(&insns, i), &string))) {
			return main_clean(error, &insns, &ip, cmds);
		}
		printf("  %s\n", string_to_c_str(&string));
		string_destroy(&string);
	}

	if ((error = interp_run(&ip, &insns))) {
		return main_clean(error, &insns, &ip, cmds);
	}

	return main_clean(0, &insns, &ip, cmds);
}

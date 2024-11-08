#include <stdlib.h>

#include "find.h"
#include "task.h"

#define INVALID_COMMAND "Unknown or invalid command. Type 'h' for help.\n"

typedef void (*cmd_handler_t)(const tr_node_t* transports);

typedef struct cmd {
	const char* prefix;
	cmd_handler_t handler;
} cmd_t;

void err_print(error_t error) {
	error_fmt_t fmt[] = {&task_error_to_str};
	error_print_ex(error, fmt, sizeof(fmt) / sizeof(fmt[0]));
}

void cmd_help(const tr_node_t* transports) {
	printf(
	    "Command help:\n"
	    "  sr\n"
	    "     Finds transport with the shortest route.\n"
	    "  lr\n"
	    "     Finds transport with the longest route.\n"
	    "  sdt\n"
	    "     Finds transport with the shortest overall distance travelled.\n"
	    "  ldt\n"
	    "     Finds transport with the longest overall distance travelled.\n"
	    "  src\n"
	    "     Finds transport with the smallest route count.\n"
	    "  lrc\n"
	    "     Finds transport with the longest route count.\n"
	    "  sst\n"
	    "     Finds transport with the shortest stop time.\n"
	    "  lst\n"
	    "     Finds transport with the longest stop time.\n"
	    "  lit\n"
	    "     Finds transport with the longest idle time.\n");
}

void cmd_shortest_route(const tr_node_t* transports) {
	const tr_node_t* tr =
	    find_comparing(transports, &cmp_min_route_length, true);
	printf("Shortest route: %s, route length: %lf\n", tr->id,
	       min_route_length(tr));
}

void cmd_longest_route(const tr_node_t* transports) {
	const tr_node_t* tr =
	    find_comparing(transports, &cmp_max_route_length, false);
	printf("Longest route: %s, route length: %lf\n", tr->id,
	       max_route_length(tr));
}

void cmd_shortest_dist_tvld(const tr_node_t* transports) {
	const tr_node_t* tr = find_comparing(transports, &cmp_dist_tvld, true);
	printf("Shortest distance travelled: %s, dist: %lf\n", tr->id,
	       dist_travelled(tr));
}

void cmd_longest_dist_tvld(const tr_node_t* transports) {
	const tr_node_t* tr = find_comparing(transports, &cmp_dist_tvld, false);
	printf("Longest distance travelled: %s, dist: %lf\n", tr->id,
	       dist_travelled(tr));
}

void cmd_shortest_route_count(const tr_node_t* transports) {
	const tr_node_t* tr = find_comparing(transports, &cmp_route_count, true);
	printf("Shortest route count: %s, route count: %lu\n", tr->id,
	       route_count(tr));
}

void cmd_longest_route_count(const tr_node_t* transports) {
	const tr_node_t* tr = find_comparing(transports, &cmp_route_count, false);
	printf("Longest route count: %s, route count: %lu\n", tr->id,
	       route_count(tr));
}

void cmd_shortest_stop_time(const tr_node_t* transports) {
	const tr_node_t* tr = find_comparing(transports, &cmp_min_stop_time, true);
	printf("Shortest stop time: %s, stop time: %lf\n", tr->id,
	       min_stop_time(tr));
}

void cmd_longest_stop_time(const tr_node_t* transports) {
	const tr_node_t* tr = find_comparing(transports, &cmp_max_stop_time, false);
	printf("Longest stop time: %s, stop time: %lf\n", tr->id,
	       max_stop_time(tr));
}

void cmd_longest_idle_time(const tr_node_t* transports) {
	const tr_node_t* tr = find_comparing(transports, &cmp_idle_time, false);
	printf("Longest idle time: %s, idle time: %lf\n", tr->id,
	       max_idle_time(tr));
}

void cmd_loop(const tr_node_t* transports) {
	const cmd_t cmds[] = {{"h", &cmd_help},
	                      {"src", &cmd_shortest_route_count},
	                      {"lrc", &cmd_longest_route_count},
	                      {"sr", &cmd_shortest_route},
	                      {"lr", &cmd_longest_route},
	                      {"sdt", &cmd_shortest_dist_tvld},
	                      {"ldt", &cmd_longest_dist_tvld},
	                      {"sst", &cmd_shortest_stop_time},
	                      {"lst", &cmd_longest_stop_time},
	                      {"lit", &cmd_longest_idle_time}};
	const size_t nCmds = sizeof(cmds) / sizeof(cmds[0]);

	printf("Input a command ('h' for help)\n");

	char* cmd = NULL;
	size_t size;

	while (getline(&cmd, &size, stdin) > 0) {
		char* newline = strrchr(cmd, '\n');
		if (newline) *newline = '\0';

		bool matched = false;

		for (size_t i = 0; i != nCmds; ++i) {
			const cmd_t c = cmds[i];
			const size_t len = strlen(c.prefix);

			if (strncmp(cmd, c.prefix, len) == 0) {
				c.handler(transports);

				matched = true;
				break;
			}
		}

		if (!matched) {
			fprintf(stderr, INVALID_COMMAND);
		}
	}

	free(cmd);
}

int main_cleanup(int ret, tr_node_t* transports, FILE* stopFile) {
	tr_node_destroy(transports);
	fclose(stopFile);

	return ret;
}

int main(int argc, char** argv) {
	if (argc < 2) {
		fprintf(stderr, "usage: %s <stop files...>", argv[0]);
		return 1;
	}

	tr_node_t* transports = NULL;

	for (size_t i = 1; i != argc; ++i) {
		FILE* stopFile = fopen(argv[i], "r");
		if (!stopFile) {
			fprintf(stderr, "Can't open %s for reading.\n", argv[i]);
			return main_cleanup(2, transports, stopFile);
		}

		error_t error = tr_node_process_file(&transports, stopFile);
		if (error) {
			err_print(error);
			return main_cleanup(3, NULL, stopFile);
		}

		fclose(stopFile);
	}

	// Validate routes.
	for (const tr_node_t* tn = transports; tn; tn = tn->next) {
		const stop_node_t* start = tn->stops;
		const stop_node_t* end;

		do {
			error_t error;
			if ((error = tr_next_route(tn, &start, &end))) {
				err_print(error);
				return main_cleanup(4, transports, NULL);
			}
			start = end->next;
		} while (start);
	}

	cmd_loop(transports);
	return main_cleanup(0, transports, NULL);
}

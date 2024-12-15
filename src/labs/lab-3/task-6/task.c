#include "task.h"

#include <math.h>
#include <stdlib.h>

#include "lib/convert.h"

const char* task_error_to_str(error_t error) {
	switch (error) {
		case ERR_INVCOORD:
			return "Invalid stop: invalid coordinates";
		case ERR_INVARVTIME:
			return "Invalid stop: invalid arrival time";
		case ERR_INVDEPTIME:
			return "Invalid stop: invalid departure time";
		case ERR_INVSTTYPE:
			return "Invalid stop: invalid type";
		case ERR_NOTSTARTNODE:
			return "Invalid route: starting node is not of STOP_START type";
		case ERR_NOENDNODE:
			return "Invalid route: no STOP_END node";
		case ERR_TIMEOVERLAP:
			return "Invalid route: stop timeframes overlap";
		case ERR_NOTINTMNODE:
			return "Invalid route: intermediate node is not of "
			       "STOP_INTERMEDIATE type.";
		default:
			return NULL;
	}
}

stop_node_t* stop_node_create(void) {
	stop_node_t* node = (stop_node_t*)malloc(sizeof(stop_node_t));
	if (!node) return NULL;

	node->id = NULL;
	node->next = NULL;

	return node;
}

void stop_node_destroy(stop_node_t* node) {
	if (!node) return;

	free(node->id);
	if (node->next) stop_node_destroy(node->next);

	free(node);
}

typedef enum sn_parse_state {
	SN_ID,
	SN_ARRIVE_TIME,
	SN_DEPART_TIME,
	SN_TYPE,
	SN_VALID
} sn_parse_state_t;

error_t sn_parse_end(error_t ret, stop_node_t* node) {
	stop_node_destroy(node);
	return ret;
}

error_t stop_node_from_line(stop_node_t** node, char* line) {
	if (!node || !line) return ERROR_INVALID_PARAMETER;

	*node = stop_node_create();
	if (!(*node)) return ERROR_OUT_OF_MEMORY;

	sn_parse_state_t st = SN_ID;

	char* ptr = (char*)line;
	char* token;
	while ((token = strsep(&ptr, ","))) {
		switch (st) {
			case SN_ID:
				(*node)->id = strdup(token);
				if (!(*node)->id) {
					return sn_parse_end(ERROR_OUT_OF_MEMORY, *node);
				}

				st = SN_ARRIVE_TIME;
				break;
			case SN_ARRIVE_TIME: {
				struct tm tm;
				tm.tm_isdst = -1;
				if (!strptime(token, "%d.%m.%Y %H:%M:%S", &tm)) {
					return sn_parse_end(ERR_INVARVTIME, *node);
				}
				(*node)->arrive_time = mktime(&tm);

				st = SN_DEPART_TIME;
				break;
			}
			case SN_DEPART_TIME: {
				struct tm tm;
				tm.tm_isdst = -1;
				if (!strptime(token, "%d.%m.%Y %H:%M:%S", &tm)) {
					return sn_parse_end(ERR_INVDEPTIME, *node);
				}
				(*node)->depart_time = mktime(&tm);

				// arrive_time must be < depart_time
				if (difftime((*node)->arrive_time, (*node)->depart_time) >= 0) {
					return sn_parse_end(ERROR_INVALID_PARAMETER, *node);
				}

				st = SN_TYPE;
				break;
			}
			case SN_TYPE:
				if (strcmp(token, "S") == 0)
					(*node)->type = STOP_START;
				else if (strcmp(token, "I") == 0)
					(*node)->type = STOP_INTERMEDIATE;
				else if (strcmp(token, "E") == 0)
					(*node)->type = STOP_END;
				else
					return sn_parse_end(ERR_INVSTTYPE, *node);

				st = SN_VALID;
				break;
			case SN_VALID:
				// Unexpected token after the last valid token
				return sn_parse_end(ERROR_UNEXPECTED_TOKEN, *node);
		}
	}

	if (st != SN_VALID) {
		// Not enough tokens.
		return sn_parse_end(ERROR_UNEXPECTED_TOKEN, *node);
	}

	return 0;
}

tr_node_t* tr_node_create(void) {
	tr_node_t* node = (tr_node_t*)malloc(sizeof(tr_node_t));
	if (!node) return NULL;

	node->stops = NULL;
	node->next = NULL;

	return node;
}

void tr_node_destroy(tr_node_t* node) {
	if (!node) return;

	free(node->id);
	node->id = NULL;

	if (node->stops) stop_node_destroy(node->stops);
	if (node->next) tr_node_destroy(node->next);

	free(node);
}

error_t tn_parse_end(error_t ret, tr_node_t* tn, stop_node_t* sn, char* line) {
	tr_node_destroy(tn);
	stop_node_destroy(sn);
	free(line);

	return ret;
}

error_t tr_node_process_file(tr_node_t** node, FILE* fp) {
	if (!node) return ERROR_INVALID_PARAMETER;

	// Coords for the stop.
	double xCoord;
	double yCoord;
	bool coordsRead = false;

	// Read timetable lines from the stop file.
	char* line = NULL;
	size_t size;

	while (getline(&line, &size, fp) > 0) {
		// Read X and Y coordinates as the first line.
		if (!coordsRead) {
			char inXCoord[128];
			char inYCoord[128];

			if (sscanf(line, "%127s %127s", inXCoord, inYCoord) != 2 ||
			    str_to_double(inXCoord, &xCoord) ||
			    str_to_double(inYCoord, &yCoord)) {
				return tn_parse_end(ERR_INVCOORD, *node, NULL, line);
			}

			coordsRead = true;
			continue;
		}

		char* newline = strrchr(line, '\n');
		if (newline) *newline = '\0';

		// Parse stop node.
		stop_node_t* sn;

		error_t error = stop_node_from_line(&sn, line);
		if (error) return tn_parse_end(error, *node, NULL, line);

		// Find an insertion point for the node.
		tr_node_t** tn = node;

		while (*tn && strcmp((*tn)->id, sn->id) != 0) {
			tn = &(*tn)->next;
		}

		if (*tn) {
			free(sn->id);
		} else {
			*tn = tr_node_create();
			if (!(*tn)) return tn_parse_end(ERROR_OUT_OF_MEMORY, *node, sn, line);

			(*tn)->id = sn->id;
		}

		// Add to the stop ptr, such that it's ordered by arrive_time.
		stop_node_t** ins = &(*tn)->stops;

		while (*ins && difftime((*ins)->arrive_time, sn->arrive_time) < 0) {
			ins = &(*ins)->next;
		}

		if (*ins) {
			stop_node_t* sn0 = *ins;

			*ins = stop_node_create();
			if (!(*ins)) return tn_parse_end(ERROR_OUT_OF_MEMORY, *node, sn, line);

			(*ins)->next = sn0;
		} else {
			*ins = stop_node_create();
			if (!(*ins)) return tn_parse_end(ERROR_OUT_OF_MEMORY, *node, sn, line);
		}

		// Copy properties to the inserted node.
		sn->id = NULL;  // ID was moved to *tn
		(*ins)->arrive_time = sn->arrive_time;
		(*ins)->depart_time = sn->depart_time;
		(*ins)->type = sn->type;
		(*ins)->coords.x = xCoord;
		(*ins)->coords.y = yCoord;

		stop_node_destroy(sn);
	}

	free(line);

	if (ferror(fp)) {
		return tn_parse_end(ERROR_IO, *node, 0, NULL);
	}

	return 0;
}

error_t tr_next_route(const tr_node_t* tn, const stop_node_t** start,
                      const stop_node_t** end) {
	// Check that any of the pointers are not NULL, and that *start points to a
	// valid starting node.
	if (!tn || !start || !end || !(*start)) return ERROR_INVALID_PARAMETER;
	if ((*start)->type != STOP_START) return ERR_NOTSTARTNODE;

	// Traverse the nodes from *start until the ptr ends, or we encounter a
	// stop node. Check depart_time validity in the meantime.
	const stop_node_t* sn;
	for (sn = *start; sn && sn->type != STOP_END; sn = sn->next) {
		// Check that there isn't another STOP_START node.
		if (sn != *start && sn->type == STOP_START) return ERR_NOTINTMNODE;

		if (sn->next) {
			double diff = difftime(sn->depart_time, sn->next->depart_time);
			if (diff >= 0) return ERR_TIMEOVERLAP;
		}
	}

	// If we didn't encounter a stop node, the stop ptr is invalid.
	if (!sn) return ERR_NOENDNODE;

	*end = sn;
	return 0;
}

double tr_route_length(const stop_node_t* start, const stop_node_t* end) {
	if (!start || !end) return 0;

	double length = 0;

	for (const stop_node_t* sn = start; sn != end; sn = sn->next) {
		if (!sn->next) return 0;
		length += sqrt(pow(sn->coords.x - sn->next->coords.x, 2) +
		               pow(sn->coords.y - sn->next->coords.y, 2));
	}

	return length;
}

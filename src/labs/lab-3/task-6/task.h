#pragma once

#include <time.h>

#include "lib/error.h"

#define ERR_INVCOORD 0x10000001
#define ERR_INVARVTIME 0x10000002
#define ERR_INVDEPTIME 0x10000003
#define ERR_INVSTTYPE 0x10000004
#define ERR_NOTSTARTNODE 0x10000005
#define ERR_NOENDNODE 0x10000006
#define ERR_TIMEOVERLAP 0x10000007
#define ERR_NOTINTMNODE 0x10000008

const char* task_error_to_str(error_t error);

typedef struct stop_node {
	struct {
		double x;
		double y;
	} coords;
	char* id;
	time_t arrive_time;
	time_t depart_time;
	enum { STOP_START, STOP_INTERMEDIATE, STOP_END } type;
	struct stop_node* next;
} stop_node_t;

typedef struct tr_node {
	char* id;
	stop_node_t* stops;
	struct tr_node* next;
} tr_node_t;

stop_node_t* stop_node_create(void);

void stop_node_destroy(stop_node_t* node);

error_t stop_node_from_line(stop_node_t** node, char* line);

tr_node_t* tr_node_create(void);

void tr_node_destroy(tr_node_t* node);

error_t tr_node_process_file(tr_node_t** node, FILE* fp);

error_t tr_next_route(const tr_node_t* tn, const stop_node_t** start,
                      const stop_node_t** end);

double tr_route_length(const stop_node_t* start, const stop_node_t* end);

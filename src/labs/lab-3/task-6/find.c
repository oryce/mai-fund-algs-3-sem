#include "find.h"

#include <float.h>

const tr_node_t* find_comparing(const tr_node_t* list, td_comp_t comp,
                                bool min) {
	if (!list) return NULL;
	const tr_node_t* result = list;

	for (const tr_node_t* tn = result->next; tn; tn = tn->next) {
		int order = comp(tn, result);
		if ((min && order < 0) || (!min && order > 0)) result = tn;
	}

	return result;
}

int cmp_double(double a, double b) {
	if (a < b)
		return -1;
	else if (a > b)
		return 1;
	else
		return 0;
}

double min_route_length(const tr_node_t* tn) {
	if (!tn) return 0;

	double minLength = DBL_MAX;

	const stop_node_t* start = tn->stops;
	const stop_node_t* end;

	do {
		tr_next_route(tn, &start, &end);

		double length = tr_route_length(start, end);
		if (length < minLength) minLength = length;

		start = end->next;
	} while (start);

	return minLength;
}

int cmp_min_route_length(const tr_node_t* a, const tr_node_t* b) {
	if (!a || !b || a == b) return 0;
	return cmp_double(min_route_length(a), min_route_length(b));
}

double max_route_length(const tr_node_t* tn) {
	if (!tn) return 0;

	double maxLength = DBL_MIN;

	const stop_node_t* start = tn->stops;
	const stop_node_t* end;

	do {
		tr_next_route(tn, &start, &end);

		double length = tr_route_length(start, end);
		if (length > maxLength) maxLength = length;

		start = end->next;
	} while (start);

	return maxLength;
}

int cmp_max_route_length(const tr_node_t* a, const tr_node_t* b) {
	if (!a || !b || a == b) return 0;
	return cmp_double(max_route_length(a), max_route_length(b));
}

double dist_travelled(const tr_node_t* tn) {
	double dist = 0;

	const stop_node_t* start = tn->stops;
	const stop_node_t* end;

	do {
		tr_next_route(tn, &start, &end);

		dist += tr_route_length(start, end);

		start = end->next;
	} while (start);

	return dist;
}

int cmp_dist_tvld(const tr_node_t* a, const tr_node_t* b) {
	if (!a || !b || a == b) return 0;
	return cmp_double(dist_travelled(a), dist_travelled(b));
}

double min_stop_time(const tr_node_t* tn) {
	double minTime = DBL_MAX;

	const stop_node_t* sn = tn->stops;
	do {
		double time = difftime(sn->depart_time, sn->arrive_time);
		if (time < minTime) minTime = time;
	} while ((sn = sn->next));

	return minTime;
}

double max_stop_time(const tr_node_t* tn) {
	double maxTime = DBL_MIN;

	const stop_node_t* sn = tn->stops;
	do {
		double time = difftime(sn->depart_time, sn->arrive_time);
		if (time > maxTime) maxTime = time;
	} while ((sn = sn->next));

	return maxTime;
}

int cmp_min_stop_time(const tr_node_t* a, const tr_node_t* b) {
	if (!a || !b || a == b) return 0;
	return cmp_double(min_stop_time(a), min_stop_time(b));
}

int cmp_max_stop_time(const tr_node_t* a, const tr_node_t* b) {
	if (!a || !b || a == b) return 0;
	return cmp_double(max_stop_time(a), max_stop_time(b));
}

double max_idle_time(const tr_node_t* tn) {
	double maxIdleTime = DBL_MIN;

	const stop_node_t* start = tn->stops;
	const stop_node_t* end;

	do {
		tr_next_route(tn, &start, &end);

		double idleTime = 0;

		const stop_node_t* sn = tn->stops;
		do {
			double time = difftime(sn->depart_time, sn->arrive_time);
			idleTime += time;
		} while ((sn = sn->next));

		if (idleTime > maxIdleTime) {
			maxIdleTime = idleTime;
		}

		start = end->next;
	} while (start);

	return maxIdleTime;
}

int cmp_idle_time(const tr_node_t* a, const tr_node_t* b) {
	if (!a || !b || a == b) return 0;
	return cmp_double(max_idle_time(a), max_idle_time(b));
}

unsigned long route_count(const tr_node_t* tn) {
	unsigned long routeCount = 0;

	const stop_node_t* start = tn->stops;
	const stop_node_t* end;

	do {
		tr_next_route(tn, &start, &end);
		start = end->next;

		++routeCount;
	} while (start);

	return routeCount;
}

int cmp_route_count(const tr_node_t* a, const tr_node_t* b) {
	if (!a || !b || a == b) return 0;

	long order = (long)route_count(a) - (long)route_count(b);
	if (order < 0)
		return -1;
	else if (order > 0)
		return 1;
	else
		return 0;
}

#pragma once

#include "task.h"

typedef int (*td_comp_t)(const tr_node_t* a, const tr_node_t* b);

const tr_node_t* find_comparing(const tr_node_t* list, td_comp_t comp,
                                bool min);

double min_route_length(const tr_node_t* tn);

int cmp_min_route_length(const tr_node_t* a, const tr_node_t* b);

double max_route_length(const tr_node_t* tn);

int cmp_max_route_length(const tr_node_t* a, const tr_node_t* b);

double dist_travelled(const tr_node_t* tn);

int cmp_dist_tvld(const tr_node_t* a, const tr_node_t* b);

double min_stop_time(const tr_node_t* tn);

int cmp_min_stop_time(const tr_node_t* a, const tr_node_t* b);

double max_stop_time(const tr_node_t* tn);

int cmp_max_stop_time(const tr_node_t* a, const tr_node_t* b);

double max_idle_time(const tr_node_t* tn);

int cmp_idle_time(const tr_node_t* a, const tr_node_t* b);

unsigned long route_count(const tr_node_t* tn);

int cmp_route_count(const tr_node_t* a, const tr_node_t* b);

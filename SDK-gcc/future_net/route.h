#ifndef __ROUTE_H__
#define __ROUTE_H__
#include "glpk.h"
#include<stdlib.h>

void search_route(char *graph[5000], int edge_num, char *condition);
void debug_sortlink();

#endif

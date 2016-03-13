
#include "route.h"
#include "lib_record.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

#define MAX_NODE 600
#define MAX_LINK 4800
typedef struct ArcBox{
    int tailvex, headvex;
    struct ArcBox *tlink, *hlink;
    int cost;
    int id;
}ArcBox;

typedef struct VexNode{
    int id;
    ArcBox *firstin, *firstout;
}VexNode;

typedef struct{
    VexNode node[MAX_NODE];
    int vexnum, arcnum;
}Graph;

Graph G;
ArcBox Link[MAX_LINK];

int str_to_int(char *topo, int &lid, int &sid, int &did, int &cost)
{
    int i = 0, j, num, ws;
    char *base = topo;
    for(i=0; i < 4; i++)
    {
        num = 0;
        if(i < 3)
        {
            while( (*topo++) != ',');
            ws = topo - base - 1;
        }
        else
        {
            while( (*topo) != '\0' && (*topo) != '\n' )
                topo++;
             ws = topo - base;
        }
        for(j = 0; j < ws; j++)
            num += (base[j] - 0x30) * pow(10, ws - j - 1);
        if(0 == i)
            lid = num;
        else if(1 == i)
            sid = num;
        else if(2 == i)
            did = num;
        else if(3 == i)
            cost = num;
        base = topo;
    }
    return 1;
}

void build_graph(int lid, int sid, int did, int cost)
{
	ArcBox *l;
    Link[lid].id = lid;
    Link[lid].tailvex = sid;
	Link[lid].headvex = did;
	Link[lid].cost = cost;
	Link[lid].tlink = NULL;
	Link[lid].hlink = NULL;
	G.node[sid].id = sid;
	G.node[did].id = did;
	
	if(G.node[sid].firstout)
	{
		l = G.node[sid].firstout;
		while(l->tlink)
		{
			l = l->tlink;
		}
		l->tlink = &Link[lid];
	}
	else
	{
		G.node[sid].firstout = &Link[lid];
	}
	
	if(G.node[did].firstin)
	{
		l = G.node[did].firstin;
		while(l->hlink)
		{
			l = l->hlink;
		}
		l->hlink = &Link[lid];
	}
	else
	{
		G.node[did].firstin = &Link[lid];
	}
	
}

//你要完成的功能总入口
void search_route(char *topo[5000], int edge_num, char *demand)
{
    /*
    unsigned short result[] = {2, 6, 3};//示例中的一个解

    for (int i = 0; i < 3; i++)
        record_result(result[i]);
    */
    printf("search_route\n");
    int i;
    int lid, sid, did, cost;
    G.arcnum = edge_num;
    for(i = 0; i < edge_num; i++)
    {
        str_to_int(topo[i], lid, sid, did, cost);
        printf("read info from topo[%d]:%d  %d  %d  %d \n", i, lid, sid, did, cost);
        build_graph(lid, sid, did, cost);
    }
    printf("end_search_route\n");
}

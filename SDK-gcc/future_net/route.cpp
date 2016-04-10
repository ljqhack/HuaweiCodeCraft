
#include "route.h"
#include "lib_record.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

#define MAX_NODE  600
#define MAX_LINK  4800
//#define INFINITY  99999999

typedef struct ArcBox{
    int tailvex, headvex;
    struct ArcBox *tlink, *hlink;
    int cost;
    int id;
    int mid;
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
ArcBox *SortLink[MAX_LINK];

int Start=0, End=1;
int MustNode[300]={0};
int MustLen = 0;

int Color[5000];

/***********************************/
typedef struct c{
    int Circle[10000];
    int indexCircle[2000];
    int countCircle;
}cir_t;
cir_t fCircle;

bool isArcFound(int mid)
{
	int i;
	for(i = 0; i < (fCircle.indexCircle[fCircle.countCircle] ); i++)
	{
		if(mid == fCircle.Circle[i])
		    return true;
	}
	if(i >= (fCircle.indexCircle[fCircle.countCircle] ))
	{
		return false;
	}
	
}

/************************************/

/*************stack******************/
typedef struct st{
    int *base;
    int *top;
    int stackSize;	
}mystack_t;

mystack_t S;
int STACK[8000];
void init_stack()
{
	memset(STACK, 0, sizeof(STACK));
	S.base = STACK;
	S.top = STACK;
	S.stackSize = 8000;
}
void push(int i)
{
	*S.top++ = i;
}

void pop()
{
	S.top--;
}

/************************************/

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

bool isMustNode(int id)
{
	int i = 0;
	for(i = 0; i < MustLen; i++)
	{
		if(id == MustNode[i])
		    return true;
	}
	if(i >= MustLen)
	    return false;
}


void demand_to_int(char *demand, int *start, int *end, int mustnode[], int *mustlen)
{
	//printf("demand:%s\n", demand);
	char *base = demand;
	int num = 0, ws, j, count = 0, flag=0;
	while(((*demand) != '\0'))   //&& ((*demand) != '\r') 
	{
		demand++;
		flag = 0;
		if( ((*demand) == ',') )
		{
			count++;
			flag = 1;

		}
		if( (*demand) == '|' )
		{
			count++;
			flag = 1;
		}
		if(flag)
		{
		    ws = demand - base;
		    num = 0;
		    for(j = 0; j < ws; j++)
		    {
			    num += (base[j] - 0x30)* pow(10, ws - j - 1);
		    }
		    base = demand + 1;
		    if(1 == count)
		    {
				*start = num;
				//printf("start: %d", *start);
			}
			else if(2 == count)
			{
				*end = num;
				//printf("end: %d", *end);
			}
			else
			{
				*(mustnode + count - 2 - 1) = num;
				//printf("must: %d", *(mustnode + count - 2 - 1));
			}
		}
	}
	//printf("sssss:%s\n, strlen:%d, char:%d",base, strlen(base), '\n');
	count++;
	/*
    ws = (demand) - base;
    num = 0;
    for(j = 0; j < ws; j++)
    {
	    num += (base[j] - 0x30)* pow(10, ws - j - 1);
    }
    */
    num = atoi(base);
	*(mustnode + count - 2 - 1) = num;
	printf("must: %d", *(mustnode + count - 2 - 1));
	*mustlen = count - 2;
}

void out_result(glp_prob *lp)
{
	printf("start out_result\n");
	int num, i;
	ArcBox *l;
	int result[4001];
	memset(result, 0, sizeof(result));
	num = glp_get_num_cols(lp);
	for(i = 1; i < num+1; i++)
	{
		result[i] = glp_mip_col_val(lp, i )>0.9 ? 1 : 0;
		//printf("glp_col[%d]=%lf", i, glp_mip_col_val(lp, i ));
		//printf("result[%d]=%d\n",i, result[i] );
	}
	l = G.node[Start].firstout;
	while( (l->tailvex != End) )
	{
		if(result[l->mid])
		{
		    record_result(l->id);
		    //printf("%d|", l->id);
		    if(l->headvex == End)
		        break;
		    l = G.node[l->headvex].firstout;
		    continue;
		}
		l = l->tlink;
	} 
	printf("end");
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
	
	if((!G.node[sid].firstin) && (!G.node[sid].firstout))
	{
		G.vexnum += 1;
	}
	if((!G.node[did].firstin) && (!G.node[did].firstout))
	{
		G.vexnum += 1;
	}
	
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

void traverse_graph()
{
	int i = 0;
	ArcBox *l;
	printf("\ngraph node num:%d\n", G.vexnum);
	for(i = 0; i < MAX_NODE; i++)
	{
		if(G.node[i].firstin || G.node[i].firstout)
		{
			printf("\nnode[%d]\n", i);	
			printf("in:");
			l = G.node[i].firstin;
			while(l)
			{
				printf(" %d ", l->id);
				l = l->hlink;
			}
			printf("\nout:");
			l = G.node[i].firstout;
			while(l)
			{
				printf(" %d ", l->id);
				l = l->tlink;
			}
		}
		
	}
}

void dijkstra(int start_node)
{
	printf("dijkstra");
	int i = 0;
	for(i = 0; i < G.vexnum; i ++)
	{
	}
}

void testFun()
{
	printf("testFun!!\n");
	glp_prob *lp;
	glp_iocp iocp;
    glp_bfcp bfcp;
    
    lp = glp_create_prob();
    
    glp_init_iocp(&iocp);
    iocp.presolve = GLP_ON;
    iocp.msg_lev = GLP_MSG_OFF;
    iocp.binarize = GLP_ON;
    
    printf("start glp_get_bfcp\n");
    glp_get_bfcp(lp, &bfcp);
    printf("end glp_get_bfcp\n");
    
    int ia[1001], ja[1001];
    double ar[1001]={0,\
		             1, 1, 1, 0, 0, 0, 0,\
		            -1, 0, 0,-1, 0,-1, 0,\
		             0,-1, 0, 1, 1, 0,-1,\
		             0, 0,-1, 0,-1, 1, 1,\
		             0, 1, 0, 0, 0, 0, 1,\
		             0, 0, 1, 0, 1, 0, 0,\
		             0, 0, 0, 0, 1, 0, 1};
    double z,x1,x2,x3,x4,x5,x6,x7;
    int i, j;

    glp_set_prob_name(lp, "mytest");
    glp_set_obj_dir(lp, GLP_MIN);
    
    glp_add_rows(lp, 7);
    glp_set_row_name(lp, 1, "a");
    glp_set_row_bnds(lp, 1, GLP_FX, 1,1);
    glp_set_row_name(lp, 2, "b");
    glp_set_row_bnds(lp, 2, GLP_FX, -1,-1);
    glp_set_row_name(lp, 3, "c");
    glp_set_row_bnds(lp, 3, GLP_FX, 0,0);
    glp_set_row_name(lp, 4, "d");
    glp_set_row_bnds(lp, 4, GLP_FX, 0,0);
    glp_set_row_name(lp, 5, "e");
    glp_set_row_bnds(lp, 5, GLP_FX, 1,1);
    glp_set_row_name(lp, 6, "f");
    glp_set_row_bnds(lp, 6, GLP_FX, 1,1);
    glp_set_row_name(lp, 7, "g");
    glp_set_row_bnds(lp, 7, GLP_UP, 1, 1);
    
    glp_add_cols(lp, 7);
    glp_set_col_name(lp, 1, "x01");
    glp_set_col_bnds(lp, 1, GLP_DB, 0, 1);
    glp_set_obj_coef(lp, 1, 1);
    glp_set_col_kind(lp, 1, GLP_IV);
    glp_set_col_name(lp, 2, "x02");
    glp_set_col_bnds(lp, 2, GLP_DB, 0, 1);
    glp_set_obj_coef(lp, 2, 2);
    glp_set_col_kind(lp, 2, GLP_IV);
    glp_set_col_name(lp, 3, "x03");
    glp_set_col_bnds(lp, 3, GLP_DB, 0, 1);
    glp_set_obj_coef(lp, 3, 1);
    glp_set_col_kind(lp, 3, GLP_IV);
    glp_set_col_name(lp, 4, "x21");
    glp_set_col_bnds(lp, 4, GLP_DB, 0, 1);
    glp_set_obj_coef(lp, 4, 3);
    glp_set_col_kind(lp, 4, GLP_IV);
    glp_set_col_name(lp, 5, "x23");
    glp_set_col_bnds(lp, 5, GLP_DB, 0, 1);
    glp_set_obj_coef(lp, 5, 1);
    glp_set_col_kind(lp, 5, GLP_IV);
    glp_set_col_name(lp, 6, "x31");
    glp_set_col_bnds(lp, 6, GLP_DB, 0, 1);
    glp_set_obj_coef(lp, 6, 1);
    glp_set_col_kind(lp, 6, GLP_IV);
    glp_set_col_name(lp, 7, "x32");
    glp_set_col_bnds(lp, 7, GLP_DB, 0, 1);
    glp_set_obj_coef(lp, 7, 1);
    glp_set_col_kind(lp, 7, GLP_IV);
    
    printf("start for \n");
    for(i = 1; i <= 7; i++)
    {
		for(j = 1; j <= 7; j++)
		{
			ia[(i-1)*7 + j] = i;
			ja[(i-1)*7 + j] = j;
			//printf("ja[%d]=%d\n", (i-1)*7 + j, j);
		}
	}
	
	printf("begin intopt\n");
    glp_load_matrix(lp, 49, ia, ja, ar);
    //glp_simplex(lp, NULL);
    glp_set_bfcp(lp, &bfcp);
    glp_intopt(lp, &iocp);
    printf("end intopt\n");
    /*
    z = glp_get_obj_val(lp);
    x1 = glp_get_col_prim(lp, 1);
    x2 = glp_get_col_prim(lp, 2);
    x3 = glp_get_col_prim(lp, 3);
    x4 = glp_get_col_prim(lp, 4);
    x5 = glp_get_col_prim(lp, 5);
    x6 = glp_get_col_prim(lp, 6);
    x7 = glp_get_col_prim(lp, 7);
    */
    z = glp_mip_obj_val(lp);
    x1 = glp_mip_col_val(lp, 1);
    x2 = glp_mip_col_val(lp, 2);
    x3 = glp_mip_col_val(lp, 3);
    x4 = glp_mip_col_val(lp, 4);
    x5 = glp_mip_col_val(lp, 5);
    x6 = glp_mip_col_val(lp, 6);
    x7 = glp_mip_col_val(lp, 7);
    printf("z=%lf\n", z);
    printf("x=%lf,%lf,%lf,%lf,%lf,%lf,%lf\n", x1,x2,x3,x4,x5,x6,x7);
    
    glp_delete_prob(lp);
    printf("testFun end...\n");
}

void build_pro(glp_prob *lp)
{
    int i = 0, j=0, rowIndex = 0;
    int mid = 0;
	ArcBox *l;
	int ind[3001] = {0};        //赛题规定每个顶点的出度不超过8,最多有600个顶点
    double val[3001] = {0};
    //double z,x1,x2,x3,x4,x5,x6,x7;
	printf("\ngraph node num:%d\n", G.vexnum);
	for(i = 0; i < MAX_NODE; i++)
	{
		if( G.node[i].firstout || G.node[i].firstin)
		{
			//printf("\nout:");
			//printf("%d\n", i);
			l = G.node[i].firstout;
			while(l)
			{
				//map arc
				mid++;
				SortLink[mid] = l;
				l->mid = mid;
				//printf("link: %d %d \n", l->id, l->mid);
				//add cols
				glp_add_cols(lp, 1);
                glp_set_col_name(lp, mid, "x");
                glp_set_col_bnds(lp, mid, GLP_DB, 0, 1);
                glp_set_obj_coef(lp, mid, l->cost);
                glp_set_col_kind(lp, mid, GLP_IV);
				l = l->tlink;
			}
		}
	}
	//debug_sortlink();
	for(i = 0; i < MAX_NODE; i++)
	{
		if( G.node[i].firstout || G.node[i].firstin)
		{	
			if(i == Start)
			{
				j = 1;
				memset(ind, 0, sizeof(ind));
                memset(val, 0, sizeof(val));
				l = G.node[i].firstout;
				while(l)
				{
					ind[j] = l->mid;
					val[j] = 1;
					//printf("ind: %d %d %lf \n", j, ind[j], val[j]);
					j++;
					l = l->tlink;
				}
				rowIndex = glp_add_rows(lp, 1);
				glp_set_mat_row(lp, rowIndex, j - 1, ind, val);
                glp_set_row_bnds(lp, rowIndex, GLP_FX, 1.0, 1.0); 
			}
			else if(i == End)
			{
				j = 1;
			    memset(ind, 0, sizeof(ind));
                memset(val, 0, sizeof(val));
                l = G.node[i].firstin;
                while(l)
                {
					ind[j] = l->mid;
					val[j] = -1;
					//printf("ind: %d %d %lf \n", j, ind[j], val[j]);
					j++;
					l = l->hlink;
				}
				rowIndex = glp_add_rows(lp, 1);
				glp_set_mat_row(lp, rowIndex, j - 1, ind, val);
                glp_set_row_bnds(lp, rowIndex, GLP_FX, -1.0, -1.0); 
			}
			else
			{
				j = 1;
			    memset(ind, 0, sizeof(ind));
                memset(val, 0, sizeof(val));
                l = G.node[i].firstin;
                while(l)
                {
					
					ind[j] = l->mid;
					val[j] = -1;
					//printf("indx: %d %d %lf \n", j, ind[j], val[j]);
					j++;
					l = l->hlink;
				}
				l = G.node[i].firstout;
				while(l)
				{
				    ind[j] = l->mid;
					val[j] = 1;
					//printf("indx: %d %d %lf \n", j, ind[j], val[j]);
					j++;
					l = l->tlink;
				}
				rowIndex = glp_add_rows(lp, 1);
				glp_set_mat_row(lp, rowIndex, j - 1, ind, val);
                glp_set_row_bnds(lp, rowIndex, GLP_FX, -0.0, -0.0); 
			}
			
			if(isMustNode(i))
			{
				j = 1;
			    memset(ind, 0, sizeof(ind));
                memset(val, 0, sizeof(val));
                l = G.node[i].firstin;
                while(l)
                {
					
					ind[j] = l->mid;
					val[j] = 1;
					//printf("ind_must: %d %d %lf \n", j, ind[j], val[j]);
					j++;
					l = l->hlink;
				}
				rowIndex = glp_add_rows(lp, 1);
				glp_set_mat_row(lp, rowIndex, j - 1, ind, val);
                glp_set_row_bnds(lp, rowIndex, GLP_FX, 1.0, 1.0); 
			}
			
		}
		
	}
	/*
    memset(ind, 0, sizeof(ind));
    memset(val, 0, sizeof(val));
    ind[1] = 5;
    ind[2] = 7;
    val[1] = 1.0;
    val[2] = 1.0;
    rowIndex = glp_add_rows(lp, 1);
	glp_set_mat_row(lp, rowIndex, 2, ind, val);
    glp_set_row_bnds(lp, rowIndex, GLP_UP, 1.0, 1.0); 
    */
}

int gI;
void DFS_Visit(ArcBox *arc, int *result, int start)
{
	//printf("DFS_Visit, start=%d\n", start);
	ArcBox *l;
	push(arc->mid);
	Color[arc->mid] = 1;  //gray color
	l = G.node[arc->headvex].firstout;
	while(l)
	{
		if( result[l->mid] && (l->headvex == start) )
		{
			push(l->mid);
			fCircle.countCircle++;
            memcpy( &fCircle.Circle[ fCircle.indexCircle[fCircle.countCircle - 1] ],\
                    S.base, (S.top - S.base)*sizeof(int) );
            fCircle.indexCircle[fCircle.countCircle] = (S.top - S.base) + fCircle.indexCircle[fCircle.countCircle - 1];
            /*
			for(gI = 0; gI < (S.top - S.base); gI++)
			{
				printf("stack:%d \n", *(S.base+gI));
			}
			printf("-----\n");
			*/
			break;
		}
		if( result[l->mid] && (Color[l->mid] == 0) )
		{
			DFS_Visit(l, result, start);
		}
		l = l->tlink;
	}
	pop();
    Color[arc->mid] = 2;  //black color
}

void findCircle(int *result, int num)
{
	int i;
	for(i = 1; i <= num; i++)
	{
		if( result[i] && !isArcFound(i) )
		//if( result[i] )
		{
	        memset(Color, 0, sizeof(Color));
	        init_stack();
	        DFS_Visit(SortLink[i], result, SortLink[i]->tailvex);
	    }
	}
}

void debug_sortlink()
{
	int i;
	printf("SortLink:");
	for(i = 1; i < G.arcnum + 1; i ++)
	{
		printf("id:%d,m:%d | ",SortLink[i]->id, i);
	}
}

void debug_circle()
{
	int i;
	printf("circle_count:%d\n", fCircle.countCircle);
	printf("circle_index:");
	for(i = 1; i < fCircle.countCircle + 1; i++)
	{
		printf("%d,", fCircle.indexCircle[i]);
	}
	printf("\ncircle_info:");
	for(i = 0; i < fCircle.indexCircle[fCircle.countCircle]; i ++)
	{
		printf("%d,", fCircle.Circle[i]);
	}
	printf("\n");
}

void check_result(glp_prob *lp, glp_iocp *iocp)
{
	int num, i, j, index;
	int result[4001];
	int ind[3001] = {0};        //赛题规定每个顶点的出度不超过8,最多有600个顶点
    double val[3001] = {0};
    int rowIndex;
	num = glp_get_num_cols(lp);
	for(i = 1; i < num+1; i++)
	{
		//result[i] = glp_get_col_prim(lp, i )>0.9 ? 1 : 0;
		//printf("glp_col[%d]=%lf ", i, glp_get_col_prim(lp, i ));
		result[i] = glp_mip_col_val(lp, i )>0.9 ? 1 : 0;
		//printf("glp_col[%d]=%lf ", i, glp_mip_col_val(lp, i ));
		//printf("result[%d]=%d\n",i, result[i] );
	}
	memset(&fCircle, 0, sizeof(fCircle));
	findCircle(result, num);
    //debug_circle();
	while(fCircle.countCircle > 0)
	{
		for(i = 1; i <= fCircle.countCircle; i ++)
		{
		    memset(ind, 0, sizeof(ind));
		    memset(val, 0, sizeof(val));
		    index = 1;
		    for(j = fCircle.indexCircle[i - 1]; j < fCircle.indexCircle[i]; j ++)
		    {
				ind[index] = fCircle.Circle[j];
				val[index] = 1;
				//printf("linkid:%d ", SortLink[fCircle.Circle[j]]->id);
				//printf("ind[%d]=%d, val[%d]=%lf\n", index, ind[index], index, val[index]);
				index++;
			}
			rowIndex = glp_add_rows(lp, 1);
			glp_set_mat_row(lp, rowIndex, index - 1, ind, val);
            glp_set_row_bnds(lp, rowIndex, GLP_UP, index -2, index -2); 
            //printf("\n\n");
	    }
	    glp_intopt(lp, iocp);
	    //glp_simplex(lp, NULL);
	    num = glp_get_num_cols(lp);
	    for(i = 1; i < num+1; i++)
	    {
		    result[i] = glp_mip_col_val(lp, i )>0.9 ? 1 : 0;
		    //printf("glp_col[%d]=%lf ", i, glp_get_col_prim(lp, i ));
		    //printf("result[%d]=%d\n",i, result[i] );
	    }
	    memset(&fCircle, 0, sizeof(fCircle));
	    findCircle(result, num);
        //debug_circle();
	}
	/*
	printf("count:%d\n", fCircle.countCircle);
	printf(":%d\n", fCircle.indexCircle[0]);
	printf(":%d\n", fCircle.indexCircle[1]);
	printf(":%d\n", fCircle.indexCircle[2]);
	for(i = 0; i < (fCircle.indexCircle[fCircle.countCircle] ); i++)
	{
		printf("%d*", fCircle.Circle[i]);
	}
    */
	
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
    glp_prob *lp;
    glp_iocp iocp;
    glp_bfcp bfcp;
    for(i = 0; i < edge_num; i++)
    {
        str_to_int(topo[i], lid, sid, did, cost);
        //printf("read info from topo[%d]:%d  %d  %d  %d \n", i, lid, sid, did, cost);
        build_graph(lid, sid, did, cost);
    }
    demand_to_int(demand, &Start, &End, MustNode, &MustLen);
    //traverse_graph();
    
    lp = glp_create_prob();
    
    glp_init_iocp(&iocp);
    iocp.presolve = GLP_ON;
    iocp.msg_lev = GLP_MSG_OFF;
    iocp.binarize = GLP_ON;
    glp_get_bfcp(lp, &bfcp);
    
    glp_set_prob_name(lp, "mypro");
    glp_set_obj_dir(lp, GLP_MIN);
    build_pro(lp);
    glp_set_bfcp(lp, &bfcp);
    //glp_simplex(lp, NULL);
    glp_intopt(lp, &iocp);
    
    check_result(lp, &iocp);
    out_result(lp);
    glp_delete_prob(lp);
    
    printf("\nend_search_route\n");
    //testFun();
}

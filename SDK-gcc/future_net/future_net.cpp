#include "route.h"
#include "lib_io.h"
#include "lib_time.h"
#include "stdio.h"
#include "string.h"
//#include "solve.h"

#define TF  "../test-case/case0/topo.csv"
#define DF  "../test-case/case0/demand.csv"
#define RF  "../test-case/case0/sample_result.csv"


int main(int argc, char *argv[])
{
    //printf("%d", sizeof(int));
    print_time("Begin");
    char *topo[5000];
    int edge_num;
    char *demand;
    int demand_num;

    char *topo_file = argv[1];
    printf("%s", topo_file);
    //const char *topo_file = TF;
    edge_num = read_file(topo, 5000, topo_file);
    if (edge_num == 0)
    {
        printf("Please input valid topo file.\n");
        return -1;
    }
    char *demand_file = argv[2];
    //const char *demand_file = DF;
    demand_num = read_file(&demand, 1, demand_file);
    if (demand_num != 1)
    {
        printf("Please input valid demand file.\n");
        return -1;
    }

    search_route(topo, edge_num, demand);

    char *result_file = argv[3];
    //const char *result_file = RF;
    write_result(result_file);
    release_buff(topo, edge_num);
    release_buff(&demand, 1);

    print_time("End");
    //hello();

	return 0;
}


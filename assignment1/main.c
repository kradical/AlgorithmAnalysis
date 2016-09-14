#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NMAX 100

void is_valid_vertex_count(int);
int read_graph(int, int [NMAX][NMAX - 1]);
int read_vertex(int, int, int [NMAX][NMAX - 1]);
void check_degree(int, int);
int is_valid_vertex(int, int);
int read_dominating_set(int, int [NMAX]);
void build_set(int [NMAX][NMAX - 1], int[NMAX], int[NMAX]);
void check_set(int, int[NMAX]);

void check_int_fscanf(int* d) {
    if(fscanf(stdin, "%d", d) != 1) {
        fprintf(stderr, "Improper graph format, failed to read value\n");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, int* argv[]) {
    if(argc != 1) {
        fprintf(stderr, "\nUSAGE:\t./a.out 0|1\n0 for terse\n1 for verbose\n");
        exit(EXIT_FAILURE);
    }

    int vertex_count;
    int graph[NMAX][NMAX - 1] = { 0 };
    int dom_set[NMAX] = { 0 };
    int node_set[NMAX] = { 0 };

    while(fscanf(stdin, "%d", &vertex_count) > 0) {
        is_valid_vertex_count(vertex_count);
        if(!read_graph(vertex_count, graph) ||
           !read_dominating_set(vertex_count, dom_set) ||
           !is_valid_graph(vertex_count, graph)) { continue; }

        memset(node_set, 0, NMAX * sizeof(int));
        build_set(graph, dom_set, node_set);
        check_set(vertex_count, node_set);
    }

    printf("\nend of program\n");
    return 0;
}

void is_valid_vertex_count(int d) {
    if(d < 0 ) {
        fprintf(stderr, "Improper graph format, negative vertex count\n");
        exit(EXIT_FAILURE);
    }
    if(d > NMAX) {
        fprintf(stderr, "vertex count is %d max is %d. Increase NMAX and recompile.\n", d, NMAX);
        exit(EXIT_FAILURE);
    }
}

int read_graph(int vertex_count, int graph[NMAX][NMAX - 1]) {
    int is_success = 1;

    int i;
    for(i = 0; i < vertex_count; i++) {
        is_success &= read_vertex(i, vertex_count, graph);
    }
    printf("vertex_count: %d\n\n", vertex_count);
    return is_success;
}

int read_vertex(int vertex, int vertex_count, int graph[NMAX][NMAX - 1]) {
    int degree;
    int is_success = 1;

    check_int_fscanf(&degree);
    check_degree(degree, vertex_count - 1);

    int i;
    for(i = 0; i < degree; i++) {
        check_int_fscanf(&graph[vertex][i]);
        is_success &= is_valid_vertex(graph[vertex][i], vertex_count);
        printf(" %d", graph[vertex][i]);
    }
    graph[vertex][i] = -1;
    printf(" %d\n", graph[vertex][i]);

    printf("degree: %d\n", degree);
    return is_success;
}

void check_degree(int d, int vertex_count) {
    int max = vertex_count - 1;
    if(d < 0 || d > max) {
        fprintf(stderr, "Improper graph format. Degree %d is not in the range [0, %d]\n", d, max);
        exit(EXIT_FAILURE);
    }
}

int is_valid_vertex(int d, int vertex_count) {
    int max = vertex_count - 1;
    if(d < 0 || d > max) {
        fprintf(stderr, "Error-  Value %d in the certificate is not in the range [0, %d]\n", d, max);
        return 0;
    }
    return 1;
}

int read_dominating_set(int vertex_count, int dom_set[NMAX]) {
    int set_size;
    int is_success = 1;

    check_int_fscanf(&set_size);
    is_valid_vertex_count(set_size);
    printf("%d\n", set_size);

    int i;
    for(i = 0; i < set_size; i++) {
        check_int_fscanf(&dom_set[i]);
        is_success &= is_valid_vertex(dom_set[i], vertex_count);
        printf(" %d", dom_set[i]);
    }
    dom_set[i] = -1;
    printf(" %d\n\n", dom_set[i]);
    return is_success;
}

int is_valid_graph(int vertex_count, int graph[NMAX][NMAX - 1]) {
    int i, j, k, neighbour, found_i;
    for(i = 0; i < vertex_count; i++) {
        for(j = 0; j < vertex_count && graph[i][j] != -1; j++) {
            // the node i's jth neighbour search through j for i
            neighbour = graph[i][j];
            found_i = 0;
            for(k = 0; k < vertex_count && graph[neighbour][k] != -1; k++) {
                if(graph[neighbour][k] == i) {
                    found_i = 1;
                    break;
                }
            }
            if(!found_i) {
                printf("INVALID GRAPH\n");
                return 0;
            }
        }
    }

    return 1;
}

void build_set(int graph[NMAX][NMAX - 1], int dom_set[NMAX], int node_set[NMAX]) {
    int i, j;
    printf("\nNODE SET\n");
    for(i = 0; dom_set[i] != -1 && i < NMAX; i++) {
        node_set[dom_set[i]] = 1;
        printf("%d %d\n", dom_set[i], node_set[dom_set[i]]);
        for(j = 0; graph[dom_set[i]][j] != -1 && j < NMAX; j++) {
            node_set[graph[dom_set[i]][j]] = 1;
            printf("%d %d\n", graph[dom_set[i]][j], node_set[graph[dom_set[i]][j]]);
        }
    }
}

void check_set(int vertex_count, int node_set[NMAX]) {
    printf("CHECK NODE SET\n");
    int i;
    for(i = 0; i < vertex_count; i++) {
        printf("%d %d\n", i, node_set[i]);
        if(!node_set[i]) {
            printf("INVALID CERT");
        }
    }
    printf("\n");
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// This program calculates the minimum dominating set for a series of graphs.
// Graphs are provided through standard input in the format specified by assignment 2.
// The maximum size of a graph may be altered using NMAX and then recompiling.
// The DEBUG flag may be set to 1 for verbose debugging output.

#define NMAX 512
#define DEBUG 0

void check_int_scanf(int*, int);
int read_graph(int*, int[NMAX][NMAX], int);
void check_vertex_count(int, int);
void read_vertex(int, int, int[NMAX][NMAX], int);
void check_degree(int, int, int);
void check_vertex(int, int, int);
void check_graph(int, int[NMAX][NMAX], int);
void print_graph(int, int[NMAX][NMAX]);
void min_dom_set(int, int*, int[NMAX], int[NMAX], int*, int[NMAX - 1], int*, int[NMAX - 1], int, int, int[NMAX][NMAX]);
void print_dom_set(int, int, int*);

#if DEBUG
    void print_vec(int, int*);
#endif

// Utility to check if scanf failed to read a value.
// Parameters:
//   d: space to read the next integer into.
//   graph_num: the current graph number for error logging.
//
// Exits if a value is not read as this implies an invalid graph.
void check_int_scanf(int* d, int graph_num) {
    if(scanf("%d", d) != 1) {
        printf("\nImproper graph format, failed to read value.\n");
        printf("Graph   %5d: BAD GRAPH\n", graph_num);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char* argv[]) {
    if(argc != 1) {
        printf("USAGE:\t%s", argv[0]);
        return EXIT_FAILURE;
    }

    int vertex_count; // graph is vertex_count x vertex_count in size
    int G[NMAX][NMAX]; // adjacency matrix form of a graph
    int n_dominated; // number of dominated vertices
    int num_choice[NMAX]; // number of times it could be dominated
    int num_dom[NMAX]; // number of times it is dominated
    int size; // size of the current dominating set
    int dom[NMAX - 1]; // current dominating set
    int min_size; // size of minimum dominating set so far
    int min_dom[NMAX - 1]; // minimum dominating set so far

    int graph_num = 1;
    int i, j, deg_i, max_deg;
    while(read_graph(&vertex_count, G, graph_num)) {
        check_graph(vertex_count, G, graph_num);
        print_graph(vertex_count, G);

        min_dom_set(0, &n_dominated, num_choice, num_dom, &size, dom, &min_size, min_dom, vertex_count, max_deg, G);
        print_dom_set(min_size, vertex_count, min_dom);

        graph_num++;
    }

    return EXIT_SUCCESS;
}

// Reads a graph from standard in
// Parameters:
//   vertex_count: a location to store the number of vertices in the graph.
//   G: a location to store the current graph adjacency matrix.
//   graph_num: the current graph number for error logging.
//
// Returns 1 if a graph was successfully read and 0 otherwise.
int read_graph(int* vertex_count, int G[NMAX][NMAX], int graph_num) {
    if(scanf("%d", vertex_count) != 1) {
        return 0;
    }

    check_vertex_count(*vertex_count, graph_num);

    // only initialize the first vertex_count rows of the adjacency matrix
    memset(G, 0, (*vertex_count) * NMAX * sizeof(int));

    int i;
    for(i = 0; i < *vertex_count; i++) {
        read_vertex(i, *vertex_count, G, graph_num);
    }

    return 1;
}

// Checks if the number of vertices is valid.
// Parameters:
//   vertex_count: the vertex_count to check
//   graph_num: the current graph number for error logging.
//
// Exits if graph is not valid.
void check_vertex_count(int vertex_count, int graph_num) {
    if(vertex_count < 0 ) {
        printf("Improper graph format, negative vertex count\n");
        printf("Graph   %5d: BAD GRAPH\n", graph_num);
        exit(EXIT_FAILURE);
    }
    if(vertex_count > NMAX) {
        printf("vertex count is %d max is %d. Increase NMAX and recompile.\n", vertex_count, NMAX);
        printf("Graph   %5d: BAD GRAPH\n", graph_num);
        exit(EXIT_FAILURE);
    }
}

// Reads information for a vertex into the adjacency matrix.
// Parameters:
//   vertex: the current vertex number.
//   vertex_count: the total vertices in G.
//   G: the adjacency matrix to read vertex information into.
//   graph_num: the current graph number for error logging.
//
// Exits if the graph is not valid
void read_vertex(int vertex, int vertex_count, int G[NMAX][NMAX], int graph_num) {
    int degree;

    check_int_scanf(&degree, graph_num);
    check_degree(degree, vertex_count, graph_num);

    int i, neighbour;
    for(i = 0; i < degree; i++) {
        check_int_scanf(&neighbour, graph_num);
        check_vertex(neighbour, vertex_count, graph_num);
        
        // a multiple-edge (node has multiple edges to the same neighbouring node)
        if(G[vertex][neighbour] == 1) {
            printf("*** Error- graph is not simple, multiple edges between node %5d and node %5d\n", vertex, neighbour);
            printf("Graph   %5d: BAD GRAPH\n", graph_num);
            exit(EXIT_FAILURE);
        } else {
            G[vertex][neighbour] = 1;
        }
    }
}

// Checks if the degree of a vertex is valid.
// Parameters:
//   degree: the degree of a vertex being checked.
//   vertex_count: the total number of vertices in the graph.
//   graph_num: the current graph number for error logging.
//
// Exits if the degree is invalid
void check_degree(int degree, int vertex_count, int graph_num) {
    int max = vertex_count - 1;
    if(degree < 0 || degree > max) {
        printf("*** Error- invalid graph, degree %5d is not in the range [0, %5d]\n", degree, vertex_count - 1);
        printf("Graph   %5d: BAD GRAPH\n", graph_num);
        exit(EXIT_FAILURE);
    }
}

// Checks if the value of a vertex is valid.
// Parameters:
//   vertex: the vertex number being checked.
//   vertex_count: the total number of vertices in the graph.
//   graph_num: the current graph number for error logging.
//
// Exits if the vertex is invalid
void check_vertex(int vertex, int vertex_count, int graph_num) {
    int max = vertex_count - 1;
    if(vertex < 0 || vertex > max) {
        printf("*** Error- invalid graph, value %5d is not in the range [0, %5d]\n", vertex, vertex_count - 1);
        printf("Graph   %5d: BAD GRAPH\n", graph_num);
        exit(EXIT_FAILURE);
    }
}

// Checks the validity of a graph (should be simple).
// Parameters:
//   vertex_count: the total number of vertices in the graph G.
//   G: the current graph being checked for validity.
//   graph_num: the current graph number for error logging.
//
// Exits if the graph is invalid.
void check_graph(int vertex_count, int G[NMAX][NMAX], int graph_num) {
    int i, j;
    for(i = 0; i < vertex_count; i++) {
        for(j = 0; j < vertex_count; j++) {
            if(G[i][j] != G[j][i]) {
                printf("*** Error- adjacency matrix is not symmetric: A[%5d][%5d] != A[%5d][%5d]\n", i, j, j, i);
                printf("Graph   %5d: BAD GRAPH\n", graph_num);
                exit(EXIT_FAILURE);
            }
        }

        if(G[i][i] == 1) {
            printf("*** Error- graph is not simple, loop at node %5d\n", i);
            printf("Graph   %5d: BAD GRAPH\n", graph_num);
            exit(EXIT_FAILURE);
        }
    }
}

// Prints the graph in an adjacency list format with degree and vertex count.
// Parameters:
//   vertex_count: the total number of vertices in the graph.
//   G: the current graph being printed.
void print_graph(int vertex_count, int G[NMAX][NMAX]) {
    printf("%5d\n", vertex_count);    

    int i, j, deg_i;
    for(i = 0; i < vertex_count; i++) {
        deg_i = 0;
        for(j = 0; j < vertex_count; j++) {
            if(G[i][j] == 1) {
                deg_i++;
            }
        }
        printf("%5d", deg_i);

        for(j = 0; j < vertex_count; j++) {
            if(G[i][j] == 1) {
                printf("%5d", j);
            }
        }
        printf("\n");
    }
    printf("\n");
}

// Recursively find the minimum dominating set
// Parameters:
//   level: the current level of recursion, corresponds to the vertex being coloured.
//   n_dom: the number of dominated vertices.
//   num_choice: an array of number of times each vertex could be dominated.
//   num_dom: an array of number of times each vertex is dominated.
//   size: the size of the current dominating set.
//   dom: the current dominating set
//   min_size: the size of the current minimum dominating set.
//   min_dom: the current minimum dominating set.
//   vertex_count: the total number of vertices in the graph G.
//   max_deg: the maximum degree of any vertice in graph G.
//   G: the graph that a minimum dominating set is being found for.
//
// Populates min_size/min_dom with the final result.
//
// This algorithm is based on pseudocode from Wendy Myrvold's slides in CSC 425.
// The colors refer to vertex states outlined in the slides.
void min_dom_set(int level, int* n_dom, int num_choice[NMAX], int num_dom[NMAX], int* size,
    int dom[NMAX - 1], int* min_size, int min_dom[NMAX - 1], int vertex_count, int max_deg, int G[NMAX][NMAX]) {
    
    #if DEBUG
        printf("\nLEVEL: %d\n", level);
        printf("# vertices dominated: %d\n", *n_dom);
        printf("# choices per vertex:\n");
        print_vec(vertex_count, num_choice);
        printf("# times dominated:\n");
        print_vec(vertex_count, num_dom);
        printf("SIZE: %d MIN_SIZE: %d\n", *size, *min_size);
    #endif

    // initialize all the data
    if(level == 0) {
        int i;
        for(i = 0; i < vertex_count; i++) {
            G[i][i] = 1;
        }

        *n_dom = 0;
        
        memset(num_choice, 0, vertex_count * sizeof(int));
        max_deg = 0;
        int j, deg_i;
        for(i = 0; i < vertex_count; i++) {
            deg_i = 0;
            for(j = 0; j < vertex_count; j++) {
                // will be + 1 because of the all 1 diagonal
                deg_i += G[i][j];
            }
            num_choice[i] = deg_i;
            if(deg_i > max_deg) {
                max_deg = deg_i;
            }
        }

        memset(num_dom, 0, vertex_count * sizeof(int));
        *size = 0;
        memset(dom, 0, (vertex_count - 1) * sizeof(int));
        *min_size = vertex_count;

        memset(min_dom, 0, (vertex_count - 1) * sizeof(int));
        for(i = 0; i < vertex_count; i++) {
            min_dom[i] = i;
        }
    }

    int i; // loop counter
    for(i = 0; i < vertex_count; i++) {
        if(!num_choice[i]) {
            return;
        }
    }

    int u = vertex_count - *n_dom;
    int n_extra = u / max_deg + (u % max_deg != 0);

    if(*size + n_extra >= *min_size) {
        return;
    }

    if(level == vertex_count || *n_dom == vertex_count) {
        if(*size < *min_size) {
            memcpy(min_dom, dom, (vertex_count - 1) * sizeof(int));
            *min_size = *size;
        }
        return;
    }

    // make vertex level blue
    for(i = 0; i < vertex_count; i++) {
        if(G[level][i]) {
            num_choice[i]--;
        };
    }

    min_dom_set(level + 1, n_dom, num_choice, num_dom, size, dom, min_size, min_dom, vertex_count, max_deg, G);

    // undo blue colouring
    for(i = 0; i < vertex_count; i++) {
        if(G[level][i]) {
            num_choice[i]++;
        };
    }

    // make vertex level red
    dom[level] = 1;
    *size += 1;
    for(i = 0; i < vertex_count; i++) {
        if(G[level][i]) {
            num_dom[i]++;
        };
    }
    *n_dom = 0;
    for(i = 0; i < vertex_count; i++) {
        if(num_dom[i]) {
            *n_dom += 1;
        }
    }

    min_dom_set(level + 1, n_dom, num_choice, num_dom, size, dom, min_size, min_dom, vertex_count, max_deg, G);

    // undo red colouring
    dom[level] = 0;
    *size -= 1;
    for(i = 0; i < vertex_count; i++) {
        if(G[level][i]) {
            if(num_dom[i]) {
                *n_dom -= 1;
            }
            num_dom[i]--;
        };
    }

    // return the graph to its original state.
    for(i = 0; i < vertex_count; i++) {
        G[i][i] = 0;
    }
}

// Print a dominating set.
// Parameters:
//   size: the size of the dominating set.
//   vertex_count: the total number of vertices in the graph.
//   dom: the dominating set.
void print_dom_set(int size, int vertex_count, int* dom) {
    printf("%5d\n", size);

    int i;
    for(i = 0; i < vertex_count; i++) {
        if(dom[i]) {
            printf("%5d ", i);
        }
    }

    printf("\n\n");
}

#if DEBUG
// Utility for printing arrays.
// Parameters:
//   size: the size of the array to print.
//   vec: the array to be printed.
void print_vec(int size, int* vec) {
    int i;
    for(i = 0; i < size; i++) {
        printf("%d: %d\n", i, vec[i]);
    }
}
#endif

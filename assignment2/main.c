#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NMAX 512
#define DEBUG 1

void is_valid_vertex_count(int);
void read_graph(int, int[NMAX][NMAX], int);
void read_vertex(int, int, int[NMAX][NMAX], int);
void check_degree(int, int, int);
void check_symmetric(int, int[NMAX][NMAX], int);
void check_vertex(int, int, int, int);
void initialize_data(int*, int[NMAX], int[NMAX], int*, int[NMAX - 1], int*, int[NMAX - 1]);
void min_dom_set(int, int*, int[NMAX], int[NMAX], int*, int[NMAX - 1], int*, int[NMAX - 1], int, int, int[NMAX][NMAX]);

#if DEBUG
    void print_vec(int, int*);
#endif
// Utility to check if fscanf failed to read a value.
// Exits if fscanf(...) == 0 as this implies an invalid graph.
void check_int_fscanf(int* d) {
    if(fscanf(stdin, "%d", d) != 1) {
        printf("\nImproper graph format, failed to read value\n");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char* argv[]) {
    if(argc != 1) {
        fprintf(stderr, "USAGE:\t%s", argv[0]);
        return EXIT_FAILURE;
    }

    int vertex_count; // graph is vertex_count x vertex_count in size
    
    int graph[NMAX][NMAX] = { 0 }; // adjacency matrix
    int n_dominated = 0; // number of dominated vertices
    int num_choice[NMAX] = { 0 }; // number of times it could be dominated
    int num_dom[NMAX] = { 0 }; // number of times it is dominated
    int size = 0;
    int dom[NMAX - 1] = { 0 };
    int min_size = 0; // size of minimum dominating set so far
    int min_dom[NMAX - 1] = { 0 }; // minimum dominating set so far


    int i, j, deg_i, max_deg, graph_ndx = 1;
    while(fscanf(stdin, "%d", &vertex_count) == 1) {
        memset(graph, 0, NMAX * NMAX * sizeof(int));
        read_graph(vertex_count, graph, graph_ndx);
        check_symmetric(vertex_count, graph, graph_ndx);

        // initialize all the data
        n_dominated = 0;
        
        memset(num_choice, 0, NMAX * sizeof(int));
        max_deg = 0;
        for(i = 0; i < vertex_count; i++) {
            deg_i = 0;
            for(j = 0; j < vertex_count; j++) {
                // will be + 1 because of the all 1 diagonal
                deg_i += graph[i][j];
            }
            num_choice[i] = deg_i;
            if(deg_i > max_deg) {
                max_deg = deg_i;
            }
        }

        memset(num_dom, 0, NMAX * sizeof(int));
        size = 0;
        memset(dom, 0, (NMAX - 1) * sizeof(int));
        min_size = vertex_count;

        memset(min_dom, 0, (NMAX - 1) * sizeof(int));
        for(i = 0; i < vertex_count; i++) {
            min_dom[i] = i;
        }

        min_dom_set(0, &n_dominated, num_choice, num_dom, &size, dom, &min_size, min_dom, vertex_count, max_deg, graph);
        printf("\n%d\n", min_size);
        for(i = 0; i < min_size; i++) {
            printf("%5d", min_dom[i]);
        }
        printf("\n");
        graph_ndx++;
    }
    return EXIT_SUCCESS;
}

// Returns the node that is invalid or 0 if they are all valid.
// 0 will always be a valid node in a well formed graph.
void read_graph(int vertex_count, int graph[NMAX][NMAX], int graph_ndx) {
    is_valid_vertex_count(vertex_count);
    printf("%5d\n", vertex_count);

    int i;
    for(i = 0; i < vertex_count; i++) {
        read_vertex(i, vertex_count, graph, graph_ndx);
    }
}

// Checks if the number of verties is valid.
// Exits if graph is not valid.
void is_valid_vertex_count(int d) {
    if(d < 0 ) {
        printf("Improper graph format, negative vertex count\n");
        exit(EXIT_FAILURE);
    }
    if(d > NMAX) {
        printf("vertex count is %d max is %d. Increase NMAX and recompile.\n", d, NMAX);
        exit(EXIT_FAILURE);
    }
}

// Reads [degree] neighbours for one vertex.
void read_vertex(int vertex, int vertex_count, int graph[NMAX][NMAX], int graph_ndx) {
    int degree;

    check_int_fscanf(&degree);
    check_degree(degree, vertex_count, graph_ndx);
    printf("%5d ", degree);

    // a node dominates itself so we set it here
    graph[vertex][vertex] = 1;

    int found_edges[NMAX - 1] = { 0 };
    int i, neighbour;
    for(i = 0; i < degree; i++) {
        check_int_fscanf(&neighbour);
        check_vertex(vertex, neighbour, vertex_count, graph_ndx);
        graph[vertex][neighbour] = 1;

        // a multiple-edge (node has multiple edges to the same neighbouring node)
        if(found_edges[neighbour]) {
            printf("*** Error- graph is not simple, multiple edges between node %5d and node %5d\n", i, neighbour);
            printf("Graph   %5d: BAD GRAPH\n", graph_ndx);
            exit(EXIT_FAILURE);
        } else {
            found_edges[neighbour] = 1;
        }
        printf("%5d ", neighbour);
    }
    printf("\n");
}

// Checks that the degree for a vertex is valid.
// Exits if graph is invalid.
void check_degree(int d, int vertex_count, int graph_ndx) {
    int max = vertex_count - 1;
    if(d < 0 || d > max) {
        printf("Improper graph format. Degree %d is not in the range [0, %d]\n", d, max);
        printf("Graph   %5d: BAD GRAPH\n", graph_ndx);
        exit(EXIT_FAILURE);
    }
}

// Checks that a neighbour vertex is valid.
// Exits if graph is invalid.
void check_vertex(int vertex, int neighbour, int vertex_count, int graph_ndx) {
    int max = vertex_count - 1;
    if(neighbour < 0 || neighbour > max) {
        printf("*** Error- invalid graph, value %5d is not in the range [0, %5d]\n", neighbour, vertex_count - 1);
        printf("Graph   %5d: BAD GRAPH\n", graph_ndx);
        exit(EXIT_FAILURE);
    }

    if(vertex == neighbour) {
        printf("*** Error- graph is not simple, loop at node %5d\n", vertex);
        printf("Graph   %5d: BAD GRAPH\n", graph_ndx);
        exit(EXIT_FAILURE);
    }
}

// Checks the validity of a graph (should be simple).
// Detects self loops, not mirrored edges, and double edges
// Returns 1 for valid and 0 for invalid.
void check_symmetric(int vertex_count, int graph[NMAX][NMAX], int graph_ndx) {
    int i, j;
    for(i = 0; i < vertex_count; i++) {
        for(j = 0; j < vertex_count; j++) {
            if(graph[i][j] != graph[j][i]) {
                printf("*** Error- adjacency matrix is not symmetric: A[%5d][%5d] != A[%5d][%5d]\n", i, j, j, i);
                printf("Graph   %5d: BAD GRAPH\n", graph_ndx);
                exit(EXIT_FAILURE);
            }
        }
    }
}

// recursively find the minimum dominating set
// try to decide the status of vertex "level"
void min_dom_set(int level, int* n_dom, int num_choice[NMAX], int num_dom[NMAX], int* size,
    int dom[NMAX - 1], int* min_size, int min_dom[NMAX - 1], int vertex_count, int max_deg, int graph[NMAX][NMAX]) {
    
    #if DEBUG
        printf("\nLEVEL: %d\n", level);
        printf("# vertices dominated: %d\n", *n_dom);
        printf("# choices per vertex:\n");
        print_vec(vertex_count, num_choice);
        printf("# times dominated:\n");
        print_vec(vertex_count, num_dom);
    #endif

    int i; // loop counter
    for(i = 0; i < vertex_count; i++) {
        if(num_choice[i] == 0) {
            return;
        }
    }

    int n_extra = *n_dom / max_deg + (*n_dom % max_deg != 0);

    if(*size + n_extra >= *min_size) {
        return;
    }

    if(*n_dom == vertex_count) {
        if(*size < *min_size) {
            // TODO correct call to memcpy
            memcpy(min_dom, dom, (NMAX - 1) * sizeof(int));
            *min_size = *size;
        }
        return;
    }

    // make vertex level blue
    for(i = 0; i < vertex_count; i++) {
        if(graph[level][i]) {
            num_choice[i]--;
        };
    }

    min_dom_set(level + 1, n_dom, num_choice, num_dom, size, dom, min_size, min_dom, vertex_count, max_deg, graph);

    for(i = 0; i < vertex_count; i++) {
        if(graph[level][i]) {
            num_choice[i]++;
        };
    }

    // make vertex level red
    dom[level] = 1;
    for(i = 0; i < vertex_count; i++) {
        if(graph[level][i]) {
            if(!num_dom[i]) {
                *n_dom++;
            }
            num_dom[i]++;
        };
    }

    min_dom_set(level + 1, n_dom, num_choice, num_dom, size, dom, min_size, min_dom, vertex_count, max_deg, graph);

    dom[level] = 0;
    for(i = 0; i < vertex_count; i++) {
        if(graph[level][i]) {
            if(num_dom[i]) {
                *n_dom--;
            }
            num_dom[i]--;
        };
    }
}

#if DEBUG
void print_vec(int size, int* vec) {
    int i;
    for(i = 0; i < size; i++) {
        printf("%d: %d\n", i, vec[i]);
    }
}
#endif

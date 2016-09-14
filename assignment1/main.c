#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NMAX 100

int NMAX_DIGITS = 1;
int DEGREE_DIGITS = 1;
char verbose;

void set_digit_counts(void);
void is_valid_vertex_count(int);
int read_graph(int, int [NMAX][NMAX]);
int read_vertex(int, int, int [NMAX][NMAX]);
void check_degree(int, int);
int is_valid_vertex(int, int);
int read_dominating_set(int, int [NMAX + 1]);
void build_set(int [NMAX][NMAX], int[NMAX + 1], int[NMAX]);
int check_set(int, int[NMAX]);

void check_int_fscanf(int* d) {
    if(fscanf(stdin, "%d", d) != 1) {
        if(verbose) {
            printf("Improper graph format, failed to read value\n");
        } else {
            printf("  -1\n");
        }
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char* argv[]) {
    if(argc != 2) {
        fprintf(stderr, "\nUSAGE:\t./a.out 0|1\n0 for terse\n1 for verbose\n");
        exit(EXIT_FAILURE);
    }

    verbose = atoi(argv[argc - 1]);

    set_digit_counts();

    int vertex_count;
    int graph[NMAX][NMAX] = { 0 };
    int dom_set[NMAX + 1] = { 0 };
    int node_set[NMAX] = { 0 };

    int graph_ndx = 0;
    int invalid_node, invalid_dom_node, d;
    while(fscanf(stdin, "%d", &vertex_count) > 0) {
        is_valid_vertex_count(vertex_count);

        graph_ndx++;
        if(verbose) {
            printf("Graph %*d:\n", NMAX_DIGITS, graph_ndx);
        } else {
            printf("  %*d", NMAX_DIGITS, graph_ndx);
        }

        invalid_node = read_graph(vertex_count, graph);
        if(!invalid_node) {
            if(!is_valid_graph(vertex_count, graph)) {
                if(verbose) {
                    printf("Graph   %*d: BAD GRAPH\n", NMAX_DIGITS, graph_ndx);
                }
                return EXIT_FAILURE;
            }
        }
        invalid_dom_node = read_dominating_set(vertex_count, dom_set);

        if(!invalid_node && !invalid_dom_node) {
            build_set(graph, dom_set, node_set);

            if(check_set(vertex_count, node_set)) {
                if(verbose) {
                    printf("Graph   %*d: OK\n", NMAX_DIGITS, graph_ndx);
                } else {
                    printf("   1\n");
                }
            } else {
                if(verbose) {
                    printf("Graph   %*d: BAD CERTIFICATE\n", NMAX_DIGITS, graph_ndx);
                } else {
                    printf("   0\n");
                }
            }
        } else {
            d = invalid_node ? invalid_node : invalid_dom_node;
            if(verbose) {
                printf("Error- Value %*d in the certificate is not in the range [0, %*d]\n", NMAX_DIGITS, d, NMAX_DIGITS, vertex_count - 1);
                printf("Graph   %*d: BAD CERTIFICATE\n", NMAX_DIGITS, graph_ndx);
            } else {
                printf("   0\n");
            }
        }
        if(verbose) {
            printf("=============================\n");
        }

        // Don't have to reset dom_set and graph because all values
        // will be set and then a trailing -1
        memset(node_set, 0, NMAX * sizeof(int));
    }

    return EXIT_SUCCESS;
}

void set_digit_counts() {
    // Calculate number of digits for formatting
    int nmax = NMAX;
    while(nmax /= 10) {
        NMAX_DIGITS++;
    }

    int neighbour_count = NMAX - 1;
    while(neighbour_count /= 10) {
        DEGREE_DIGITS++;
    }
}

void is_valid_vertex_count(int d) {
    if(d < 0 ) {
        if(verbose) {
            printf("Improper graph format, negative vertex count\n");
        } else {
            printf("  -1\n");
        }

        exit(EXIT_FAILURE);
    }
    if(d > NMAX) {
        if(verbose) {
            printf("vertex count is %d max is %d. Increase NMAX and recompile.\n", d, NMAX);
        } else {
            printf("  -1\n");
        }
        exit(EXIT_FAILURE);
    }
}

// returns the node that is invalid or 0 if they are all valid
// 0 will always be a valid node in a well formed graph
int read_graph(int vertex_count, int graph[NMAX][NMAX]) {
    int invalid_node = 0;

    int i;
    for(i = 0; i < vertex_count; i++) {
        invalid_node = read_vertex(i, vertex_count, graph);
    }

    return invalid_node;
}

// returns the node that is invalid or 0 if they are all valid
// 0 will always be a valid node in a well formed graph
int read_vertex(int vertex, int vertex_count, int graph[NMAX][NMAX]) {
    int degree;
    int invalid_node = 0;

    check_int_fscanf(&degree);
    check_degree(degree, vertex_count - 1);

    if(verbose) {
        printf("%*d(%*d):", NMAX_DIGITS, vertex, DEGREE_DIGITS, degree);
    }
    int i;
    for(i = 0; i < degree; i++) {
        check_int_fscanf(&graph[vertex][i]);
        invalid_node = is_valid_vertex(graph[vertex][i], vertex_count) ? graph[vertex][i] : invalid_node;
        if(verbose) {
            printf(" %*d", NMAX_DIGITS, graph[vertex][i]);
        }
    }
    graph[vertex][i] = -1;

    if(verbose) {
        printf("\n");
    }
    return invalid_node;
}

void check_degree(int d, int vertex_count) {
    int max = vertex_count - 1;
    if(d < 0 || d > max) {
        if(verbose) {
            printf("Improper graph format. Degree %d is not in the range [0, %d]\n", d, max);
        } else {
            printf("  -1\n");
        }
        exit(EXIT_FAILURE);
    }
}

// returns the node that is invalid or 0
// 0 will always be a valid node in a well formed graph
int is_valid_vertex(int d, int vertex_count) {
    int max = vertex_count - 1;
    if(d < 0 || d > max) {
        return d;
    }
    return 0;
}

// returns the node that is invalid or 0 if they are all valid
// 0 will always be a valid node in a well formed graph
int read_dominating_set(int vertex_count, int dom_set[NMAX + 1]) {
    int set_size;
    int invalid_node = 0;

    check_int_fscanf(&set_size);
    is_valid_vertex_count(set_size);
    if(verbose) {
        printf("Proposed dominating set:\n");
    }

    int i;
    for(i = 0; i < set_size; i++) {
        check_int_fscanf(&dom_set[i]);
        invalid_node = is_valid_vertex(dom_set[i], vertex_count) ? dom_set[i] : invalid_node;
        if(verbose) {
            printf("%*d ", NMAX_DIGITS, dom_set[i]);
        }
    }
    dom_set[i] = -1;

    if(verbose) {
        printf("\n");
    }
    return invalid_node;
}

int is_valid_graph(int vertex_count, int graph[NMAX][NMAX]) {
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
                if(verbose) {
                    printf("*** Error- adjacency matrix is not symmetric: A[%*d][%*d] = 1, A[%*d][%*d] = 0\n",
                        DEGREE_DIGITS, i, DEGREE_DIGITS, neighbour, DEGREE_DIGITS, neighbour, DEGREE_DIGITS, i);
                } else {
                    printf("  -1\n");
                }

                return 0;
            }
        }
    }
    return 1;
}

void build_set(int graph[NMAX][NMAX], int dom_set[NMAX + 1], int node_set[NMAX]) {
    int i, j;

    for(i = 0; dom_set[i] != -1 && i < NMAX; i++) {
        node_set[dom_set[i]] = 1;
        for(j = 0; graph[dom_set[i]][j] != -1 && j < NMAX; j++) {
            node_set[graph[dom_set[i]][j]] = 1;
        }
    }
}

int check_set(int vertex_count, int node_set[NMAX]) {
    int i;
    for(i = 0; i < vertex_count; i++) {
        if(!node_set[i]) {
            if(verbose) {
                printf("Error- Vertex %*d is not dominated\n", NMAX_DIGITS, i);
            }
            return 0;
        }
    }

    return 1;
}

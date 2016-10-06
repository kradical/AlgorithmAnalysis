#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NMAX 512
#define SENTINEL (NMAX + 1)

void is_valid_vertex_count(int);
void read_graph(int, int[NMAX][NMAX]);
void read_vertex(int, int, int[NMAX][NMAX]);
void check_degree(int, int);
int is_valid_graph(int, int[NMAX][NMAX]);
int is_valid_vertex(int, int);

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

    int vertex_count;
    // adjacency list format, room for all vertices' neighbours plus 1 for writing a negative 1
    int graph[NMAX][NMAX] = { 0 };

    int graph_ndx = 0;
    int invalid_node, invalid_dom_node, d;
    while(fscanf(stdin, "%d", &vertex_count) == 1) {
        graph_ndx++;

        read_graph(vertex_count, graph);
        if(!is_valid_graph(vertex_count, graph)) {
            printf("Graph   %5d: BAD GRAPH\n", graph_ndx);
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
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

// Returns the node that is invalid or 0 if they are all valid.
// 0 will always be a valid node in a well formed graph.
void read_graph(int vertex_count, int graph[NMAX][NMAX]) {
    is_valid_vertex_count(vertex_count);
    printf("%5d\n", vertex_count);

    int i;
    for(i = 0; i < vertex_count; i++) {
        read_vertex(i, vertex_count, graph);
    }
}

// Reads [degree] neighbours for one vertex.
void read_vertex(int vertex, int vertex_count, int graph[NMAX][NMAX]) {
    int degree;

    check_int_fscanf(&degree);
    check_degree(degree, vertex_count);
    printf("%5d ", degree);

    int i;
    for(i = 0; i < degree; i++) {
        check_int_fscanf(&graph[vertex][i]);
        printf("%5d ", graph[vertex][i]);
    }
    graph[vertex][i] = SENTINEL;
    printf("\n");
}

// Checks that the degree for a vertex is valid.
// Exits if graph is invalid.
void check_degree(int d, int vertex_count) {
    int max = vertex_count - 1;
    if(d < 0 || d > max) {
        printf("Improper graph format. Degree %d is not in the range [0, %d]\n", d, max);
        exit(EXIT_FAILURE);
    }
}

// Checks the validity of a graph (should be simple).
// Detects self loops, not mirrored edges, and double edges
// Returns 1 for valid and 0 for invalid.
int is_valid_graph(int vertex_count, int graph[NMAX][NMAX]) {
    int found_edges[NMAX - 1]; // initialized at top of for loop

    int i, j, k, neighbour, found_i;
    for(i = 0; i < vertex_count; i++) {
        memset(found_edges, 0, (NMAX - 1) * sizeof(int));

        // Go through the adjacency list until sentinel
        // (bound by vertex count to prevent access to unowned memory)
        for(j = 0; j < vertex_count && graph[i][j] != SENTINEL; j++) {
            // the node i's jth neighbour search through j for i
            neighbour = graph[i][j];

            // vertex value is out of range
            if(neighbour < 0 || neighbour >= vertex_count) {
                printf("*** Error- invalid graph, value %5d is not in the range [0, %5d]\n", neighbour, vertex_count - 1);
                return 0;
            }

            // a loop, (node has itself in the adjacency list)
            if(neighbour == i) {
                printf("*** Error- graph is not simple, loop at node %5d\n", i);
                return 0;
            }

            // a multiple-edge (node has multiple edges to the same neighbouring node)
            if(found_edges[neighbour]) {
                printf("*** Error- graph is not simple, multiple edges between node %5d and node %5d\n", i, neighbour);
                return 0;
            } else {
                found_edges[neighbour] = 1;
            }

            found_i = 0;
            for(k = 0; k < vertex_count && graph[neighbour][k] != SENTINEL; k++) {
                if(graph[neighbour][k] == i) {
                    found_i = 1;
                    break;
                }
            }
            if(!found_i) {
                printf("*** Error- adjacency matrix is not symmetric: A[%5d][%5d] = 1, A[%5d][%5d] = 0\n",
                    i, neighbour, neighbour, i);
                return 0;
            }
        }
    }
    return 1;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// This program calculates the minimum dominating set for a series of graphs.
// Graphs are provided through standard input in the format specified by assignment 2.
// The maximum size of a graph may be altered using NMAX and then recompiling.
// The DEBUG flag may be set to 1 for verbose debugging output.

#define DEBUG 0
#define NMAX 128
#define MMAX ((NMAX +31)/ 32)

/* set operations (setadd is its address, pos is the bit number): */
#define SETWD(pos) ((pos)>>5)    /* number of longword containing bit pos */
#define SETBT(pos) ((pos)&037)   /* position within longword of bit pos */
#define ADD_ELEMENT(setadd,pos) ((setadd)[SETWD(pos)] |= bit[SETBT(pos)])
#define DEL_ELEMENT(setadd,pos) ((setadd)[SETWD(pos)] &= ~bit[SETBT(pos)])
#define IS_ELEMENT(setadd,pos) ((setadd)[SETWD(pos)] & bit[SETBT(pos)])

/* number of 1-bits in longword x */
#define POP_COUNT(x) bytecount[(x)>>24 & 0377] + bytecount[(x)>>16 & 0377] \
                        + bytecount[(x)>>8 & 0377] + bytecount[(x) & 0377]

int bytecount[] = {
    0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,
    1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
    1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
    2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
    1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
    2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
    2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
    3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
    1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
    2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
    2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
    3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
    2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
    3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
    3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
    4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8
};

int bit[] = {
    020000000000,010000000000,04000000000,02000000000,01000000000,
    0400000000,0200000000,0100000000,040000000,020000000,010000000,
    04000000,02000000,01000000,0400000,0200000,0100000,040000,020000,
    010000,04000,02000,01000,0400,0200,0100,040,020,010,04,02,01
};

void check_int_scanf(int*, int);
int read_graph(int*, int*, int[NMAX][MMAX], int);
void check_vertex_count(int, int);
void read_vertex(int, int, int[NMAX][MMAX], int);
void check_degree(int, int, int);
void check_vertex(int, int, int);
void check_graph(int, int[NMAX][MMAX], int);
void print_graph(int, int[NMAX][MMAX]);
void initialize_p(int, int[NMAX]);
void print_set(int, int*);
int find_dom_set(int, int*, int[NMAX], int[NMAX], int*, int[MMAX], int, int, int[NMAX][MMAX], int[NMAX]);
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
    int m;
    int G[NMAX][MMAX]; // compressed adjacency matrix form of a graph
    int n_dominated; // number of dominated vertices
    int num_choice[NMAX]; // number of times it could be dominated
    int num_dom[NMAX]; // number of times it is dominated
    int size; // size of the current dominating set
    int dom[MMAX]; // current dominating set
    int p[NMAX];

    int graph_num = 1;
    int i, j, deg_i, max_deg;
    while(read_graph(&vertex_count, &m, G, graph_num)) {
        check_graph(vertex_count, G, graph_num);
        print_graph(vertex_count, G);

        initialize_p(vertex_count, p);
        find_dom_set(0, &n_dominated, num_choice, num_dom, &size, dom, vertex_count, max_deg, G, p);
        print_dom_set(size, vertex_count, dom);

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
int read_graph(int* vertex_count, int* m, int G[NMAX][MMAX], int graph_num) {
    if(scanf("%d", vertex_count) != 1) {
        return 0;
    }
    *m = (*vertex_count + 31) / 32;

    check_vertex_count(*vertex_count, graph_num);

    // initialize graph to have no edges
    int i, j;
    for(i = 0; i < *vertex_count; i++) {
        for(j = 0; j < *m; j++) {
            G[i][j]= 0;
        }
    }

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
void read_vertex(int vertex, int vertex_count, int G[NMAX][MMAX], int graph_num) {
    int degree;

    check_int_scanf(&degree, graph_num);
    check_degree(degree, vertex_count, graph_num);

    int i, neighbour;
    for(i = 0; i < degree; i++) {
        check_int_scanf(&neighbour, graph_num);
        check_vertex(neighbour, vertex_count, graph_num);
        
        // a multiple-edge (node has multiple edges to the same neighbouring node)
        if(IS_ELEMENT(G[vertex], neighbour)) {
            printf("*** Error- graph is not simple, multiple edges between node %5d and node %5d\n", vertex, neighbour);
            printf("Graph   %5d: BAD GRAPH\n", graph_num);
            exit(EXIT_FAILURE);
        } else {
            ADD_ELEMENT(G[vertex], neighbour);
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
void check_graph(int vertex_count, int G[NMAX][MMAX], int graph_num) {
    int i, j;
    for(i = 0; i < vertex_count; i++) {
        for(j = 0; j < vertex_count; j++) {
            if(IS_ELEMENT(G[i], j) && !IS_ELEMENT(G[j], i)) {
                printf("*** Error- adjacency matrix is not symmetric: A[%5d][%5d] != A[%5d][%5d]\n", i, j, j, i);
                printf("Graph   %5d: BAD GRAPH\n", graph_num);
                exit(EXIT_FAILURE);
            }
        }

        if(IS_ELEMENT(G[i], i)) {
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
void print_graph(int vertex_count, int G[NMAX][MMAX]) {
    int i, d;

    printf("%5d\n", vertex_count);
    for (i = 0; i < vertex_count; i++) {
        d = set_size(vertex_count, G[i]);
        printf("%5d", d);
        print_set(vertex_count, G[i]);
    }
}

// Randomize method taken from wendy myrvold's 425 slides.
void initialize_p(int vertex_count, int p[NMAX]) {
    int i;
    for(i = 0; i < vertex_count; i++) {
        p[i] = i;
    }

    srand(time(NULL));
    for (i = vertex_count - 1; i >= 1; i--) {
        int r = rand() % (vertex_count - 1);

        int temp = p[i];
        p[i] = p[r];
        p[r] = temp;
    }
}

// Compute the size of a set.
int set_size(int n, int set[]) {
    int j, m, d;

    m = (n + 31) / 32;
    d = 0;

    for(j = 0; j < m; j++) {
       d += POP_COUNT(set[j]);
    }

    return d;
}

// Prints a set.
void print_set(int n, int set[]) {
   int i;

   for(i = 0; i < n; i++) {
       if (IS_ELEMENT(set, i)) {
           printf("%5d", i);
       }
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
//   vertex_count: the total number of vertices in the graph G.
//   max_deg: the maximum degree of any vertice in graph G.
//   G: the graph that a minimum dominating set is being found for.
//
// This algorithm is based on pseudocode from Wendy Myrvold's slides in CSC 425.
// The colors refer to vertex states outlined in the slides.
int find_dom_set(int level, int* n_dom, int num_choice[NMAX], int num_dom[NMAX], int* size,
    int dom[MMAX], int vertex_count, int max_deg, int G[NMAX][MMAX], int p[NMAX]) {
    
    #if DEBUG
        printf("\nLEVEL: %d\n", level);
        printf("# vertices dominated: %d\n", *n_dom);
        printf("# choices per vertex:\n");
        print_vec(vertex_count, num_choice);
        printf("# times dominated:\n");
        print_vec(vertex_count, num_dom);
    #endif

    // initialize all the data
    if(level == 0) {
        int i;
        for(i = 0; i < vertex_count; i++) {
            ADD_ELEMENT(G[i], i);
        }

        *n_dom = 0;
        
        memset(num_choice, 0, vertex_count * sizeof(int));
        max_deg = 0;

        for(i = 0; i < vertex_count; i++) {
            // will be + 1 because of the all 1 diagonal
            num_choice[i] = set_size(vertex_count, G[i]);
            if(num_choice[i] > max_deg) {
                max_deg = num_choice[i];
            }
        }

        memset(num_dom, 0, vertex_count * sizeof(int));
        *size = 0;
        memset(dom, 0, MMAX * sizeof(int));
    }

    int i;
    for(i = 0; i < vertex_count; i++) {
        if(!num_choice[i]) {
            return 0;
        }
    }

    if(level == vertex_count || *n_dom == vertex_count) {
        return 1;
    }

    int u = p[level];

    // make vertex level blue
    for(i = 0; i < vertex_count; i++) {
        if(IS_ELEMENT(G[u], i)) {
            num_choice[i]--;
        };
    }

    if(find_dom_set(level + 1, n_dom, num_choice, num_dom, size, dom, vertex_count, max_deg, G, p)) {
        return 1;
    }

    // undo blue colouring
    for(i = 0; i < vertex_count; i++) {
        if(IS_ELEMENT(G[u], i)) {
            num_choice[i]++;
        };
    }

    // make vertex u red
    ADD_ELEMENT(dom, u);
    *size += 1;
    for(i = 0; i < vertex_count; i++) {
        if(IS_ELEMENT(G[u], i)) {
            num_dom[i]++;
        };
    }
    *n_dom = 0;
    for(i = 0; i < vertex_count; i++) {
        if(num_dom[i]) {
            *n_dom += 1;
        }
    }

    if(find_dom_set(level + 1, n_dom, num_choice, num_dom, size, dom, vertex_count, max_deg, G, p)) {
        return 1;
    }

    // undo red colouring
    DEL_ELEMENT(dom, u);
    *size -= 1;
    for(i = 0; i < vertex_count; i++) {
        if(IS_ELEMENT(G[u], i)) {
            if(num_dom[i]) {
                *n_dom -= 1;
            }
            num_dom[i]--;
        };
    }
}

// Print a dominating set.
// Parameters:
//   size: the size of the dominating set.
//   vertex_count: the total number of vertices in the graph.
//   dom: the dominating set.
void print_dom_set(int size, int vertex_count, int* dom) {
    printf("\n%5d\n", size);
    print_set(vertex_count, dom);
    printf("\n");
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

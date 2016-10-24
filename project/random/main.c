// This code was taken from Brendan McKay's nauty
/*****************************************************************************
*                                                                            *
*   Conventions and Assumptions:                                             *
*                                                                            *
*       A 'longword' is the chunk of memory that is occupied by a long int   *
*       We assume that longwords have at least 32 bits.
*       The rightmost 32 bits of longwords are numbered 0..31, left to       *
*       right.                                                               *
*                                                                            *
*       The int variables m and n have consistent meanings througout.        *
*       Graphs have n vertices always, and sets have m longwords.            *
*                                                                            *
*       A 'set' consists of m contiguous longwords, whose bits are numbered  *
*       0,1,2,... from left (high-order) to right (low-order), using only    *
*       the rightmost 32 bits of each longword.  It is used to represent a   *
*       subset of {0,1,...,n-1} in the usual way - bit number x is 1 iff x   *
*       is in the subset.  Bits numbered n or greater, and unnumbered bits,  *
*       are assumed permanently zero.                                        *
*                                                                            *
*****************************************************************************/
#include <stdio.h> 

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

int read_graph(int*, int*, int[NMAX][MMAX]);
void print_graph(int, int[NMAX][MMAX]);
int set_size(int, int[]);
void print_set(int, int[]);
int find_dom_set(int, int, int, int, int[NMAX][MMAX], int[MMAX], int[MMAX]);

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

int main(int argc, char *argv[]) {
    int n,m;
    int G[NMAX][MMAX];
    int vertex_set[MMAX];
    int answer[MMAX];

    while (read_graph(&n, &m, G)) {
        print_graph(n, G);

        find_dom_set(0, 5, n, m, G, vertex_set, answer);

        int d = set_size(n, answer);
        printf("%5d\n", d);
        print_set(n, answer);
    }
}

// Code written by Wendy Myrvold.
// Reads a graph in adjacency list format and
// stores it in a compressed adjacency matrix.
int read_graph(int *n, int *m, int G[NMAX][MMAX]) {
    int i, j, u, d;

    if (scanf("%d", n)!= 1) return(0);
    *m= (*n+31)/32;

    // Initialize the graph to have no edges.
    for (i=0; i < *n; i++) {
        for (j=0; j < *m; j++) {
            G[i][j]= 0;
        }
    }

    // Read in the graph.
    for(i = 0; i < *n; i++) {
        if(scanf("%d", &d) != 1){
            return 0;
        }
        for(j = 0; j < d; j++) {
            if(scanf("%d", &u) != 1) {
                return 0;
            }

            ADD_ELEMENT(G[i], u);
            ADD_ELEMENT(G[u], i);
        }
    }

    return 1;
}

// Prints a graph stored in the compressed adjacency format.
void print_graph(int n, int G[NMAX][MMAX]) {
    int i, j, d, m;

    printf("%5d\n", n);
    for (i = 0; i < n; i++) {
        d = set_size(n, G[i]);
        printf("%5d", d);
        print_set(n, G[i]);
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

int find_dom_set(int level, int k, int n, int m, int G[NMAX][MMAX], int dominated[MMAX], int dom_set[MMAX]) {
    int new_dominated[MMAX];
    int i, j, u;


    if (level == 0) {
        // Set diagonal entries of adjacency matrix to 1 to simplify logic.
        for (i = 0; i < n; i++) {
            ADD_ELEMENT(G[i], i);
        }

        // Initialize dominated and dom_set to have no vertices.
        for (j = 0; j < m; j++) {
            dominated[j]=0;
            dom_set[j]=0;
        }
    }

    if (level == k) {
        if (set_size(n, dom_set) == n) { // ??
            printf("Dominating set: ");
            print_set(n, dom_set);
            return 1;
        } else {
            return 0;
        }
    }

#if DEBUG
    printf("Level %3d: The dominated vertices are: ", level);
    print_set(n, dominated);
#endif

    if (level >= 1) return 0; // students should write their own code.

    u = 0;
    ADD_ELEMENT(dom_set, u); // This would put vertex 0 into the dominating set:

    for (j = 0; j < m; j++) {
        new_dominated[j] = dominated[j] | G[u][j];
    }

    if (find_dom_set(level+1, k-1, n, m, G, new_dominated, dom_set)) {
        return 1;
    }

    DEL_ELEMENT(dom_set, u); // Take u out again.

    return 0;
}

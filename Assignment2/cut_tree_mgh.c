/*
** Default gcc compiler at Omega can compile this code
** The first line will have two values, n and m separated by space, giving the numbers of vertices and edges.
** The next m lines will each contain three integers. The first two, the vertices incident to an edge, will be in the range 0 . . . n - 1.
** The next m lines can be provided by copying from input file and pasting on the input console
** The output will be displayed after the second input lines are provided.
** Adjacency lists were used.
*/



// Reference: Generalized Cut Trees+ by Dan Gusfield and Dalit Naor. The MGH pseudo code was used in cut_tree_mgh function
// The below function algorithms were taken from http://ranger.uta.edu/~weems/NOTES5311/ffLab.c  [Ford-Fulkerson with adjacency lists]
// The Ford-Fulkerson Algorithm, enqueue, dequeue, Breadth-First, read_input, Search for an augmenting path these 

#include <stdio.h>
#include <stdlib.h>

// Basic Definitions
#define WHITE 0
#define GRAY 1
#define BLACK 2
#define oo 1000000000

void cut_tree_mgh();
// Declarations

int n;             // number of nodes
int residualEdges; // number of edges in residual network
struct edge {
  int tail,head,capacity,flow,inverse; 
};

typedef struct edge edgeType;

edgeType *edgeTab;

int *firstEdge; // Table indicating first in range of edges with a common tail

int *color;    // Needed for breadth-first search
int *pred;     // Array to store augmenting path
int *predEdge; // edgeTab subscript of edge used to reach vertex i in BFS



int min(int x, int y)
{
    return x < y ? x : y; // returns minimum of x and y
}

// Queue for breadth-first search - not circular.

int head, tail;
int *q;

void enqueue(int x)
{
    q[tail] = x;
    tail++;
    color[x] = GRAY;
}

int dequeue()
{
    int x = q[head];
    head++;
    color[x] = BLACK;
    return x;
}

// Breadth-First Search for an augmenting path

int bfs(int start, int target)
// Searches for path from start to target.
// Returns 1 if found, otherwise 0.
{
    int u, v, i;

    for (u = 0; u < n; u++)
        color[u] = WHITE;
    head = tail = 0; // Since q is not circular, it is reinitialized for each BFS
    enqueue(start);
    pred[start] = -1;
    while (head != tail)
    {
        u = dequeue();
        if (u == target)
            return 1;

        // Search all adjacent white nodes v. If the residual capacity
        // from u to v in the residual network is positive,
        // enqueue v.
        for (i = firstEdge[u]; i < firstEdge[u + 1]; i++)
        {
            v = edgeTab[i].head;
            if (color[v] == WHITE && edgeTab[i].capacity - edgeTab[i].flow > 0)
            {
                enqueue(v);
                pred[v] = u;
                predEdge[v] = i;
            }
        }
    }
    // No augmenting path remains, so a maximum flow and minimum cut
    // have been found.  Black vertices are in the
    // source side (S) of the minimum cut, while white vertices are in the
    // sink side (T).

    return 0;
}

// Ford-Fulkerson Algorithm

int max_flow(int source, int sink)
{
    int i, j, u;
    int max_flow;
    int APcount = 0;

    color = (int *)malloc(n * sizeof(int));
    pred = (int *)malloc(n * sizeof(int));
    predEdge = (int *)malloc(n * sizeof(int));
    q = (int *)malloc(n * sizeof(int));
    if (!color || !pred || !predEdge || !q)
    {
        printf("malloc failed %d\n", __LINE__);
        exit(0);
    }

    // Initialize empty flow.
    max_flow = 0;
    for (i = 0; i < residualEdges; i++)
        edgeTab[i].flow = 0;

    // While there exists an augmenting path,
    // increment the flow along this path.
    while (bfs(source, sink))
    {
        // Determine the amount by which we can increment the flow.
        int increment = oo;
        APcount++;
        for (u = sink; pred[u] != (-1); u = pred[u])
        {
            i = predEdge[u];
            increment = min(increment, edgeTab[i].capacity - edgeTab[i].flow);
        }
        // Now increment the flow.
        for (u = sink; pred[u] != (-1); u = pred[u])
        {
            i = edgeTab[predEdge[u]].inverse;
            edgeTab[predEdge[u]].flow += increment;
            edgeTab[i].flow -= increment; // Reverse in residual
        }
        max_flow += increment;
    }

    return max_flow;
}

// Reading the input file and organize adjacency lists for residual network.

int tailThenHead(const void *xin, const void *yin)
// Used in calls to qsort() and bsearch() for read_input_file()
{
    int result;
    edgeType *x, *y;

    x = (edgeType *)xin;
    y = (edgeType *)yin;
    result = x->tail - y->tail;
    if (result != 0)
        return result;
    else
        return x->head - y->head;
}

void read_input()
{
    int tail,head,capacity,i,j;
    int inputEdges;   // Number of edges in input file.
    int workingEdges; // Number of residual network edges initially
                      // generated from input file.
    edgeType work;
    edgeType *ptr;

    // read number of nodes and edges
    printf("The number of nodes and edges separated by space: ");
    scanf("%d %d", &n, &inputEdges);
    // Table is allocated at worst-case size, since space for inverses is needed.
    edgeTab = (edgeType *)malloc(2 * inputEdges * sizeof(edgeType));
    if (!edgeTab)
    {
        printf("edgeTab malloc failed %d\n", __LINE__);
        exit(0);
    }
    // read edges, each with a capacity
    printf("\nEnter The tail head and capacities separated by space. All at once\n");
    workingEdges = 0;
    for (i = 0; i < inputEdges; i++)
    {
        scanf("%d %d %d", &tail, &head, &capacity);
        // Test for illegal edge, including incoming to source and outgoing from
        // sink.
        if (tail < 0 || tail >= n || head < 0 || head >= n || capacity < 0)
        {
            printf("Invalid input %d %d %d at %d\n", tail, head, capacity, __LINE__);
            exit(0);
        }
        // Save input edge
        edgeTab[workingEdges].tail = tail;
        edgeTab[workingEdges].head = head;
        edgeTab[workingEdges].capacity = capacity;
        workingEdges++;
        // Save inverse of input edge
        edgeTab[workingEdges].tail = head;
        edgeTab[workingEdges].head = tail;
        edgeTab[workingEdges].capacity = capacity;
        workingEdges++;
    }

    qsort(edgeTab, workingEdges, sizeof(edgeType), tailThenHead);

    residualEdges=0;
    for (i = 1; i < workingEdges; i++)
        if (edgeTab[residualEdges].tail == edgeTab[i].tail && edgeTab[residualEdges].head == edgeTab[i].head)
            edgeTab[residualEdges].capacity += edgeTab[i].capacity; // || case
        else
        {
            residualEdges++;
            edgeTab[residualEdges].tail = edgeTab[i].tail;
            edgeTab[residualEdges].head = edgeTab[i].head;
            edgeTab[residualEdges].capacity = edgeTab[i].capacity;
        }
    residualEdges++;

    for (i = 0; i < residualEdges; i++)
    {
        work.tail = edgeTab[i].head;
        work.head = edgeTab[i].tail;
        ptr = (edgeType *)bsearch(&work, edgeTab, residualEdges, sizeof(edgeType),
                                  tailThenHead);
        if (ptr == NULL)
        {
            printf("bsearch %d failed %d\n", i, __LINE__);
            exit(0);
        }
        edgeTab[i].inverse = ptr - edgeTab; // ptr arithmetic to get subscript
    }

    firstEdge = (int *)malloc((n + 1) * sizeof(int));
    if (!firstEdge)
    {
        printf("malloc failed %d\n", __LINE__);
        exit(0);
    }
    j = 0;
    for (i = 0; i < n; i++)
    {
        firstEdge[i] = j;
        // Skip over edges with vertex i as their tail.
        for (;
             j < residualEdges && edgeTab[j].tail == i;
             j++)
            ;
    }
    firstEdge[n] = residualEdges; //Sentinel
}


// Calculate the cut tree
// The below program was written using the pseudo code for CUT TREE PROGRAM MGH.
void cut_tree_mgh(){

    int source, sink, minimum_cut, i, j, pos;
    int p[n], f1[n];
    int **tree = (int **)malloc(n * sizeof(int *));
    
	for (i = 0; i < n; i++){
        p[i] = 0;
        f1[i] = 0;
        tree[i] = (int *)malloc(n * sizeof(int));
        for (j = 0; j < n; j++){
            tree[i][j] = 0;
        }
    }

    for (source = 1; source < n; source++){
        sink = p[source];

        //Calculate the minimum cut
        minimum_cut = max_flow(source, sink);

        f1[source] = minimum_cut;
        //Mark the nodes in the supernode with its representative
        for (i = 0; i < n; i++){
            if (p[i] == sink && i != source && color[i] == BLACK){
                p[i] = source;
            }
        }

        //Change the label
        if (color[p[sink]] == BLACK){
			p[source] = p[sink];
            p[sink] = source;
            f1[source] = f1[sink];
            f1[sink] = minimum_cut;
        }

        //Store the final cut tree when source is the last node of the input graph.
        if (source == n - 1){
            for (i = 1; i <= source; i++){
                tree[i][p[i]] = f1[i];
            }
        }
    }

    printf("\nThe final Cut Tree\n");
    for (i = 0; i < n; i++){
		for (j = 0; j < n; j++){
        	if (tree[i][j] > 0){
        		printf("\n%d %d %d", i, j, tree[i][j]);
			}
		}
	}
	printf("\n");
}

int main()
{
    read_input();
    cut_tree_mgh();
}




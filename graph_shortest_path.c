/*
 * ============================================================
 *  Graph Shortest Path — Dijkstra's Algorithm
 *  Author  : Nasif Hasan
 *  Language: C99
 *  Project : Portfolio — Algorithms & Data Structures
 * ============================================================
 *
 *  Finds the shortest path between two nodes in a weighted,
 *  undirected graph using Dijkstra's greedy algorithm.
 *
 *  Complexity:
 *    Time  — O(V²)  with adjacency matrix + linear scan
 *    Space — O(V²)  for the adjacency matrix
 *
 *  Compile:
 *    gcc -std=c99 -Wall -o graph graph_shortest_path.c
 *
 *  Run:
 *    ./graph
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>

/* ── Constants ─────────────────────────────────────────────── */
#define MAX_VERTICES 100
#define INF          INT_MAX
#define MAX_LABEL    8

/* ── Data Structures ───────────────────────────────────────── */

typedef struct {
    int  n_vertices;
    int  n_edges;
    int  adj[MAX_VERTICES][MAX_VERTICES];  /* adjacency matrix; 0 = no edge */
    char labels[MAX_VERTICES][MAX_LABEL];  /* human-readable node names     */
} Graph;

/* ── Prototypes ────────────────────────────────────────────── */
void graph_init      (Graph *g);
void graph_add_edge  (Graph *g, int u, int v, int weight);
int  graph_node_index(Graph *g, const char *label);
int  graph_add_node  (Graph *g, const char *label);
void dijkstra        (Graph *g, int src, int dst);
void print_path      (int prev[], int dst, Graph *g);
void print_graph     (Graph *g);
void print_banner    (void);
void run_demo        (void);
void run_interactive (void);

/* ─────────────────────────────────────────────────────────── */
/*  Graph initialisation                                        */
/* ─────────────────────────────────────────────────────────── */
void graph_init(Graph *g) {
    g->n_vertices = 0;
    g->n_edges    = 0;
    for (int i = 0; i < MAX_VERTICES; i++)
        for (int j = 0; j < MAX_VERTICES; j++)
            g->adj[i][j] = 0;
}

/* ─────────────────────────────────────────────────────────── */
/*  Add a node; returns its index                              */
/* ─────────────────────────────────────────────────────────── */
int graph_add_node(Graph *g, const char *label) {
    if (g->n_vertices >= MAX_VERTICES) {
        fprintf(stderr, "Error: vertex limit (%d) reached.\n", MAX_VERTICES);
        return -1;
    }
    int idx = g->n_vertices++;
    strncpy(g->labels[idx], label, MAX_LABEL - 1);
    g->labels[idx][MAX_LABEL - 1] = '\0';
    return idx;
}

/* Look up node index by label; returns -1 if not found */
int graph_node_index(Graph *g, const char *label) {
    for (int i = 0; i < g->n_vertices; i++)
        if (strcmp(g->labels[i], label) == 0)
            return i;
    return -1;
}

/* ─────────────────────────────────────────────────────────── */
/*  Add an undirected weighted edge                            */
/* ─────────────────────────────────────────────────────────── */
void graph_add_edge(Graph *g, int u, int v, int weight) {
    if (u < 0 || v < 0 || u >= g->n_vertices || v >= g->n_vertices) {
        fprintf(stderr, "Error: invalid vertex index.\n");
        return;
    }
    if (weight <= 0) {
        fprintf(stderr, "Warning: edge weight should be positive.\n");
    }
    g->adj[u][v] = weight;
    g->adj[v][u] = weight;   /* undirected */
    g->n_edges++;
}

/* ─────────────────────────────────────────────────────────── */
/*  Print graph structure                                       */
/* ─────────────────────────────────────────────────────────── */
void print_graph(Graph *g) {
    printf("\nGraph: %d vertices, %d edges\n", g->n_vertices, g->n_edges);
    printf("--------------------------------------------------\n");
    printf("  Edges (undirected):\n");
    for (int i = 0; i < g->n_vertices; i++) {
        for (int j = i + 1; j < g->n_vertices; j++) {
            if (g->adj[i][j] > 0) {
                printf("    [%s] <--(%d)--> [%s]\n",
                       g->labels[i], g->adj[i][j], g->labels[j]);
            }
        }
    }
    printf("--------------------------------------------------\n");
}

/* ─────────────────────────────────────────────────────────── */
/*  Reconstruct and print path via back-tracking prev[]        */
/* ─────────────────────────────────────────────────────────── */
void print_path(int prev[], int dst, Graph *g) {
    if (prev[dst] == -1) {
        printf("[%s]", g->labels[dst]);
        return;
    }
    print_path(prev, prev[dst], g);
    printf(" -> [%s]", g->labels[dst]);
}

/* ─────────────────────────────────────────────────────────── */
/*  Dijkstra's Shortest Path Algorithm                         */
/*                                                             */
/*  src — source vertex index                                  */
/*  dst — destination vertex index                             */
/* ─────────────────────────────────────────────────────────── */
void dijkstra(Graph *g, int src, int dst) {
    int  dist[MAX_VERTICES];    /* shortest distance from src        */
    int  prev[MAX_VERTICES];    /* previous node in optimal path     */
    bool visited[MAX_VERTICES]; /* has this node been finalised?     */

    /* ── Step 1: Initialise ────────────────────────────────── */
    for (int i = 0; i < g->n_vertices; i++) {
        dist[i]    = INF;
        prev[i]    = -1;
        visited[i] = false;
    }
    dist[src] = 0;

    printf("\n  [INIT] dist[%s] = 0, all others = INF\n\n", g->labels[src]);

    /* ── Step 2: Main loop — relax V-1 times ──────────────── */
    for (int iter = 0; iter < g->n_vertices - 1; iter++) {

        /* Find unvisited vertex with minimum distance (greedy pick) */
        int u   = -1;
        int min = INF;
        for (int i = 0; i < g->n_vertices; i++) {
            if (!visited[i] && dist[i] < min) {
                min = dist[i];
                u   = i;
            }
        }

        if (u == -1) break;          /* remaining nodes are unreachable */
        visited[u] = true;

        printf("  [VISIT] node [%s]  dist=%d\n", g->labels[u], dist[u]);

        if (u == dst) break;         /* early exit — destination reached */

        /* Relax all edges from u */
        for (int v = 0; v < g->n_vertices; v++) {
            if (visited[v])        continue; /* already finalised */
            if (g->adj[u][v] == 0) continue; /* no edge */
            if (dist[u] == INF)    continue; /* u unreachable    */

            int alt = dist[u] + g->adj[u][v];

            if (alt < dist[v]) {
                printf("    [RELAX] %s->%s : %d+%d=%d < %s  update!\n",
                       g->labels[u], g->labels[v],
                       dist[u], g->adj[u][v], alt,
                       dist[v] == INF ? "INF" : "current");
                dist[v] = alt;
                prev[v] = u;
            }
        }
    }

    /* ── Step 3: Report result ─────────────────────────────── */
    printf("\n--------------------------------------------------\n");
    if (dist[dst] == INF) {
        printf("  Result : No path from [%s] to [%s].\n",
               g->labels[src], g->labels[dst]);
    } else {
        printf("  Source      : [%s]\n", g->labels[src]);
        printf("  Destination : [%s]\n", g->labels[dst]);
        printf("  Shortest distance : %d\n", dist[dst]);
        printf("  Path : ");
        print_path(prev, dst, g);
        printf("\n");
    }
    printf("--------------------------------------------------\n");
}

/* ─────────────────────────────────────────────────────────── */
/*  Demo — pre-built example graph (8 nodes, 12 edges)         */
/* ─────────────────────────────────────────────────────────── */
void run_demo(void) {
    Graph g;
    graph_init(&g);

    /* Add nodes A through H */
    const char *node_names[] = {"A","B","C","D","E","F","G","H"};
    int n = sizeof(node_names) / sizeof(node_names[0]);
    for (int i = 0; i < n; i++)
        graph_add_node(&g, node_names[i]);

    /* Add edges */
    graph_add_edge(&g, 0, 1,  4);   /* A-B */
    graph_add_edge(&g, 0, 3,  2);   /* A-D */
    graph_add_edge(&g, 1, 2,  5);   /* B-C */
    graph_add_edge(&g, 1, 4, 10);   /* B-E */
    graph_add_edge(&g, 2, 5,  3);   /* C-F */
    graph_add_edge(&g, 3, 4,  7);   /* D-E */
    graph_add_edge(&g, 3, 6,  6);   /* D-G */
    graph_add_edge(&g, 4, 5,  8);   /* E-F */
    graph_add_edge(&g, 4, 6,  4);   /* E-G */
    graph_add_edge(&g, 4, 7,  9);   /* E-H */
    graph_add_edge(&g, 5, 7,  2);   /* F-H */
    graph_add_edge(&g, 6, 7,  5);   /* G-H */

    print_graph(&g);

    /* Find shortest path: A -> H */
    printf("\nFinding shortest path: [A] -> [H]\n");
    dijkstra(&g, 0, 7);
}

/* ─────────────────────────────────────────────────────────── */
/*  Interactive mode — user builds own graph                   */
/* ─────────────────────────────────────────────────────────── */
void run_interactive(void) {
    Graph g;
    graph_init(&g);

    int v_count, e_count;

    printf("\n  Enter number of vertices: ");
    if (scanf("%d", &v_count) != 1 || v_count <= 0 || v_count > MAX_VERTICES) {
        printf("  Invalid input.\n");
        return;
    }

    printf("  Enter node labels (e.g. A B C ...):\n  > ");
    for (int i = 0; i < v_count; i++) {
        char label[MAX_LABEL];
        scanf("%7s", label);
        graph_add_node(&g, label);
    }

    printf("\n  Enter number of edges: ");
    if (scanf("%d", &e_count) != 1 || e_count < 0) {
        printf("  Invalid input.\n");
        return;
    }

    printf("  Enter edges as:  <nodeA> <nodeB> <weight>\n");
    for (int i = 0; i < e_count; i++) {
        char la[MAX_LABEL], lb[MAX_LABEL];
        int  w;
        printf("  Edge %d: ", i + 1);
        scanf("%7s %7s %d", la, lb, &w);

        int u = graph_node_index(&g, la);
        int v = graph_node_index(&g, lb);
        if (u == -1 || v == -1) {
            printf("  Unknown node label — skipping.\n");
            i--;
            continue;
        }
        graph_add_edge(&g, u, v, w);
    }

    print_graph(&g);

    char src_label[MAX_LABEL], dst_label[MAX_LABEL];
    printf("\n  Enter source node      : ");
    scanf("%7s", src_label);
    printf("  Enter destination node : ");
    scanf("%7s", dst_label);

    int src = graph_node_index(&g, src_label);
    int dst = graph_node_index(&g, dst_label);

    if (src == -1 || dst == -1) {
        printf("  Error: node not found.\n");
        return;
    }

    printf("\n  Finding shortest path: [%s] -> [%s]\n", src_label, dst_label);
    dijkstra(&g, src, dst);
}

/* ─────────────────────────────────────────────────────────── */
/*  Banner                                                      */
/* ─────────────────────────────────────────────────────────── */
void print_banner(void) {
    printf("==================================================\n");
    printf("   Graph Shortest Path — Dijkstra's Algorithm\n");
    printf("   Author: Nasif Hasan\n");
    printf("==================================================\n");
}

/* ─────────────────────────────────────────────────────────── */
/*  Entry Point                                                 */
/* ─────────────────────────────────────────────────────────── */
int main(void) {
    print_banner();

    printf("\n  Choose mode:\n");
    printf("    1. Run demo graph (A to H, 8 nodes, 12 edges)\n");
    printf("    2. Enter your own graph\n");
    printf("\n  > ");

    int choice;
    if (scanf("%d", &choice) != 1) choice = 1;

    switch (choice) {
        case 1:  run_demo();        break;
        case 2:  run_interactive(); break;
        default: printf("  Invalid choice. Running demo.\n");
                 run_demo();
    }

    printf("\n==================================================\n");
    printf("   Portfolio project by Nasif Hasan\n");
    printf("==================================================\n\n");

    return 0;
}

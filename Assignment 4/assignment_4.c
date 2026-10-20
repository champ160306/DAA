#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAXN 25000
#define MAXE 100000
#define INF 1000000000

int head[MAXN], nxt[2 * MAXE], to[2 * MAXE], eid[2 * MAXE];
int wt[MAXE];
int ecount, n;
int dist[MAXN], prevn[MAXN];
unsigned long long seed = 12345;

int rnd(int m) {
    seed = seed * 6364136223846793005ULL + 1442695040888963407ULL;
    return (int)((seed >> 33) % m);
}

void init(int nodes) {
    n = nodes;
    ecount = 0;
    for (int i = 0; i < n; i++) head[i] = -1;
}

int add_edge(int u, int v, int w) {
    int id = ecount++;
    wt[id] = w;
    to[2 * id] = v; eid[2 * id] = id; nxt[2 * id] = head[u]; head[u] = 2 * id;
    to[2 * id + 1] = u; eid[2 * id + 1] = id; nxt[2 * id + 1] = head[v]; head[v] = 2 * id + 1;
    return id;
}

void update_weight(int id, int w) {
    wt[id] = w;
}

int hk[4 * MAXE], hv[4 * MAXE], hs;

void hpush(int k, int v) {
    int i = hs++;
    while (i > 0) {
        int p = (i - 1) / 2;
        if (hk[p] <= k) break;
        hk[i] = hk[p]; hv[i] = hv[p];
        i = p;
    }
    hk[i] = k; hv[i] = v;
}

void hpop(int *k, int *v) {
    *k = hk[0]; *v = hv[0];
    hs--;
    int lk = hk[hs], lv = hv[hs];
    int i = 0;
    while (1) {
        int c = 2 * i + 1;
        if (c >= hs) break;
        if (c + 1 < hs && hk[c + 1] < hk[c]) c++;
        if (hk[c] >= lk) break;
        hk[i] = hk[c]; hv[i] = hv[c];
        i = c;
    }
    hk[i] = lk; hv[i] = lv;
}

void dijkstra(int src) {
    for (int i = 0; i < n; i++) { dist[i] = INF; prevn[i] = -1; }
    dist[src] = 0;
    hs = 0;
    hpush(0, src);
    while (hs > 0) {
        int d, u;
        hpop(&d, &u);
        if (d > dist[u]) continue;
        for (int e = head[u]; e != -1; e = nxt[e]) {
            int v = to[e];
            int nd = d + wt[eid[e]];
            if (nd < dist[v]) {
                dist[v] = nd;
                prevn[v] = u;
                hpush(nd, v);
            }
        }
    }
}

void print_path(int dest, const char **names) {
    int stack[MAXN], top = 0;
    for (int v = dest; v != -1; v = prevn[v]) stack[top++] = v;
    for (int i = top - 1; i >= 0; i--) {
        printf("%s", names[stack[i]]);
        if (i > 0) printf(" -> ");
    }
    printf("\n");
}

void report(int src, int *hosp, int hc, const char **names, const char **hnames) {
    dijkstra(src);
    int best = -1;
    for (int i = 0; i < hc; i++) {
        printf("%s: ", hnames[i]);
        if (dist[hosp[i]] >= INF) { printf("unreachable\n"); continue; }
        printf("%d min | ", dist[hosp[i]]);
        print_path(hosp[i], names);
        if (best == -1 || dist[hosp[i]] < dist[hosp[best]]) best = i;
    }
    if (best != -1) printf("Nearest hospital: %s (%d min)\n", hnames[best], dist[hosp[best]]);
}

int gw, gh;

void build_grid(int w, int h) {
    gw = w; gh = h;
    init(w * h);
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++) {
            if (x + 1 < w) add_edge(y * w + x, y * w + x + 1, 1 + rnd(9));
            if (y + 1 < h) add_edge(y * w + x, (y + 1) * w + x, 1 + rnd(9));
        }
}

void draw_grid(int src, int dest) {
    static char mark[MAXN];
    memset(mark, 0, gw * gh);
    for (int v = dest; v != -1; v = prevn[v]) mark[v] = 1;
    for (int y = 0; y < gh; y++) {
        for (int x = 0; x < gw; x++) {
            int v = y * gw + x;
            char c = mark[v] ? '*' : '.';
            if (v == src) c = 'S';
            if (v == dest) c = 'D';
            printf("%c ", c);
        }
        printf("\n");
    }
}

int main() {
    const char *names[] = {"S", "J1", "J2", "J3", "J4", "J5", "J6", "J7", "J8", "J9", "J10", "J11"};
    const char *hnames[] = {"H1(J5)", "H2(J9)", "H3(J11)"};
    int hosp[] = {5, 9, 11};
    init(12);
    int ed[][3] = {
        {0, 1, 4}, {0, 2, 3}, {1, 3, 5}, {2, 3, 2}, {2, 4, 6}, {3, 5, 7}, {4, 6, 3},
        {3, 6, 4}, {5, 7, 2}, {6, 8, 5}, {7, 9, 3}, {8, 9, 2}, {6, 10, 6}, {8, 11, 4}, {10, 11, 2}, {1, 4, 8}
    };
    int ne = sizeof(ed) / sizeof(ed[0]);
    int ids[32];
    for (int i = 0; i < ne; i++) ids[i] = add_edge(ed[i][0], ed[i][1], ed[i][2]);

    printf("===== CITY NETWORK: INITIAL TRAFFIC =====\n");
    report(0, hosp, 3, names, hnames);

    printf("\n===== TRAFFIC UPDATE: J3-J6 congested (4 -> 25), J2-J3 (2 -> 12) =====\n");
    update_weight(ids[7], 25);
    update_weight(ids[3], 12);
    report(0, hosp, 3, names, hnames);

    printf("\n===== TRAFFIC UPDATE: J2-J3 cleared (12 -> 2), J6-J8 accident (5 -> 40) =====\n");
    update_weight(ids[3], 2);
    update_weight(ids[9], 40);
    report(0, hosp, 3, names, hnames);

    printf("\n===== GRID CITY 10 x 10: ROUTE VISUALIZATION =====\n");
    build_grid(10, 10);
    int s = 0, d = 99;
    dijkstra(s);
    printf("Shortest time S to D: %d min\n", dist[d]);
    draw_grid(s, d);
    for (int v = d; v != -1; v = prevn[v]) {
        if (prevn[v] != -1) {
            for (int e = head[v]; e != -1; e = nxt[e]) {
                if (to[e] == prevn[v] && eid[e] >= 0) { update_weight(eid[e], 60); break; }
            }
            if (v == 44 || v == 45) break;
        }
    }
    dijkstra(s);
    printf("\nAfter heavy congestion on part of the route, new time: %d min\n", dist[d]);
    draw_grid(s, d);

    printf("\n===== LARGE CITY: 150 x 150 GRID (22500 INTERSECTIONS) =====\n");
    build_grid(150, 150);
    printf("Intersections: %d, Roads: %d\n", n, ecount);
    clock_t t0 = clock();
    dijkstra(0);
    clock_t t1 = clock();
    printf("Initial shortest time to farthest corner: %d min\n", dist[n - 1]);
    printf("Initial computation time: %.3f ms\n", 1000.0 * (t1 - t0) / CLOCKS_PER_SEC);
    t0 = clock();
    long long total = 0;
    for (int i = 0; i < 500; i++) {
        update_weight(rnd(ecount), 1 + rnd(30));
        dijkstra(0);
        total += dist[n - 1];
    }
    t1 = clock();
    printf("500 real-time weight updates with recomputation: %.3f ms total\n", 1000.0 * (t1 - t0) / CLOCKS_PER_SEC);
    printf("Final shortest time to farthest corner: %d min\n", dist[n - 1]);
    return 0;
}

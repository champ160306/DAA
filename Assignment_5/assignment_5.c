#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAXS 300
#define MAXK 20
#define INF 1000000000

int stages, cnt[MAXS];
int tm[MAXS][MAXK][MAXK], wx[MAXS][MAXK][MAXK], fu[MAXS][MAXK][MAXK];
int closed[MAXS][MAXK][MAXK];
int dp[MAXS][MAXK], par[MAXS][MAXK];
unsigned long long seed = 2024;

int rnd(int m) {
    seed = seed * 6364136223846793005ULL + 1442695040888963407ULL;
    return (int)((seed >> 33) % m);
}

int edge_cost(int s, int a, int b) {
    if (closed[s][a][b]) return INF;
    return tm[s][a][b] + wx[s][a][b] + fu[s][a][b];
}

void build(int st, int minc, int maxc) {
    stages = st;
    for (int s = 0; s < st; s++) cnt[s] = (s == 0 || s == st - 1) ? 3 : minc + rnd(maxc - minc + 1);
    for (int s = 0; s + 1 < st; s++)
        for (int a = 0; a < cnt[s]; a++)
            for (int b = 0; b < cnt[s + 1]; b++) {
                closed[s][a][b] = 0;
                if (rnd(100) < 70) {
                    tm[s][a][b] = 5 + rnd(30);
                    wx[s][a][b] = rnd(8);
                    fu[s][a][b] = 2 + rnd(10);
                } else {
                    tm[s][a][b] = 0; wx[s][a][b] = 0; fu[s][a][b] = 0;
                    closed[s][a][b] = 1;
                }
            }
    for (int s = 0; s + 1 < st; s++)
        for (int a = 0; a < cnt[s]; a++) {
            int ok = 0;
            for (int b = 0; b < cnt[s + 1]; b++) if (!closed[s][a][b]) ok = 1;
            if (!ok) { int b = rnd(cnt[s + 1]); closed[s][a][b] = 0; tm[s][a][b] = 10; wx[s][a][b] = 1; fu[s][a][b] = 4; }
        }
}

int solve(int src, int dst) {
    for (int s = 0; s < stages; s++)
        for (int a = 0; a < cnt[s]; a++) { dp[s][a] = INF; par[s][a] = -1; }
    dp[0][src] = 0;
    for (int s = 0; s + 1 < stages; s++)
        for (int a = 0; a < cnt[s]; a++) {
            if (dp[s][a] >= INF) continue;
            for (int b = 0; b < cnt[s + 1]; b++) {
                int c = edge_cost(s, a, b);
                if (c >= INF) continue;
                if (dp[s][a] + c < dp[s + 1][b]) {
                    dp[s + 1][b] = dp[s][a] + c;
                    par[s + 1][b] = a;
                }
            }
        }
    return dp[stages - 1][dst];
}

void print_route(int dst) {
    int path[MAXS], b = dst;
    for (int s = stages - 1; s >= 0; s--) { path[s] = b; b = par[s][b]; }
    for (int s = 0; s < stages; s++) {
        printf("S%d.N%d", s, path[s]);
        if (s + 1 < stages) printf(" -> ");
    }
    printf("\n");
}

void batch(int reqs[][2], int nr) {
    for (int i = 0; i < nr; i++) {
        int c = solve(reqs[i][0], reqs[i][1]);
        printf("Request %d: source N%d (stage 0) to destination N%d (stage %d): ", i + 1, reqs[i][0], reqs[i][1], stages - 1);
        if (c >= INF) { printf("no route available\n"); continue; }
        printf("cost %d\n  Route: ", c);
        print_route(reqs[i][1]);
    }
}

int main() {
    build(6, 3, 4);
    printf("===== MULTISTAGE GRAPH: %d STAGES =====\n", stages);
    for (int s = 0; s < stages; s++) printf("Stage %d: %d nodes\n", s, cnt[s]);
    int reqs[4][2] = {{0, 0}, {1, 2}, {2, 1}, {0, 2}};

    printf("\n===== BATCH PROCESSING: NORMAL CONDITIONS =====\n");
    batch(reqs, 4);

    printf("\n===== REAL-TIME UPDATE: WEATHER DELAY ON ROUTE OF REQUEST 1 =====\n");
    solve(reqs[0][0], reqs[0][1]);
    {
        int path[MAXS], b = reqs[0][1];
        for (int s = stages - 1; s >= 0; s--) { path[s] = b; b = par[s][b]; }
        wx[1][path[1]][path[2]] += 40;
        printf("Weather delay +40 added on stage 1 (N%d) to stage 2 (N%d)\n", path[1], path[2]);
    }
    batch(reqs, 4);

    printf("\n===== REAL-TIME UPDATE: ROAD CLOSURE ON ROUTE OF REQUEST 2 =====\n");
    solve(reqs[1][0], reqs[1][1]);
    {
        int path[MAXS], b = reqs[1][1];
        for (int s = stages - 1; s >= 0; s--) { path[s] = b; b = par[s][b]; }
        closed[2][path[2]][path[3]] = 1;
        printf("Road closed between stage 2 (N%d) and stage 3 (N%d)\n", path[2], path[3]);
    }
    batch(reqs, 4);

    printf("\n===== LARGE DATASET =====\n");
    seed = 777;
    build(250, 16, 20);
    int total_nodes = 0, total_edges = 0;
    for (int s = 0; s < stages; s++) total_nodes += cnt[s];
    for (int s = 0; s + 1 < stages; s++) total_edges += cnt[s] * cnt[s + 1];
    printf("Stages: %d, Nodes: %d, Possible routes between stages: %d\n", stages, total_nodes, total_edges);
    clock_t t0 = clock();
    long long sum = 0;
    int ok = 0;
    for (int r = 0; r < 1000; r++) {
        int c = solve(r % 3, (r / 3) % 3);
        if (c < INF) { sum += c; ok++; }
    }
    clock_t t1 = clock();
    printf("Batch of 1000 delivery requests processed, routes found: %d\n", ok);
    printf("Average optimal cost: %.2f\n", ok ? (double)sum / ok : 0.0);
    printf("Total processing time: %.3f ms\n", 1000.0 * (t1 - t0) / CLOCKS_PER_SEC);
    return 0;
}

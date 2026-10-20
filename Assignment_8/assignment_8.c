#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAXN 20
#define INF 1000000000

typedef struct Node {
    int m[MAXN][MAXN];
    int lb, level, city, time;
    int path[MAXN + 1];
} Node;

int N, dist[MAXN][MAXN], fuel[MAXN][MAXN], cost[MAXN][MAXN], tmat[MAXN][MAXN];
int best, bestpath[MAXN + 1], besttime, timelimit;
long expanded, generated;
unsigned long long seed = 31337;
Node **heap;
int hs;

int rnd(int m) {
    seed = seed * 6364136223846793005ULL + 1442695040888963407ULL;
    return (int)((seed >> 33) % m);
}

void hpush(Node *x) {
    int i = hs++;
    while (i > 0) {
        int p = (i - 1) / 2;
        if (heap[p]->lb <= x->lb) break;
        heap[i] = heap[p];
        i = p;
    }
    heap[i] = x;
}

Node *hpop() {
    Node *top = heap[0];
    hs--;
    Node *l = heap[hs];
    int i = 0;
    while (1) {
        int c = 2 * i + 1;
        if (c >= hs) break;
        if (c + 1 < hs && heap[c + 1]->lb < heap[c]->lb) c++;
        if (heap[c]->lb >= l->lb) break;
        heap[i] = heap[c];
        i = c;
    }
    if (hs > 0) heap[i] = l;
    return top;
}

int reduce(int m[MAXN][MAXN]) {
    int total = 0;
    for (int i = 0; i < N; i++) {
        int mn = INF;
        for (int j = 0; j < N; j++) if (m[i][j] < mn) mn = m[i][j];
        if (mn != INF && mn > 0) {
            total += mn;
            for (int j = 0; j < N; j++) if (m[i][j] != INF) m[i][j] -= mn;
        }
    }
    for (int j = 0; j < N; j++) {
        int mn = INF;
        for (int i = 0; i < N; i++) if (m[i][j] < mn) mn = m[i][j];
        if (mn != INF && mn > 0) {
            total += mn;
            for (int i = 0; i < N; i++) if (m[i][j] != INF) m[i][j] -= mn;
        }
    }
    return total;
}

void build(int n, int limit) {
    N = n;
    timelimit = limit;
    for (int i = 0; i < N; i++) {
        dist[i][i] = INF; fuel[i][i] = INF; cost[i][i] = INF; tmat[i][i] = INF;
    }
    for (int i = 0; i < N; i++)
        for (int j = i + 1; j < N; j++) {
            int d = 10 + rnd(90);
            int road = 10 + rnd(11);
            dist[i][j] = dist[j][i] = d;
            fuel[i][j] = fuel[j][i] = road;
            cost[i][j] = cost[j][i] = d + d * road / 20;
            tmat[i][j] = tmat[j][i] = d * 6 / 10 + rnd(10);
        }
}

void lc_branch_and_bound() {
    best = INF; besttime = 0; expanded = 0; generated = 0;
    heap = malloc(sizeof(Node *) * 4000000);
    hs = 0;
    Node *root = malloc(sizeof(Node));
    memcpy(root->m, cost, sizeof(cost));
    root->lb = reduce(root->m);
    root->level = 0; root->city = 0; root->time = 0;
    root->path[0] = 0;
    generated++;
    hpush(root);
    while (hs > 0) {
        Node *cur = hpop();
        if (cur->lb >= best) { free(cur); continue; }
        expanded++;
        if (cur->level == N - 1) {
            int total = cur->lb;
            int tt = cur->time + tmat[cur->city][0];
            if (tt <= timelimit && total < best) {
                best = total; besttime = tt;
                for (int i = 0; i < N; i++) bestpath[i] = cur->path[i];
                bestpath[N] = 0;
            }
            free(cur);
            continue;
        }
        for (int j = 0; j < N; j++) {
            if (cur->m[cur->city][j] == INF) continue;
            int nt = cur->time + tmat[cur->city][j];
            if (nt > timelimit) continue;
            if (hs >= 3999990) continue;
            Node *ch = malloc(sizeof(Node));
            memcpy(ch->m, cur->m, sizeof(cur->m));
            int i = cur->city;
            for (int k = 0; k < N; k++) { ch->m[i][k] = INF; ch->m[k][j] = INF; }
            ch->m[j][0] = INF;
            int r = reduce(ch->m);
            ch->lb = cur->lb + cur->m[i][j] + r;
            ch->level = cur->level + 1;
            ch->city = j;
            ch->time = nt;
            memcpy(ch->path, cur->path, sizeof(int) * (cur->level + 1));
            ch->path[ch->level] = j;
            generated++;
            if (ch->lb >= best) { free(ch); continue; }
            hpush(ch);
        }
        free(cur);
    }
    while (hs > 0) free(hpop());
    free(heap);
}

int bf_best, bf_path[MAXN + 1], bf_cur[MAXN + 1], bf_used[MAXN];
long bf_count;

void bf(int level, int c, int t) {
    if (t > timelimit) return;
    if (level == N) {
        bf_count++;
        int total = c + cost[bf_cur[N - 1]][0];
        int tt = t + tmat[bf_cur[N - 1]][0];
        if (tt <= timelimit && total < bf_best) {
            bf_best = total;
            memcpy(bf_path, bf_cur, sizeof(int) * N);
        }
        return;
    }
    for (int j = 1; j < N; j++) {
        if (bf_used[j]) continue;
        bf_used[j] = 1; bf_cur[level] = j;
        bf(level + 1, c + cost[bf_cur[level - 1]][j], t + tmat[bf_cur[level - 1]][j]);
        bf_used[j] = 0;
    }
}

void brute_force() {
    memset(bf_used, 0, sizeof(bf_used));
    bf_best = INF; bf_count = 0;
    bf_cur[0] = 0;
    bf(1, 0, 0);
}

void print_tour(int *p) {
    for (int i = 0; i <= N; i++) {
        printf("C%d", p[i % N] + 1);
        if (i < N) printf(" -> ");
    }
    printf("\n");
}

double ms(clock_t a, clock_t b) { return 1000.0 * (b - a) / CLOCKS_PER_SEC; }

int main() {
    build(8, 400);
    printf("===== COST MATRIX (DISTANCE + FUEL COST) FOR %d CITIES, TIME LIMIT %d =====\n", N, timelimit);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (i == j) printf("  -- ");
            else printf("%4d ", cost[i][j]);
        }
        printf("\n");
    }

    printf("\n===== VERIFICATION: BRUTE FORCE VS LC BRANCH AND BOUND =====\n");
    clock_t t0 = clock();
    brute_force();
    clock_t t1 = clock();
    printf("Brute Force: cost %d, tours checked %ld, time %.3f ms\n", bf_best, bf_count, ms(t0, t1));
    print_tour(bf_path);
    t0 = clock();
    lc_branch_and_bound();
    t1 = clock();
    printf("LC Branch and Bound: cost %d, time taken %d, nodes generated %ld, nodes expanded %ld, time %.3f ms\n", best, besttime, generated, expanded, ms(t0, t1));
    print_tour(bestpath);

    int sizes[] = {10, 12, 14, 16};
    for (int k = 0; k < 4; k++) {
        seed = 1000 + sizes[k];
        build(sizes[k], 100 * sizes[k]);
        printf("\n===== %d CITIES =====\n", N);
        t0 = clock();
        lc_branch_and_bound();
        t1 = clock();
        printf("LC Branch and Bound: cost %d, time taken %d (limit %d)\n", best, besttime, timelimit);
        printf("Nodes generated: %ld, nodes expanded: %ld, time %.3f ms\n", generated, expanded, ms(t0, t1));
        printf("Brute force would need %s permutations\n", sizes[k] == 10 ? "362880" : sizes[k] == 12 ? "39916800" : sizes[k] == 14 ? "6227020800" : "1307674368000");
        print_tour(bestpath);
    }

    seed = 2025;
    build(20, 2000);
    printf("\n===== 20 CITIES =====\n");
    t0 = clock();
    lc_branch_and_bound();
    t1 = clock();
    printf("LC Branch and Bound: cost %d, time taken %d (limit %d)\n", best, besttime, timelimit);
    printf("Nodes generated: %ld, nodes expanded: %ld, time %.3f ms\n", generated, expanded, ms(t0, t1));
    printf("Brute force would need 121645100408832000 permutations\n");
    print_tour(bestpath);
    return 0;
}

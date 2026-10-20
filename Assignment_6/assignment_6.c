#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAXN 2000
#define MAXW 20001

int n;
int wgt[MAXN], val[MAXN], per[MAXN], used[MAXN];
const char *nm[MAXN];
unsigned long long seed = 99;

int rnd(int m) {
    seed = seed * 6364136223846793005ULL + 1442695040888963407ULL;
    return (int)((seed >> 33) % m);
}

int effective(int i, int boost) {
    return per[i] ? val[i] * boost : val[i];
}

int brute(int W, int boost, int *bestmask) {
    int best = -1;
    for (long m = 0; m < (1L << n); m++) {
        int w = 0, v = 0;
        for (int i = 0; i < n; i++)
            if (m >> i & 1) {
                if (used[i]) { w = W + 1; break; }
                w += wgt[i]; v += effective(i, boost);
                if (w > W) break;
            }
        if (w <= W && v > best) { best = v; *bestmask = (int)m; }
    }
    return best;
}

static int table[MAXN + 1][MAXW];

int dp(int W, int boost, int *sel) {
    for (int w = 0; w <= W; w++) table[0][w] = 0;
    for (int i = 1; i <= n; i++) {
        int it = i - 1;
        for (int w = 0; w <= W; w++) {
            table[i][w] = table[i - 1][w];
            if (!used[it] && wgt[it] <= w) {
                int c = table[i - 1][w - wgt[it]] + effective(it, boost);
                if (c > table[i][w]) table[i][w] = c;
            }
        }
    }
    int w = W;
    for (int i = n; i >= 1; i--) {
        sel[i - 1] = 0;
        if (table[i][w] != table[i - 1][w]) { sel[i - 1] = 1; w -= wgt[i - 1]; }
    }
    return table[n][W];
}

int greedy(int W, int boost, int *sel) {
    int idx[MAXN];
    for (int i = 0; i < n; i++) { idx[i] = i; sel[i] = 0; }
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++) {
            double ri = (double)effective(idx[i], boost) / wgt[idx[i]];
            double rj = (double)effective(idx[j], boost) / wgt[idx[j]];
            if (rj > ri) { int t = idx[i]; idx[i] = idx[j]; idx[j] = t; }
        }
    int w = 0, v = 0;
    for (int k = 0; k < n; k++) {
        int i = idx[k];
        if (used[i]) continue;
        if (w + wgt[i] <= W) { w += wgt[i]; v += effective(i, boost); sel[i] = 1; }
    }
    return v;
}

void show(int *sel, int boost) {
    int tw = 0, tv = 0, rv = 0;
    for (int i = 0; i < n; i++)
        if (sel[i]) {
            printf("  %-18s weight %3d utility %3d %s\n", nm[i], wgt[i], val[i], per[i] ? "[perishable]" : "");
            tw += wgt[i]; tv += effective(i, boost); rv += val[i];
        }
    printf("  Total weight: %d, Total weighted utility: %d, Raw utility: %d\n", tw, tv, rv);
}

int main() {
    const char *names[] = {"Medicine Kit", "Rice Bags", "Drinking Water", "Blankets", "Baby Food",
                           "Insulin", "Tents", "Canned Food", "First Aid Supplies", "Fresh Vegetables",
                           "Torches", "Antibiotics"};
    int w0[] = {12, 30, 25, 18, 10, 6, 35, 22, 14, 20, 8, 7};
    int v0[] = {90, 60, 80, 45, 55, 70, 65, 50, 85, 40, 20, 75};
    int p0[] = {1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1};
    n = 12;
    for (int i = 0; i < n; i++) { nm[i] = names[i]; wgt[i] = w0[i]; val[i] = v0[i]; per[i] = p0[i]; used[i] = 0; }
    int W = 100;
    int sel[MAXN], mask;

    printf("===== ITEMS (TRUCK CAPACITY %d KG) =====\n", W);
    for (int i = 0; i < n; i++)
        printf("%2d. %-18s weight %3d utility %3d %s\n", i + 1, nm[i], wgt[i], val[i], per[i] ? "[perishable]" : "");

    printf("\n===== 0/1 KNAPSACK: NO PRIORITY =====\n");
    int b = brute(W, 1, &mask);
    printf("Brute Force optimal utility: %d\n", b);
    int d = dp(W, 1, sel);
    printf("Dynamic Programming optimal utility: %d\n", d);
    show(sel, 1);
    int g = greedy(W, 1, sel);
    printf("Greedy (value/weight ratio) utility: %d\n", g);
    show(sel, 1);

    printf("\n===== PERISHABLE PRIORITY (UTILITY MULTIPLIER 2) =====\n");
    d = dp(W, 2, sel);
    printf("Dynamic Programming optimal weighted utility: %d\n", d);
    show(sel, 2);
    g = greedy(W, 2, sel);
    printf("Greedy weighted utility: %d\n", g);

    printf("\n===== MULTIPLE TRUCKS (3 TRUCKS, 100 KG EACH) =====\n");
    memset(used, 0, sizeof(used));
    int grand = 0;
    for (int t = 1; t <= 3; t++) {
        int r = dp(W, 2, sel);
        printf("Truck %d weighted utility: %d\n", t, r);
        show(sel, 2);
        for (int i = 0; i < n; i++) if (sel[i]) used[i] = 1;
        grand += r;
    }
    printf("Combined weighted utility of all trucks: %d\n", grand);

    printf("\n===== REAL-TIME DECISION: NEW ITEM ARRIVES =====\n");
    memset(used, 0, sizeof(used));
    nm[n] = "Oxygen Cylinders"; wgt[n] = 28; val[n] = 120; per[n] = 1; n++;
    printf("New item: Oxygen Cylinders weight 28 utility 120 [perishable]\n");
    d = dp(W, 2, sel);
    printf("Re-optimized weighted utility: %d\n", d);
    show(sel, 2);

    printf("\n===== PERFORMANCE ANALYSIS =====\n");
    printf("%-6s %-10s %-18s %-12s %-10s\n", "N", "W", "Method", "Utility", "Time(ms)");
    int sizes[] = {16, 20, 22};
    for (int k = 0; k < 3; k++) {
        n = sizes[k]; W = 200;
        for (int i = 0; i < n; i++) { wgt[i] = 5 + rnd(40); val[i] = 10 + rnd(100); per[i] = 0; used[i] = 0; }
        clock_t t0 = clock();
        int bb = brute(W, 1, &mask);
        clock_t t1 = clock();
        printf("%-6d %-10d %-18s %-12d %-10.3f\n", n, W, "Brute Force", bb, 1000.0 * (t1 - t0) / CLOCKS_PER_SEC);
        t0 = clock();
        int dd = dp(W, 1, sel);
        t1 = clock();
        printf("%-6d %-10d %-18s %-12d %-10.3f\n", n, W, "Dynamic Prog.", dd, 1000.0 * (t1 - t0) / CLOCKS_PER_SEC);
        t0 = clock();
        int gg = greedy(W, 1, sel);
        t1 = clock();
        printf("%-6d %-10d %-18s %-12d %-10.3f\n", n, W, "Greedy", gg, 1000.0 * (t1 - t0) / CLOCKS_PER_SEC);
    }
    int big[] = {200, 1000, 2000};
    for (int k = 0; k < 3; k++) {
        n = big[k]; W = 20000;
        for (int i = 0; i < n; i++) { wgt[i] = 5 + rnd(200); val[i] = 10 + rnd(500); per[i] = 0; used[i] = 0; }
        clock_t t0 = clock();
        int dd = dp(W, 1, sel);
        clock_t t1 = clock();
        printf("%-6d %-10d %-18s %-12d %-10.3f\n", n, W, "Dynamic Prog.", dd, 1000.0 * (t1 - t0) / CLOCKS_PER_SEC);
        t0 = clock();
        int gg = greedy(W, 1, sel);
        t1 = clock();
        printf("%-6d %-10d %-18s %-12d %-10.3f\n", n, W, "Greedy", gg, 1000.0 * (t1 - t0) / CLOCKS_PER_SEC);
        printf("%-6d %-10d %-18s %-12s %-10s\n", n, W, "Brute Force", "infeasible", "2^N");
    }
    return 0;
}

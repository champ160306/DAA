#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAXV 3000

int V;
static unsigned char adj[MAXV][MAXV];
int deg[MAXV], color[MAXV], enroll[MAXV];
unsigned long long seed = 4242;

int rnd(int m) {
    seed = seed * 6364136223846793005ULL + 1442695040888963407ULL;
    return (int)((seed >> 33) % m);
}

void clear_graph(int v) {
    V = v;
    for (int i = 0; i < V; i++) { memset(adj[i], 0, V); deg[i] = 0; enroll[i] = 0; }
}

void add_edge(int a, int b) {
    if (a == b || adj[a][b]) return;
    adj[a][b] = adj[b][a] = 1;
    deg[a]++; deg[b]++;
}

int count_colors() {
    int m = 0;
    for (int i = 0; i < V; i++) if (color[i] + 1 > m) m = color[i] + 1;
    return m;
}

int valid() {
    for (int i = 0; i < V; i++)
        for (int j = i + 1; j < V; j++)
            if (adj[i][j] && color[i] == color[j]) return 0;
    return 1;
}

int greedy_color() {
    static int used[MAXV];
    for (int i = 0; i < V; i++) color[i] = -1;
    for (int i = 0; i < V; i++) {
        memset(used, 0, sizeof(int) * (V + 1));
        for (int j = 0; j < V; j++) if (adj[i][j] && color[j] >= 0) used[color[j]] = 1;
        int c = 0;
        while (used[c]) c++;
        color[i] = c;
    }
    return count_colors();
}

int welsh_powell() {
    static int order[MAXV], used[MAXV];
    for (int i = 0; i < V; i++) { order[i] = i; color[i] = -1; }
    for (int i = 0; i < V; i++)
        for (int j = i + 1; j < V; j++)
            if (deg[order[j]] > deg[order[i]]) { int t = order[i]; order[i] = order[j]; order[j] = t; }
    for (int k = 0; k < V; k++) {
        int i = order[k];
        memset(used, 0, sizeof(int) * (V + 1));
        for (int j = 0; j < V; j++) if (adj[i][j] && color[j] >= 0) used[color[j]] = 1;
        int c = 0;
        while (used[c]) c++;
        color[i] = c;
    }
    return count_colors();
}

int dsatur() {
    static int sat[MAXV];
    static unsigned char seen[MAXV][64];
    for (int i = 0; i < V; i++) { color[i] = -1; sat[i] = 0; memset(seen[i], 0, 64); }
    for (int step = 0; step < V; step++) {
        int best = -1;
        for (int i = 0; i < V; i++) {
            if (color[i] >= 0) continue;
            if (best == -1 || sat[i] > sat[best] || (sat[i] == sat[best] && deg[i] > deg[best])) best = i;
        }
        int c = 0;
        while (1) {
            int ok = 1;
            for (int j = 0; j < V; j++) if (adj[best][j] && color[j] == c) { ok = 0; break; }
            if (ok) break;
            c++;
        }
        color[best] = c;
        for (int j = 0; j < V; j++)
            if (adj[best][j] && color[j] < 0 && c < 512 && !(seen[j][c / 8] >> (c % 8) & 1)) {
                seen[j][c / 8] |= 1 << (c % 8);
                sat[j]++;
            }
    }
    return count_colors();
}

int bt_best, bt_color[MAXV];

void bt(int i, int used) {
    if (used >= bt_best) return;
    if (i == V) { bt_best = used; memcpy(bt_color, color, sizeof(int) * V); return; }
    for (int c = 0; c <= used && c < bt_best; c++) {
        int ok = 1;
        for (int j = 0; j < i; j++) if (adj[i][j] && color[j] == c) { ok = 0; break; }
        if (!ok) continue;
        color[i] = c;
        bt(i + 1, c == used ? used + 1 : used);
        color[i] = -1;
    }
}

int backtracking() {
    for (int i = 0; i < V; i++) color[i] = -1;
    bt_best = V + 1;
    bt(0, 0);
    memcpy(color, bt_color, sizeof(int) * V);
    return bt_best;
}

void room_allocation(int *roomcap, int nr, int maxslots) {
    static int slot[MAXV], room[MAXV];
    static int roomused[MAXV][64];
    int order[MAXV];
    for (int i = 0; i < V; i++) { order[i] = i; slot[i] = -1; room[i] = -1; }
    for (int i = 0; i < V; i++)
        for (int j = i + 1; j < V; j++)
            if (color[order[j]] < color[order[i]] || (color[order[j]] == color[order[i]] && enroll[order[j]] > enroll[order[i]])) {
                int t = order[i]; order[i] = order[j]; order[j] = t;
            }
    int slots = 0;
    for (int k = 0; k < V; k++) {
        int i = order[k], placed = 0;
        for (int s = 0; s < slots && !placed; s++) {
            int conflict = 0;
            for (int j = 0; j < V; j++) if (slot[j] == s && adj[i][j]) { conflict = 1; break; }
            if (conflict) continue;
            int br = -1;
            for (int r = 0; r < nr; r++)
                if (!roomused[s][r] && roomcap[r] >= enroll[i] && (br == -1 || roomcap[r] < roomcap[br])) br = r;
            if (br >= 0) { slot[i] = s; room[i] = br; roomused[s][br] = 1; placed = 1; }
        }
        if (!placed) {
            int br = -1;
            for (int r = 0; r < nr; r++)
                if (roomcap[r] >= enroll[i] && (br == -1 || roomcap[r] < roomcap[br])) br = r;
            slot[i] = slots; room[i] = br; roomused[slots][br] = 1; slots++;
        }
    }
    printf("Room-constrained schedule uses %d slots (conflict-free colouring used %d)\n", slots, count_colors());
    if (V <= 20) {
        for (int s = 0; s < slots; s++) {
            printf("  Slot %d:", s + 1);
            for (int i = 0; i < V; i++) if (slot[i] == s) printf(" C%d(%d students)->Room%d(cap %d)", i + 1, enroll[i], room[i] + 1, roomcap[room[i]]);
            printf("\n");
        }
    }
    (void)maxslots;
}

void print_schedule() {
    int m = count_colors();
    for (int c = 0; c < m; c++) {
        printf("  Slot %d:", c + 1);
        for (int i = 0; i < V; i++) if (color[i] == c) printf(" C%d", i + 1);
        printf("\n");
    }
}

double ms(clock_t a, clock_t b) { return 1000.0 * (b - a) / CLOCKS_PER_SEC; }

int main() {
    clear_graph(10);
    int e[][2] = {{0,1},{0,2},{0,3},{1,2},{1,4},{2,5},{3,4},{3,6},{4,7},{5,6},{5,8},{6,9},{7,8},{8,9},{2,3},{4,5}};
    for (unsigned i = 0; i < sizeof(e) / sizeof(e[0]); i++) add_edge(e[i][0], e[i][1]);
    int en[] = {120, 80, 150, 60, 90, 200, 40, 70, 110, 50};
    for (int i = 0; i < V; i++) enroll[i] = en[i];

    printf("===== SMALL EXAMPLE: 10 COURSES, %d CONFLICT PAIRS =====\n", (int)(sizeof(e) / sizeof(e[0])));
    int r_greedy_color = greedy_color();
    printf("\nGreedy Coloring: %d slots, valid: %s\n", r_greedy_color, valid() ? "yes" : "no");
    print_schedule();
    int r_welsh_powell = welsh_powell();
    printf("\nWelsh-Powell: %d slots, valid: %s\n", r_welsh_powell, valid() ? "yes" : "no");
    print_schedule();
    int r_dsatur = dsatur();
    printf("\nDSATUR: %d slots, valid: %s\n", r_dsatur, valid() ? "yes" : "no");
    print_schedule();
    int r_backtracking = backtracking();
    printf("\nBacktracking (optimal): %d slots, valid: %s\n", r_backtracking, valid() ? "yes" : "no");
    print_schedule();

    printf("\n===== ROOM ALLOCATION: 10 COURSES, 4 ROOMS (CAPACITIES 220, 160, 120, 100) =====\n");
    dsatur();
    int rooms[] = {220, 160, 120, 100};
    room_allocation(rooms, 4, 10);

    int sizes[] = {500, 1000, 2000};
    for (int k = 0; k < 3; k++) {
        int courses = sizes[k], students = courses * 5;
        clear_graph(courses);
        static int taken[8];
        for (int s = 0; s < students; s++) {
            int c = 3 + rnd(3);
            for (int a = 0; a < c; a++) taken[a] = rnd(courses);
            for (int a = 0; a < c; a++) {
                enroll[taken[a]]++;
                for (int b = a + 1; b < c; b++) add_edge(taken[a], taken[b]);
            }
        }
        long edges = 0;
        for (int i = 0; i < V; i++) edges += deg[i];
        printf("\n===== LARGE DATASET: %d COURSES, %d STUDENTS, %ld CONFLICT PAIRS =====\n", courses, students, edges / 2);
        printf("%-16s %-10s %-10s %-8s\n", "Algorithm", "Slots", "Time(ms)", "Valid");
        clock_t t0 = clock();
        int r = greedy_color();
        clock_t t1 = clock();
        printf("%-16s %-10d %-10.3f %-8s\n", "Greedy", r, ms(t0, t1), valid() ? "yes" : "no");
        t0 = clock();
        r = welsh_powell();
        t1 = clock();
        printf("%-16s %-10d %-10.3f %-8s\n", "Welsh-Powell", r, ms(t0, t1), valid() ? "yes" : "no");
        t0 = clock();
        r = dsatur();
        t1 = clock();
        printf("%-16s %-10d %-10.3f %-8s\n", "DSATUR", r, ms(t0, t1), valid() ? "yes" : "no");
        int rc[40];
        int maxen = 0;
        for (int i = 0; i < V; i++) if (enroll[i] > maxen) maxen = enroll[i];
        for (int i = 0; i < 40; i++) rc[i] = maxen + (i * 5) % 40;
        printf("Room allocation with 40 rooms:\n");
        room_allocation(rc, 40, V);
    }
    return 0;
}

#include <stdio.h>

#define MAX_MOVIES 100000
#define NAME_LEN 100

struct Movie {
    int id;
    char title[NAME_LEN];
    int release_year;
    float imdb_rating;
    long watch_time_minutes;
};

struct Movie movies[MAX_MOVIES];
int movie_count = 0;

int atoi_manual(char str[]);
float atof_manual(char str[]);

int my_strlen(char str[]) {
    int i = 0;
    while (str[i] != '\0') {
        i++;
    }
    return i;
}

void my_strcpy(char dest[], char src[]) {
    int i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

void swap_movies(struct Movie *a, struct Movie *b) {
    struct Movie temp = *a;
    *a = *b;
    *b = temp;
}

int compare(struct Movie *a, struct Movie *b, int key) {
    if (key == 1) {
        if (a->imdb_rating > b->imdb_rating) return 1;
        if (a->imdb_rating < b->imdb_rating) return -1;
        return 0;
    } else if (key == 2) {
        if (a->release_year > b->release_year) return 1;
        if (a->release_year < b->release_year) return -1;
        return 0;
    } else if (key == 3) {
        if (a->watch_time_minutes > b->watch_time_minutes) return 1;
        if (a->watch_time_minutes < b->watch_time_minutes) return -1;
        return 0;
    }
    return 0;
}

int partition(struct Movie arr[], int low, int high, int key) {
    struct Movie pivot = arr[high];
    int i = low - 1;
    int j;

    for (j = low; j < high; j++) {
        if (compare(&arr[j], &pivot, key) >= 0) {
            i++;
            swap_movies(&arr[i], &arr[j]);
        }
    }
    swap_movies(&arr[i + 1], &arr[high]);
    return i + 1;
}

void quick_sort(struct Movie arr[], int low, int high, int key) {
    if (low < high) {
        int pi = partition(arr, low, high, key);
        quick_sort(arr, low, pi - 1, key);
        quick_sort(arr, pi + 1, high, key);
    }
}

int load_dataset_from_file(char filename[]) {
    FILE *fp;
    char line[300];
    int idx = 0;
    int ch;

    fp = fopen(filename, "r");
    if (fp == NULL) {
        return 0;
    }

    fgets(line, 300, fp);

    while (fgets(line, 300, fp) != NULL && idx < MAX_MOVIES) {
        int field = 0;
        int li = 0;
        char buffer[NAME_LEN];
        int bi = 0;

        movies[idx].id = idx + 1;

        for (li = 0; line[li] != '\0'; li++) {
            ch = line[li];

            if (ch == ',' || ch == '\n' || ch == '\r') {
                buffer[bi] = '\0';

                if (field == 0) {
                    my_strcpy(movies[idx].title, buffer);
                } else if (field == 1) {
                    movies[idx].release_year = atoi_manual(buffer);
                } else if (field == 2) {
                    movies[idx].imdb_rating = atof_manual(buffer);
                } else if (field == 3) {
                    movies[idx].watch_time_minutes = atoi_manual(buffer);
                }

                field++;
                bi = 0;

                if (ch == '\n') {
                    break;
                }
            } else {
                buffer[bi] = ch;
                bi++;
            }
        }
        idx++;
    }

    fclose(fp);
    return idx;
}

int atoi_manual(char str[]) {
    int i = 0;
    int result = 0;
    int sign = 1;

    if (str[0] == '-') {
        sign = -1;
        i = 1;
    }

    for (; str[i] != '\0'; i++) {
        if (str[i] >= '0' && str[i] <= '9') {
            result = result * 10 + (str[i] - '0');
        }
    }
    return result * sign;
}

float atof_manual(char str[]) {
    int i = 0;
    float result = 0;
    float fraction = 0;
    float divisor = 1;
    int sign = 1;
    int after_dot = 0;

    if (str[0] == '-') {
        sign = -1;
        i = 1;
    }

    for (; str[i] != '\0'; i++) {
        if (str[i] == '.') {
            after_dot = 1;
        } else if (str[i] >= '0' && str[i] <= '9') {
            if (after_dot == 0) {
                result = result * 10 + (str[i] - '0');
            } else {
                divisor = divisor * 10;
                fraction = fraction + (str[i] - '0') / divisor;
            }
        }
    }
    return (result + fraction) * sign;
}

void print_movies(int limit) {
    int i;
    int n = limit;

    if (n > movie_count) {
        n = movie_count;
    }

    printf("%-5s %-30s %-8s %-8s %-12s\n", "ID", "Title", "Year", "Rating", "WatchTime");
    for (i = 0; i < n; i++) {
        printf("%-5d %-30s %-8d %-8.1f %-12ld\n",
               movies[i].id,
               movies[i].title,
               movies[i].release_year,
               movies[i].imdb_rating,
               movies[i].watch_time_minutes);
    }
}

int main() {
    int key;
    int display_count;
    char filename[100];

    printf("StreamFlix Movie Recommendation Sorter\n");
    printf("Enter CSV filename (format: title,year,rating,watchtime): ");
    scanf("%s", filename);

    movie_count = load_dataset_from_file(filename);

    if (movie_count == 0) {
        printf("File not found or empty.\n");
        return 0;
    }

    printf("\nLoaded %d movies.\n\n", movie_count);

    printf("Sort by:\n1. IMDB Rating\n2. Release Year\n3. Watch Time Popularity\n");
    printf("Enter choice: ");
    scanf("%d", &key);

    quick_sort(movies, 0, movie_count - 1, key);

    printf("\nHow many top results to display? ");
    scanf("%d", &display_count);

    printf("\nSorted Recommendations:\n\n");
    print_movies(display_count);

    return 0;
}
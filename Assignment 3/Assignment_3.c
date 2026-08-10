#include <stdio.h>

#define MAX_ITEMS 100

struct Item {
    char name[50];
    float weight;
    float value;
    int divisible;
    float ratio;
    float taken;
};

struct Item items[MAX_ITEMS];
int item_count = 0;

void sort_items() {
    int i, j;
    struct Item temp;

    for (i = 0; i < item_count - 1; i++) {
        for (j = 0; j < item_count - i - 1; j++) {
            if (items[j].ratio < items[j + 1].ratio) {
                temp = items[j];
                items[j] = items[j + 1];
                items[j + 1] = temp;
            }
        }
    }
}

float fill_boat(float capacity) {
    float remaining_capacity = capacity;
    float total_value = 0;
    int i;

    for (i = 0; i < item_count; i++) {
        if (remaining_capacity <= 0) {
            break;
        }

        if (items[i].weight <= remaining_capacity) {
            items[i].taken = items[i].weight;
            total_value = total_value + items[i].value;
            remaining_capacity = remaining_capacity - items[i].weight;
        } else {
            if (items[i].divisible == 1) {
                items[i].taken = remaining_capacity;
                total_value = total_value + (items[i].ratio * remaining_capacity);
                remaining_capacity = 0;
            } else {
                items[i].taken = 0;
            }
        }
    }

    return total_value;
}

void show_result(float capacity, float total_value) {
    int i;

    printf("\nBoat Capacity: %.2f kg\n\n", capacity);
    printf("%-15s %-10s %-10s %-10s %-12s\n", "Item", "Weight", "Value", "Ratio", "Taken(kg)");

    for (i = 0; i < item_count; i++) {
        printf("%-15s %-10.2f %-10.2f %-10.2f %-12.2f\n",
               items[i].name,
               items[i].weight,
               items[i].value,
               items[i].ratio,
               items[i].taken);
    }

    printf("\nTotal Utility Value Loaded: %.2f\n", total_value);
}

int main() {
    float boat_capacity;
    float result;
    int i;

    printf("Enter boat maximum weight capacity (kg): ");
    scanf("%f", &boat_capacity);

    printf("Enter number of relief item types: ");
    scanf("%d", &item_count);

    for (i = 0; i < item_count; i++) {
        printf("\nItem %d\n", i + 1);

        printf("Name: ");
        scanf("%s", items[i].name);

        printf("Weight (kg): ");
        scanf("%f", &items[i].weight);

        printf("Utility Value: ");
        scanf("%f", &items[i].value);

        printf("Divisible (1 = Yes, e.g. food/water, 0 = No, e.g. medical kit): ");
        scanf("%d", &items[i].divisible);

        items[i].ratio = items[i].value / items[i].weight;
        items[i].taken = 0;
    }

    sort_items();

    result = fill_boat(boat_capacity);

    show_result(boat_capacity, result);

    return 0;
}
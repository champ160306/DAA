#include <stdio.h>

#define MAX_ROWS 10000

void merge(int arr[], int temp[], int left, int mid, int right)
{
    int i = left;
    int j = mid + 1;
    int k = left;

    while (i <= mid && j <= right)
    {
        if (arr[i] < arr[j])
            temp[k++] = arr[i++];
        else
            temp[k++] = arr[j++];
    }

    while (i <= mid)
        temp[k++] = arr[i++];

    while (j <= right)
        temp[k++] = arr[j++];

    for (i = left; i <= right; i++)
        arr[i] = temp[i];
}

void mergeSort(int arr[], int temp[], int left, int right)
{
    if (left >= right)
        return;

    int mid = (left + right) / 2;

    mergeSort(arr, temp, left, mid);
    mergeSort(arr, temp, mid + 1, right);

    merge(arr, temp, left, mid, right);
}

int main()
{
    FILE *fp = fopen("/home/nisl4/archive/social_media_usage.csv", "r");   // Change path if needed

    if (fp == NULL)
    {
        printf("Cannot open file.\n");
        return 1;
    }

    int choice;

    printf("Which column do you want to sort?\n");
    printf("1. Daily_Minutes_Spent\n");
    printf("2. Posts_Per_Day\n");
    printf("3. Likes_Per_Day\n");
    printf("4. Follows_Per_Day\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);

    char line[256];

    // Skip header
    fgets(line, sizeof(line), fp);

    int values[MAX_ROWS];
    int temp[MAX_ROWS];
    int count = 0;

    char user[20];
    char app[30];
    int dailyMinutes, posts, likes, follows;

    while (fscanf(fp,
                  "%19[^,],%29[^,],%d,%d,%d,%d",
                  user,
                  app,
                  &dailyMinutes,
                  &posts,
                  &likes,
                  &follows) == 6)
    {
        switch (choice)
        {
            case 1:
                values[count] = dailyMinutes;
                break;

            case 2:
                values[count] = posts;
                break;

            case 3:
                values[count] = likes;
                break;

            case 4:
                values[count] = follows;
                break;

            default:
                printf("Invalid choice.\n");
                fclose(fp);
                return 1;
        }

        count++;
    }

    fclose(fp);

    mergeSort(values, temp, 0, count - 1);

    printf("\nSorted Values:\n");

    for (int i = 0; i < count; i++)
        printf("%d ", values[i]);

    return 0;
}

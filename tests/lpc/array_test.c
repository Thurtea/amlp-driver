// array_test.c - Array manipulation functions

int *create_array(int size) {
    return allocate(size);
}

void fill_array(int *arr, int size, int value) {
    int i;
    for (i = 0; i < size; i++) {
        arr[i] = value;
    }
}

int sum_array(int *arr, int size) {
    int i, sum;
    sum = 0;
    for (i = 0; i < size; i++) {
        sum += arr[i];
    }
    return sum;
}

int find_max(int *arr, int size) {
    int i, max;
    if (size <= 0) return 0;
    max = arr[0];
    for (i = 1; i < size; i++) {
        if (arr[i] > max) max = arr[i];
    }
    return max;
}

int find_min(int *arr, int size) {
    int i, min;
    if (size <= 0) return 0;
    min = arr[0];
    for (i = 1; i < size; i++) {
        if (arr[i] < min) min = arr[i];
    }
    return min;
}

void print_array(int *arr, int size) {
    int i;
    printf("[ ");
    for (i = 0; i < size; i++) {
        if (i > 0) printf(", ");
        printf("%d", arr[i]);
    }
    printf(" ]\n");
}

void test_arrays() {
    int *arr;
    arr = create_array(5);
    fill_array(arr, 5, 10);
    printf("Array sum: %d\n", sum_array(arr, 5));
    printf("Max: %d, Min: %d\n", find_max(arr, 5), find_min(arr, 5));
}

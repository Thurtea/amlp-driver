// math.c - Arithmetic functions for testing

int add(int a, int b) {
    return a + b;
}

int subtract(int a, int b) {
    return a - b;
}

int multiply(int a, int b) {
    return a * b;
}

int divide(int a, int b) {
    if (b == 0) {
        write("Error: Division by zero\n");
        return 0;
    }
    return a / b;
}

int square(int x) {
    return x * x;
}

int abs(int x) {
    return x < 0 ? -x : x;
}

void test_math() {
    printf("add(10, 5) = %d\n", add(10, 5));
    printf("subtract(10, 5) = %d\n", subtract(10, 5));
    printf("multiply(10, 5) = %d\n", multiply(10, 5));
    printf("divide(10, 5) = %d\n", divide(10, 5));
    printf("square(7) = %d\n", square(7));
    printf("abs(-42) = %d\n", abs(-42));
}

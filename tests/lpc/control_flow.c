// control_flow.c - Tests for if/else, loops, switch statements

int factorial(int n) {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

int fibonacci(int n) {
    if (n <= 1) {
        return n;
    }
    return fibonacci(n - 1) + fibonacci(n - 2);
}

int count_to(int n) {
    int i, count;
    count = 0;
    for (i = 0; i < n; i++) {
        count++;
    }
    return count;
}

int while_sum(int n) {
    int i, sum;
    i = 0;
    sum = 0;
    while (i < n) {
        sum += i;
        i++;
    }
    return sum;
}

void test_logic(int x) {
    if (x > 0) {
        write("positive\n");
    } else if (x < 0) {
        write("negative\n");
    } else {
        write("zero\n");
    }
}

void test_switch(int day) {
    switch (day) {
        case 1:
            write("Monday\n");
            break;
        case 2:
            write("Tuesday\n");
            break;
        case 3:
            write("Wednesday\n");
            break;
        default:
            write("Other day\n");
    }
}

void test_control_flow() {
    printf("factorial(5) = %d\n", factorial(5));
    printf("fibonacci(6) = %d\n", fibonacci(6));
    printf("count_to(10) = %d\n", count_to(10));
    printf("while_sum(5) = %d\n", while_sum(5));
}

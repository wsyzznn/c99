/* test_input.c - Sample C code to test the generated lexer */
int main(void) {
    int x = 42;
    float pi = 3.14;
    char *msg = "hello world";
    if (x >= 10 && x <= 100) {
        x += 1;
        x++;
    }
    for (int i = 0; i < x; i++) {
        printf("%d\n", i);
    }
    return 0;
}

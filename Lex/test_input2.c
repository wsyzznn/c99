/* test_input2.c - More comprehensive C test */
#include <stdio.h>

typedef unsigned long size_t;

struct Point {
    int x;
    int y;
};

enum Color { RED, GREEN, BLUE };

static inline int max(int a, int b) {
    return a > b ? a : b;
}

int main(int argc, char *argv[]) {
    /* multi-line
       comment test */
    // single line comment
    
    const volatile int cv = 0xFF;
    unsigned long long ull = 0123ULL;
    float f = 3.14e-2f;
    double d = .5E+3;
    int hex = 0xDEAD;
    char c = 'A';
    char esc = '\n';
    char *s = "hello\tworld\"test";
    
    struct Point p = {10, 20};
    struct Point *pp = &p;
    int val = pp->x;
    
    int arr[100];
    for (int i = 0; i < 100; i++) {
        arr[i] = i * 2;
    }
    
    switch (cv) {
        case 0: break;
        default: continue;
    }
    
    do {
        val++;
    } while (val < 10);
    
    if (val == 10 || val != 20) {
        val <<= 2;
        val >>= 1;
        val &= 0xF;
        val |= 0x10;
        val ^= 0xFF;
    }
    
    printf("result: %d, sizeof=%lu\n", val, sizeof(int));
    
    goto end;
end:
    return 0;
}

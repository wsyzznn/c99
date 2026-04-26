// test.c
int global_var;

int main() {
    int a, b;
    a = 5;
    b = a + 10 * 2;  // 测试优先级：应该先算 10*2

    // 测试控制流与拉链回填
    if (a < b) {
        a = a + 1;
    } else {
        b = b - 1;
    }

    // 测试循环控制流
    while (a < 20) {
        a = a + 2;
    }

    return a;
}
/* 语法分析联调样例：无 typed 形参（文法里 parameter 仅为 identifier_list 或 ε） */
int f();

int f() {
    int x;
    x = 1;
    if (x < 2) {
        x = 2;
    } else {
        x = 3;
    }
    while (x > 0) {
        x = x - 1;
    }
    return x;
}

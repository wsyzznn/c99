/* 轻量工具：扫描子集/全集 .y 中的 %token 行与 %% 分段（课程「Yacc 输入处理」演示用，非完整 yacc 解析器） */
#include <cctype>
#include <cstdio>
#include <cstring>

static void trim(char *s) {
    char *e = s + std::strlen(s);
    while (e > s && std::isspace((unsigned char)e[-1]))
        *--e = 0;
    char *p = s;
    while (*p && std::isspace((unsigned char)*p))
        ++p;
    if (p != s)
        std::memmove(s, p, std::strlen(p) + 1);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        std::fprintf(stderr, "usage: yacc_subset_scan <file.y>\n");
        return 1;
    }
    FILE *fp = std::fopen(argv[1], "r");
    if (!fp) {
        std::perror(argv[1]);
        return 1;
    }
    char buf[2048];
    int sec = 0;
    std::printf("sections:\n");
    while (std::fgets(buf, sizeof buf, fp)) {
        trim(buf);
        if (buf[0] == '%' && buf[1] == '%' && buf[2] == 0) {
            ++sec;
            std::printf("  ---- section %d ----\n", sec);
            continue;
        }
        if (std::strncmp(buf, "%token", 6) == 0 && std::isspace((unsigned char)buf[6]))
            std::printf("  TOKEN: %s\n", buf);
        if (std::strncmp(buf, "%start", 7) == 0)
            std::printf("  START: %s\n", buf);
        if (std::strncmp(buf, "%union", 6) == 0)
            std::printf("  UNION: ...\n");
    }
    std::fclose(fp);
    std::printf("done (see grammar.cpp for full productions wired to LR/LALR)\n");
    return 0;
}

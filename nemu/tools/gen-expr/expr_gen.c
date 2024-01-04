#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_DEPTH 3

void generate_expression(int depth);
void generate_number();
void generate_operator();

int main() {
    srand(time(NULL)); // 初始化随机数生成器
    generate_expression(0);
    printf("\n"); // 输出表达式后换行
    return 0;
}

// 生成一个随机数，可能为 0
void generate_number() {
    printf("%d", rand() % 10);
}

// 生成一个运算符，如果是除法，确保不产生除以 0 的情况
void generate_operator(int depth) {
    switch (rand() % 4) {
        case 0:
            printf(" + ");
            break;
        case 1:
            printf(" - ");
            break;
        case 2:
            printf(" * ");
            break;
        case 3:
            printf(" / ");
            do {
                // 尝试生成一个非 0 数字作为除数
                generate_number();
            } while (rand() % 10 == 0);
            break;
    }
    generate_expression(depth + 1); // 生成另一部分的表达式
}

// 生成一个表达式
void generate_expression(int depth) {
    if (depth > MAX_DEPTH) {
        // 如果深度超过最大值，生成一个数字
        generate_number();
    } else {
        int expr_type = rand() % 3;
        switch (expr_type) {
            case 0:
                // 生成一个数字
                generate_number();
                break;
            case 1:
                // 生成一个括号表达式
                printf("(");
                generate_expression(depth + 1);
                printf(")");
                break;
            case 2:
                // 生成一个二元表达式
                generate_expression(depth + 1);
                generate_operator(depth);
                break;
        }
    }
}

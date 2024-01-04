#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define MAX_DEPTH 3
#define BUFFER_SIZE 256
#include "dbg.h"
// 声明缓冲区
char buf[BUFFER_SIZE];
char result_str[32];
 
// 用于存放下一个写入位置的指针
int pos = 0;
 // Write the program to a temporary file
const char *filename = "temp_expr_prog.c";
// 函数声明
char program_template[] = 
        "#include <stdio.h>\n"
        "int main() {\n"
        "    int result = (%s);\n"
        "    printf(\"%%d\\n\", result);\n"
        "    return 0;\n"
        "}\n";
    
char program[BUFFER_SIZE * 2];
void gen_rand_expr(int depth);

char command[BUFFER_SIZE];

// 生成一个随机数并追加到缓冲区
void append_number() {
    // 生成一个随机数并追加到buf
    int num = rand() % 1000; // 生成0-999之间的随机数
	log_info("random num = %d", num);
    pos += snprintf(buf + pos, BUFFER_SIZE - pos, "%d", num);
}

// 生成一个随机运算符并追加到缓冲区
void append_operator() {
    const char *operators = "+-*/";
    int index = rand() % 4;
    pos += snprintf(buf + pos, BUFFER_SIZE - pos, " %c ", operators[index]);
}

// 生成随机表达式的函数
void gen_rand_expr(int depth) {
    if (depth > MAX_DEPTH) {
        append_number();
    } else {
        int expr_type = rand() % 3;
        switch (expr_type) {
            case 0:
                append_number();
                break;
            case 1:
                pos += snprintf(buf + pos, BUFFER_SIZE - pos, "(");
                gen_rand_expr(depth + 1);
                pos += snprintf(buf + pos, BUFFER_SIZE - pos, ")");
                break;
            case 2:
                gen_rand_expr(depth + 1);
                append_operator();
                gen_rand_expr(depth + 1);
                break;
        }
    }
}

// 公开的接口函数，用于生成随机表达式并将其放入buf中
void generate_expression() {
    // 初始化随机种子
//	srand((unsigned int)time(NULL));
	buf[0] = '\0';
	log_info("refreshing buf = %s", buf);
    // 重置buf和pos
    pos = 0;
    // 开始生成表达式
    gen_rand_expr(0);
    // 确保字符串正确终止
    buf[pos] = '\0';
}


int main(int argc, char *argv[]) {
    // Seed the random number generator
     srand((unsigned int)time(NULL));
	int loop = 1;
	int i;
	if(argc > 1) {
		sscanf(argv[1], "%d", &loop);
	}

for(i=0; i<loop; i++) {

    // Generate a random expression
    generate_expression();
//	log_info("Here Proving the rand() error = %d",rand());
    // Create the C program
    log_info("checking buf=%s\n checking program=%s",buf, program);
  	sprintf(program, program_template, buf);

    // Write the program to a temporary file
    FILE *fp = fopen(filename, "w");
    if(fp == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }
    fputs(program, fp);
    fclose(fp);

    // Compile the program
    sprintf(command, "gcc -o temp_expr_prog %s", filename);
    int ret = system(command);
    if(ret != 0) {
        fprintf(stderr, "Compilation failed.\n");
        return EXIT_FAILURE;
    }

    // Run the compiled program and capture its output
    sprintf(command, "./temp_expr_prog");
    fp = popen(command, "r");
    if(fp == NULL) {
        perror("Error running program");
        return EXIT_FAILURE;
    }

    // Read the output
    if(fgets(result_str, sizeof(result_str), fp) == NULL) {
        perror("Error reading program output");
        return EXIT_FAILURE;
    }
    pclose(fp);

    // Remove temporary files
    remove(filename);
    remove("temp_expr_prog");

    // Output the expression and its result
    printf("Expression: %s\n", buf);
    printf("Result: %s\n", result_str);
//	buf[0] = '\0';
}
    return EXIT_SUCCESS;
}

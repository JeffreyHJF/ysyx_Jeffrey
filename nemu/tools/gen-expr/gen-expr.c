/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include "common.h"
#define MAX_DEPTH 3
// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

static word_t choose(word_t n)
{
  return rand()%n;
}
 
//生成一个随机数字 too deep recursive function bugs

static void gen_num() {
    word_t num = rand() % 9 ; // 生成0到9之间的随机数
    char num_str[2]; //临时缓冲区
    snprintf(num_str, sizeof(num_str), "%d", num);//将数字转化为字符串
    if(strlen(buf)+strlen(num_str)<sizeof(buf))
    {
    strcat(buf, num_str);//添加到buf末尾
    }
    else{return;}
}
 
 
// 生成一个随机操作符
static void gen_rand_op() {
    char ops[] = {'+', '-', '*', '/'}; // 定义操作符集合
    word_t op_index = choose(4); // 随机选择一个操作符的索引
    char op_str[2] = {ops[op_index], '\0'}; // 创建一个包含操作符的字符串
    if(strlen(buf)+strlen(op_str)<sizeof(buf))
    {
    strcat(buf, op_str); // 将选中的操作符存储到缓冲区中
    }
    else {return;}
}
 
 
// 生成随机表达式
static void gen_rand_expr(int depth) {
	if(depth > MAX_DEPTH) {
		gen_num();	
	} 
	else {
    switch (choose(3)) {
        case 0: 
            if( buf[strlen(buf) - 1]!=')')
            {
            gen_num(); // 生成随机数字
            }
            else
            {
              gen_rand_expr(depth + 1);
            }
            break;
        case 1: 
              // 避免在操作数之后立即插入左括号，而是在操作符之后插入左括号
            if (buf[0] != '\0' &&  strchr("+-*/", buf[strlen(buf) - 1]))
            {
                strcat(buf, "("); // 将左括号添加到缓冲区末尾
                gen_rand_expr(depth + 1); // 递归生成随机表达式
                strcat(buf, ")"); // 将右括号添加到缓冲区末尾
            } 
            else 
            {
                gen_rand_expr(depth + 1); // 递归生成随机表达式
            }
            break;
        default: 
            gen_rand_expr(depth + 1); // 递归生成随机表达式
            gen_rand_op(); // 生成随机操作符
            gen_rand_expr(depth + 1); // 递归生成随机表达式
            break;
      }
	}
}
 
 
// 检查表达式中的除零行为
static int check_division_by_zero() {
  char *p = buf;
  while (*p) {
    if (*p == '/' && *(p + 1) == '0') {
	  printf(" divided by 0: %s", buf);
	  buf[0] = '\0';
      return 1; // 表达式中存在除零行为
    }
    p++;
  }
  return 0; // 表达式中不存在除零行为
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
	buf[0] = '\0';
    gen_rand_expr(0);
	
	if(check_division_by_zero())
		continue;
    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    ret = fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%d %s\n", result, buf);
  }//generating result and its realted expr
  return 0;
}

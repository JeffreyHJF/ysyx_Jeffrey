#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include "common.h"
static char buf[65536] = {};
static char code_buf[65536+128] = {};
static int index_buf;
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

int choose(int n){
    int flag = rand() % 3 ; // 0 1 2
	//printf("index = %d, flag = %d. \n",index_buf, flag);
    return flag;
}
void gen_num(){
    int num = rand()% 100;
    int num_size = 0, num_tmp = num;
    while(num_tmp){
	num_tmp /= 10;
	num_size ++;
    }
    int x = 1;
    while(num_size)
    {
	x *= 10;
	num_size -- ;
    }
    x /= 10;
    while(num)
    {
	char c = num / x + '0';
	num %= x;
	x /= 10;
	buf[index_buf ++] = c;
    }
}
void gen(char c){
    buf[index_buf ++] = c;
}
void gen_rand_op(){
    char op[4] = {'+', '-', '*', '/'};
    int op_position = rand() % 4;
    buf[index_buf ++] = op[op_position];
}
bool overflow_flag;
static void gen_rand_expr() {
   //    buf[0] = '\0';	
   if(index_buf > 65530)
       	{//printf("overSize\n");
		overflow_flag = true;
		return;
		}
    switch (choose(3)) {
	case 0:
	    gen_num();
	    break;
	case 1:
	    gen('(');
	    gen_rand_expr();
	    gen(')');
	    break;
	default:
	    gen_rand_expr();
	    gen_rand_op();
	    gen_rand_expr();
	    break;
    }
}
int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);//如果命令行参数中指定了循环次数，则将其读取并存储到 loop 变量中。
  }
  int i;
  for (i = 0; i < loop; i ++) 
{
		buf[0] = '\0';
		index_buf = 0;
      //strange /000 to sprintf calling a error empty buf
      gen_rand_expr();
	  if(overflow_flag){
		continue;
	  	overflow_flag = false;
	}
      // if (check_division_by_zero)
      // {
      //   memset(buf, '\0', sizeof(buf));//将所有元素设置为空字符可以将数组重置为空
      //   i--;
      //   continue;
      // }
      sprintf(code_buf, code_format, buf);//使用生成的随机表达式按照之前format格式填充 code_buf 缓冲区
 
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
 
      printf("%u %s\n", result, buf);
  
}
  return 0;
}

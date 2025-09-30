#include <stdio.h>

int main() {
    int a = 255;

    // 输出十六进制和八进制
    printf("16進数：%x\n", a);
    printf("8進数：%o\n", a);
    printf("2進数：");

    int ar[32];  // 用于存储转换后的二进制（最多32位）
    int i = 0;

    // 将十进制 a 转换为二进制，低位在前
    while (a > 0) {
        ar[i] = a % 2;  // 取余得到当前最低位
        a /= 2;         // 除以2，进入下一位
        i++;
    }

    // 倒序输出数组，从高位到低位，得到正常的二进制显示
    for (int j = i - 1; j >= 0; j--) {
        printf("%d", ar[j]);
    }

    printf("\n");
    return 0;
}

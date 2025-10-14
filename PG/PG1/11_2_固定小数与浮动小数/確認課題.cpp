#include <stdio.h>

int main(void) {
    // 整数部分处理
    int num = 10;
    printf("整数的代入：値は %d\n", num);

    num = num << 4;  // 相当于乘以 16
    printf("num を左シフト：%d\n", num);

    num = num * 10;  // 乘以 10
    printf("num * 10 = %d\n", num);

    int num_right = num >> 4;  // 相当于除以 16
    printf("num を右シフト：%d\n", num_right);

    // 小数部分处理（使用固定小数点）
    float numf = 1.23f;
    int num2 = (int)(numf * (1 << 4));  // 1.23 × 16 ≈ 19
    printf("\n小数の代入：値は %.2f\n", numf);
    printf("// %.2f * (2の4乗) = %.2f * 16 = %.2f\n", numf, numf, numf * 16);
    printf("num2 = %d\n", num2);

    // 小数的计算后再还原
    int total = num + num2;
    printf("\n固定小数同士の計算を行うので num を左シフトする\n");
    printf("num + num2 = %d\n", total);

    int restored = total >> 4;  // 回到原来的小数形式
    printf("右シフトで戻す：%d\n", restored);

    return 0;
}

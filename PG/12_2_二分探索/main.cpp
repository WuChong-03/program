#include <stdio.h>
int main() {
    int left = 1;
    int right = 100;
    int mid;
    int isLarger; // 1: 比较大，0: 比较小或相等
    while (left <= right) {
        mid = (left + right) / 2;
        printf("思い浮かべた数字は %d より大きい：Yes なら 1、No なら 0 を入力\n", mid);
        scanf_s("%d", &isLarger);

        if (isLarger == 1) {
            left = mid + 1;
        }
        else {
            right = mid - 1;
        }

        if (left == right) {
            printf("思い浮かべた数字は %d\n", left);
            break;
        }
    }
    return 0;
}
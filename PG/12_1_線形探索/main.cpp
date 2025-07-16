#include <stdio.h>
int main() {
    int num[100];
    for (int i = 0; i < 100; i++)
    {
        num[i] = i + 1;
    }
    int target;
    printf("̽输入查找的数字:");
    scanf_s("%d", &target); 
    for (int i = 0; i < 100; i++)
    {
        if (num[i] == target)
        {
			printf("查找数字的位置:num[%d]\n", i);
			break; 
        }
    }   
    return 0;
}
# 📘 Binary Search（二分探索）

## 什么是二分探索？

二分探索是一种高效的搜索算法，它的做法是：

>**每次将数据分成两半，只在可能存在目标值的那一半继续查找**

必须是**已经排序好**的数据，能在大规模数据中**快速**定位目标。

## 基本算法流程
1. 设置目标值（比如要找的子弹）

2. 设置左右范围 left = 0, right = 数组长度 - 1

3. 重复以下步骤直到找到或范围结束：

4. 取中间下标 mid = (left + right) / 2
    - 如果 data[mid] == target，找到了
    - 如果 data[mid] < target，搜索右半边（left = mid + 1）
    - 如果 data[mid] > target，搜索左半边（right = mid - 1）

        //如果左右范围交错还没找到，就说明没有这个数

## 示例代码

```c
#include <stdio.h>

int main() {
    int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};   // 升序排列的数组
    int size = sizeof(data) / sizeof(data[0]);      //求元素数公式
    int target = 7;  // 要找的目标值
    bool isFound = false;//初始化为未找到

    int left = 0;           //初始化左端:0号元素
    int right = size - 1;   //初始化右端:元素数-1

    while (left <= right) { 
        int mid = (left + right) / 2;   //取中间元素的index
        if (data[mid] == target) {     
            isFound = true;
            break;            //若中间值等于目标值,退出循环 
        } else if (data[mid] < target) {
            left = mid + 1;   //若中间值小于目标值,更新左端继续循环
        } else {
            right = mid - 1;  //若中间值大于目标值,更新右端继续循环 
        }
    }

    if (isFound) {
        printf("找到了！\n");
    } else {
        printf("没找到。\n");
    }

    return 0;
}

```

## 注意:
break 的作用：

以前写 for 循环常常是从头到尾跑完；
如果你使用 break，就可以在满足条件时立即退出循环，节省运算时间。
```c
    while (left <= right) { 
        int mid = (left + right) / 2;   //取中间元素的index
        if (data[mid] == target) {     
            isFound = true;
            break;            //若中间值等于目标值,退出循环 
            }
    }
```

## PG课题的猜数应用程序:
```c
//设定一个范围使用二分探索找到想要的数字
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
```

假设用此方法输出33:

第一遍
    mid = (1 + 100) / 2 = 50

    问：33 > 50 吗？用户输入 0
    → right = 50 - 1 = 49
第二遍
    mid = (1 + 49) / 2 = 25
    
    问：33 > 25 吗？用户输入 1
    → left = 25 + 1 = 26
第三遍
    mid = (26 + 49) / 2 = 37

    问：33 > 37 吗？用户输入 0
    → right = 37 - 1 = 36
第四遍
    mid = (26 + 36) / 2 = 31

    问：33 > 31 吗？用户输入 1
    → left = 31 + 1 = 32
第五遍
    mid = (32 + 36) / 2 = 34

    问：33 > 34 吗？用户输入 0
    → right = 34 - 1 = 33
第六遍
    mid = (32 + 33) / 2 = 32....(小数省略)
    
    问：33 > 32 吗？用户输入 1
    → left = 32 + 1 = 33

这时 left == 33，right == 33
下一次判断：left == right，触发printf：
思い浮かべた数字は 33


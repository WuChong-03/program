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
按照流程图使用二分探索(流程图参照同文件夹图片)
#include <stdio.h>
int main() {
	//検索対象の配列の宣言、初期化
	int arr[100];	
	for (int i = 0; i < 100; i++){
		arr[i] = i + 1;
	}
	//検索する値の宣言、初期化
	int seachValue;
	//最小、最大要素、中間要素値のindex（添え字）の宣言、初期化
	int size = sizeof(arr) / sizeof(arr[0]);
	int low = 0, high = size - 1 , mid;
	//大きいか小さいかのフラグ
	int input = 0;

	printf("思い浮かべた数字(1~100)を入力してください\n");
	scanf_s("%d", &seachValue);


	while (low <= high) {
		mid = (low + high) / 2;

		if (arr[mid] == seachValue){
			printf("思い浮かべた数字は%dですね", arr[mid]);
			break;
		}
		printf("思い浮かべた数字は%dより大きいですか？(1:大きい, ０:小さい)\n", arr[mid]);
		scanf_s("%d", &input);

		if (arr[mid] < seachValue && input == 1 ){
			low = mid + 1;
		}
		if (arr[mid] > seachValue && input == 0){
			high = mid - 1;
		}
	}
    return 0;
}
```
| 轮次 | \[low, high] 区间索引 | mid索引 | mid值 arr\[mid] | 问题       | 输入 | 区间更新      |
| -- | ----------------- | ----- | -------------- | -------- | -- | --------- |
| 1  | \[0, 99]          | 49    | 50             | 33 > 50? | 0  | high = 48 |
| 2  | \[0, 48]          | 24    | 25             | 33 > 25? | 1  | low = 25  |
| 3  | \[25, 48]         | 36    | 37             | 33 > 37? | 0  | high = 35 |
| 4  | \[25, 35]         | 30    | 31             | 33 > 31? | 1  | low = 31  |
| 5  | \[31, 35]         | 33    | 34             | 33 > 34? | 0  | high = 32 |
| 6  | \[31, 32]         | 31    | 32             | 33 > 32? | 1  | low = 32  |
| 7  | \[32, 32]         | 32    | 33             | 命中！      | -  | break;    

**当 low 与 high 相等时，(low + high) / 2 的结果也等于它们自身**

**mid 不再变化，也就意味着搜索已经缩小到只剩一个数。**

### 考试题目:

https://docs.google.com/forms/d/e/1FAIpQLScdpkADf4bD5oS-zZ_xr9KEVbDGYY81FSS5Es2vp7tfu5gVzw/viewscore?viewscore=AE0zAgDc1FaaT4Oy-iLFfbtIoBmKA3Aouhgamm_8PwH5kP4UE1A1FwbBxqh6fI863w

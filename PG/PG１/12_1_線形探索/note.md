# 📘 Linear Search

## 什么是线性探索？

线性探索是一种最基本最简单的搜索算法，它的做法是：

> **从数据的开头一个个往后找，直到找到目标值为止。**

非常适合**没有排序**的数据、小规模数据，或只是想快速实现功能的场景。

---

## 基本算法流程 
1. 设置目标值（比如要找的子弹）
2. 从数组头开始依次比较
3. 如果找到了，结束搜索(break)
4. 如果找不到，一直查到最后



## 示例代码

```c
#include <stdio.h>

int main() {
    int data[] = {2, 3, 8, 10, 7, 5, 9, 11, 1, 4, 6};//乱序数组
    int size = sizeof(data) / sizeof(data[0]);//求元素数公式
    int target = 9;//目标数
    bool isFound = false; //初始化为未找到

    for (int i = 0; i < size; i++) {
        if (data[i] == target) {
            isFound;
            break;  // 一旦找到目标，立即跳出循环
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
for (int i = 0; i < Max; i++) {
    if (/* 如果找到了目标数据 */) {
        break;  // 提前结束
    }
}
```

## 游戏开发中的应用程序：
在AL应用作业中已经使用过:
- 查找未使用的子弹并发射；
```c
for (int i = 0; i < kBulletCount; i++) {
    if (!bullet[i].isShot) {
        bullet[i].isShot = true;
        bullet[i].pos = player.pos;
        break;
    }
}
```
- 查找敌人是否被击中状态；
```c
for (int i = 0; i < kEnemyCount; i++) {
    if (enemy[i].isAlive) {
        float dx = bullet.pos.x - enemy[i].pos.x;
        float dy = bullet.pos.y - enemy[i].pos.y;
        float distSq = dx * dx + dy * dy;
        float radiusSum = bullet.radius + enemy[i].radius;

        if (distSq <= radiusSum * radiusSum) {
            enemy[i].isAlive = false;
            bullet.isShot = false;
            break;
        }
    }
}
```
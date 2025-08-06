# AL_2_04_横向卷轴游戏_Scroll的意义与画面构造
## 前言
在 MT_3_00 中，我们利用 WCS 来便捷的进行物体的物理运动计算。

将WCS中的点缩放旋转平移进行绘制:

float drawX = posX * scale + screenOriginX;
float drawY = -posY * scale + screenOriginY;

但画面可绘制的范围是有限的，要实现长其之不尽的场景，必须引入 **Scroll(卷轴位置)** : 以这个位置为基准,来推动背景的卷轴滚动等操作

物体在屏幕上的位置（SCS）

= （世界位置 WCS - 镜头位置 SCROLL）

→ 进行缩放（S）和轴变换（如Y轴反转）

→ 最后加上屏幕原点（T）



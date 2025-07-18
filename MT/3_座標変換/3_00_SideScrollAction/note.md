# MT_3_00_横スクロールアクション
## 前言: 

从第三单元开始，我们学习制作横向卷轴动作游戏（如马里奥）。

    在游戏开发中，我们依然使用向量来表示位置与移动。但我们发现：

    屏幕坐标系（SCS）虽然适合绘图，但不利于进行重力、跳跃、碰撞等物理逻辑计算。

    因为像素单位太大或方向不统一，导致逻辑计算不直观。

    所以我们引入一种更抽象、灵活的系统 —— 🟧 世界坐标系（WCS），用于所有逻辑计算。


## 世界坐标系（WCS）与坐标变换

- 世界坐标系（WCS）是一种自定义的逻辑坐标系统。

- 原点、轴向、单位长度可自由设定

- 更适合表示物体的位置、速度、方向 , 便于物理运动计算

- 但无法直接用于绘图，必须转换成屏幕坐标系（SCS）才能显示

    这个过程称为：

    📌 **坐标变换**（Coordinate Transform）
    
    其本质是：

    **将世界坐标中的点，按照坐标系变换的反顺序、反方向进行变换**
    
## 坐标系 vs 点的变换顺序

        坐标轴

        S → R → T（缩放→旋转→平移）

        点坐标

        T → R → S（平移→旋转→缩放）


## TRS 具体例

```c
// 世界坐标（WCS）中的逻辑位置与大小
float posX, posY, radius;

// 世界坐标（WCS）中的屏幕原点位置, 缩放比率
const float scale = 1.0f;
const float screenOriginX = 600.0f;
const float screenOriginY = 400.0f;

// 平移 + 缩放 + 轴向
float drawX = posX * scale + screenOriginX;
float drawY = -posY * scale + screenOriginY;
float drawRadius = radius * scale;  // ✔️ 半径也要缩放

// 绘图
Novice::DrawEllipse(
    (int)drawX, (int)drawY,
    (int)drawRadius, (int)drawRadius,
    0.0f, WHITE, kFillModeSolid
);

```

以后如果需要物体旋转、以角色为中心卷轴、，就可以扩展 R（旋转）、和引入中心点偏移.本次更新先不赘述



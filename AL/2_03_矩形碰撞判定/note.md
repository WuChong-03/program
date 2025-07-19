# AL_2_03_AABB型HitBox

## 简单介绍:
AABB 全称是 **Axis-Aligned Bounding Box** , 是最常见的一种 **HitBox** 

|英语|中文|
|---|---|
|Axis|	轴（指坐标轴：X轴 / Y轴）|
|Aligned|	对齐（边平行于坐标轴）|
Bounding|	包围（包围住物体）|
Box	|盒子（矩形 / 立方体的意思）| 


- 用4条跟坐标轴对齐的边，包住一个物体，用来做碰撞判断的“盒子”。
我们之前做过的移动不超出游戏窗体,也可以看作最大号的AABB

```c
   ↑ Y轴
   |
   |      ┌────────┐
   |      │   角色  │ ← 圆形、图像等包在里面
   |      └────────┘
   |
   +------------------------→ X轴
```

## 四条边的定义:

要使用AABB型的HitBox,我们一开始就需要明确定义目标的4条边:

`left` `right` `top` `bottom`
```c
float targetLeft = target.pos.x - target.width / 2;
float targetRight = target.pos.x + target.width / 2;
float targetTop = target.pos.y - target.height / 2;
float targetBottom = target.pos.y + target.height / 2;
```

## 判定条件
只要两个矩形在 X 和 Y 方向上都有重叠（交集），就判定它们碰撞了

以玩家和目标碰撞举例:
```c
      [PlayerLeft]────────[PlayerRight]  
                        [TargetLeft]────────[TargetRight]

// 玩家right > 目标left 且 玩家left < 目标right
// x轴碰撞成立

if (playerRight > targetLeft && playerLeft < targetRight) {
    isHitX = true;
} else {
    isHitX = false;
}
```
```c
      [PlayerTop]  
      [         ]  
      [PlayerBot]  
                [TargetTop]  
                [         ]  
                [TargetBot]

// 玩家bottom > 目标top 且 玩家top < 目标bottom
// y轴碰撞成立
if (playerBottom > targetTop && playerTop < targetBottom) {
    isHitY = true;
} else {
    isHitY = false;
}
```
```c

// 横向和纵向都有交集 → 碰撞成立
if (isHitX && isHitY) {
    isHit = true;
} else {
    isHit = false;
}

```
## 关于hitbox扩展
- 实际开发中，hitbox经常会封装成函数，写得更简洁

- 本节中学习的 AABB 是“轴对齐”的矩形，非常适合初级游戏开发
以后会学到更复杂的 OBB（倾斜矩形）、圆形判定、像素级判定等

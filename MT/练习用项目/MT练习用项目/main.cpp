#include <Novice.h>                 
#include <cmath>                     

const char kWindowTitle[] = "GC1C_02_ゴ_チュウ"; 

//====================//
// 步骤1: 基本结构体定义
//====================//

// 构建二维向量结构体 xy
struct Vector2 {
	float x;        
	float y;                    
};

// 构建2x2矩阵
struct Matrix2x2 {
	float m[2][2];                 
};

//====================//
// 矩阵构建函数
//====================//

// 生成单位矩阵：不改变任何向量（相当于“乘以 1”）
Matrix2x2 MakeIdentity2() {
	Matrix2x2 I{};                   // 先清零初始化
	I.m[0][0] = 1; I.m[0][1] = 0;    // 第一行： [1, 0]
	I.m[1][0] = 0; I.m[1][1] = 1;    // 第二行： [0, 1]
	return I;                        // 返回单位矩阵
}

// 生成 2D 旋转矩阵：让向量绕原点旋转 theta（弧度）
Matrix2x2 MakeRotate2D(float theta) {
	Matrix2x2 R{};                   // 创建结果矩阵并置零
	float c = cosf(theta);           // 预计算 cos(theta)
	float s = sinf(theta);           // 预计算 sin(theta)
	R.m[0][0] = c;  R.m[0][1] = -s; // 第一行： [ cos, -sin ]
	R.m[1][0] = s;  R.m[1][1] = c; // 第二行： [ sin,  cos ]
	return R;                        // 返回旋转矩阵
}

//====================//
// 矩阵 × 向量（加工厂）
//====================//

// Mul:乘法  Mat2:2x2矩阵  Vec2:2维向量
// 功能：计算 r = M * v（把矩阵作用在向量上）
Vector2 MulMat2Vec2(Matrix2x2 M, Vector2 v) {
	Vector2 r{};                     // 结果向量 r（先清零）

	// 计算新 x 分量：第 1 行 × 列向量
	r.x = M.m[0][0] * v.x + M.m[0][1] * v.y;
	// 计算新 y 分量：第 2 行 × 列向量
	r.y = M.m[1][0] * v.x + M.m[1][1] * v.y;

	return r;                        // 把新向量返回给调用者
}

//====================//
// Windows 入口函数
//====================//
int WINAPI WinMain(_In_ HINSTANCE hInstance,            // Windows 句柄（系统传入）
	_In_opt_ HINSTANCE hPrevInstance,    // 旧实例句柄（保留，恒为空）
	_In_ LPSTR lpCmdLine,                // 命令行参数
	_In_ int nCmdShow)                   // 窗口显示方式
{
	Novice::Initialize(kWindowTitle, 1280, 720);        // 初始化 Novice：创建 1280x720 窗口

	char keys[256] = { 0 };                              // 当前帧键盘状态缓存
	char preKeys[256] = { 0 };                           // 上一帧键盘状态缓存

	// 点的初始位置：从原点（0,0）向右 200 的向量（将被当作“要变换的向量”）
	Vector2 pos = { 200.0f, 0.0f };

	// 当前旋转角度（弧度），初始为 0
	float theta = 0.0f;

	// 主循环：直到窗口关闭
	while (Novice::ProcessMessage() == 0) {              // 处理系统消息（返回 0 表示继续）
		Novice::BeginFrame();                            // 开始一帧（清屏/准备渲染）

		memcpy(preKeys, keys, 256);                      // 备份上一帧键盘状态
		Novice::GetHitKeyStateAll(keys);                 // 读取当前帧键盘状态

		//--------------------------------
		// ↓ 更新处理（游戏逻辑）
		//--------------------------------

		// 按住空格键则不断增加角度：实现持续旋转
		if (keys[DIK_SPACE]) {                           // 如果本帧 SPACE 被按住
			theta += 0.05f;                              // 角速度：每帧加 0.05 弧度
		}

		// 构建旋转矩阵 R（外部变量名 R）
		Matrix2x2 R = MakeRotate2D(theta);               // 用当前角度生成旋转矩阵
		// 把 R 作用在向量 pos 上，得到变换后的位置 newPos
		Vector2 newPos = MulMat2Vec2(R, pos);            // 这里把 R 传给函数的形参 M

		//--------------------------------
		// ↓ 绘制处理（渲染到屏幕）
		//--------------------------------

		// 从屏幕中心 (640,360) 画到 (640+newPos.x, 360-newPos.y) 的白线
		// 注意：屏幕 y 轴向下为正，因此要用 (360 - y) 把数学坐标转为屏幕坐标
		Novice::DrawLine(
			640, 360,                                    // 起点：屏幕中心
			640 + static_cast<int>(newPos.x),            // 终点 x：中心 + 旋转后 x
			360 - static_cast<int>(newPos.y),            // 终点 y：中心 - 旋转后 y（屏幕坐标系修正）
			0xFFFFFFFF                                   // 颜色：白色（ARGB）
		);

		// 左上角 HUD：提示与调试信息
		Novice::ScreenPrintf(10, 10, "Press SPACE to rotate");        // 提示：按空格旋转
		Novice::ScreenPrintf(10, 30, "theta: %.2f", theta);           // 显示当前角度
		Novice::ScreenPrintf(10, 50, "newPos.x: %.2f, newPos.y: %.2f",// 显示旋转后向量
			newPos.x, newPos.y);

		Novice::EndFrame();                             // 结束一帧（提交渲染/交换缓冲）

		// 如果本帧 ESC 被按下（上一帧没按，这一帧按了），就退出循环
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;                                       // 退出主循环
		}
	}

	Novice::Finalize();                                  // 释放 Novice 资源，关闭窗口
	return 0;                                            // 进程结束（返回给操作系统）
}

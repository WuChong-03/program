#include <Novice.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <time.h>
const char kWindowTitle[] = "GC_1C_02_ゴ_チュウ";

#pragma region 構造体
typedef struct Vector2 {
	float x;
	float y;
}Vector2;

typedef struct Circle {
	Vector2 pos;
	float radius;
	float speed;
}Circle;
#pragma endregion
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
#pragma region 初期化
	Circle circleA{ 300.0f,300.0f,50.0f,10.0f };
	Circle circleB{ 800.0f,500.0f,50.0f };
	float t = 0.0f;
	bool isMoving = false;
#pragma endregion
	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);
	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };
#pragma endregion
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();
#pragma region キー受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);
#pragma endregion
		/// ↓更新処理ここから
#pragma region circleA移動
	// SPACEキーが押された瞬間に移動開始
		if (keys[DIK_SPACE] && !preKeys[DIK_SPACE]) {
			isMoving = true;
			t = 0.0f;
		}

		// 移動中ならtを進めて、現在位置を補間する
		if (isMoving) {
			// 1秒60フレーム → 2秒 = 120フレーム → 毎フレーム1/120ずつ増やす
			t += 1.0f / 120.0f;
			if (t >= 1.0f) {
				t = 1.0f;
				isMoving = false;  // 到达后不再移动
			}

			// LERP補間
			circleA.pos.x = (1.0f - t) * 300.0f + t * circleB.pos.x;
			circleA.pos.y = (1.0f - t) * 300.0f + t * circleB.pos.y;
		}
#pragma endregion
	/// ↑更新処理ここまで


	/// ↓描画処理ここから
	Novice::DrawEllipse(
		int(circleB.pos.x),
		int(circleB.pos.y),
		int(circleB.radius),
		int(circleB.radius),
		0.0f,
		RED,
		kFillModeSolid
	);
	Novice::DrawEllipse(
		int(circleA.pos.x),
		int(circleA.pos.y),
		int(circleA.radius),
		int(circleA.radius),
		0.0f,
		WHITE,
		kFillModeSolid
	);

	/// ↑描画処理ここまで

		// フレームの終了
	Novice::EndFrame();

	// ESCキーが押されたらループを抜ける
	if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
		break;
	}
}

// ライブラリの終了
Novice::Finalize();
return 0;
}

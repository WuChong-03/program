#include <Novice.h>
#define _USE_MATH_DEFINES
#include <math.h>
typedef struct Vector2 {
	float x;
	float y;
}Vector2;

typedef struct Dot {
	Vector2 position;
}Dot;

const char kWindowTitle[] = "GC_1C_02_ゴ_チュウ";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	//初期化ここから
	Dot a = { 600,400 };
	Dot b = { 0,0 };
	Vector2 vector = { 300,200 };
	Vector2 rotatedVector = { 0,0 };
	float theta = 0.0f;

	float scale = 1.0f;
	float scaleSpeed = 0.01f;

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		scale += scaleSpeed;
		if (scale >= 2.0) {
			scaleSpeed *= -1.0f;
		}
		else if (scale <= 0.5f)
		{
			scaleSpeed *= -1.0f;
		}

		theta += 1.0f / 128.0f * static_cast<float>(M_PI);

		rotatedVector.x = (vector.x * cosf(theta) - vector.y * sinf(theta)) * scale;
		rotatedVector.y = (vector.y * cosf(theta) + vector.x * sinf(theta)) * scale;

		b.position.x = a.position.x + rotatedVector.x;
		b.position.y = a.position.y + rotatedVector.y;
		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		Novice::DrawLine(int(a.position.x), int(a.position.y), int(b.position.x), int(b.position.y), WHITE);
		///
		/// ↑描画処理ここまで
		///

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

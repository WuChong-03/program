#include <Novice.h>
#define _USE_MATH_DEFINES
#include <math.h>
typedef struct Vector2
{
	float x;
	float y;
}Vector2;

typedef struct Circle
{
	Vector2 centerPosition;
	float radius;
	float speed;
}Circle;

const char kWindowTitle[] = "GC_1C_02_ゴ_チュウ";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	//初期化ここから
	Circle circleA{ 400.0f,400.0f,100.0f,0.0f };
	Circle circleB{ 50.0f,50.0f,50.0f,10.0f };
	Vector2 inputMove{};
	Vector2 move{};
	bool isHit = false;
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
		inputMove.x = 0.0f;
		inputMove.y = 0.0f;
		if (keys[DIK_D]) {
			inputMove.x = 1.0f;
		}
		if (keys[DIK_A]) {
			inputMove.x = -1.0f;
		}
		if (keys[DIK_W]) {
			inputMove.y = -1.0f;
		}
		if (keys[DIK_S]) {
			inputMove.y = 1.0f;
		}
		float length = sqrtf(inputMove.x * inputMove.x + inputMove.y * inputMove.y);
		move.x = inputMove.x;
		move.y = inputMove.y;
		if (length != 0.0f) {
			move.x = move.x / length;
			move.y = move.y / length;
		}
		circleB.centerPosition.x += move.x * circleB.speed;
		circleB.centerPosition.y += move.y * circleB.speed;


		float distance = sqrtf(
			powf((circleA.centerPosition.x - circleB.centerPosition.x), 2.0f)
			+ powf((circleA.centerPosition.y - circleB.centerPosition.y), 2.0f)
		);
		if (distance <= circleA.radius + circleB.radius)
		{
			isHit = true;
		}
		else
		{
			isHit = false;
		}
		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		Novice::DrawEllipse(int(circleA.centerPosition.x), int(circleA.centerPosition.y), int(circleA.radius), int(circleA.radius), 0.0f, WHITE, kFillModeSolid);
	
		if (!isHit)
		{
			Novice::DrawEllipse(int(circleB.centerPosition.x), int(circleB.centerPosition.y), int(circleB.radius), int(circleB.radius), 0.0f, RED, kFillModeSolid);
		}
		else
		{
			Novice::DrawEllipse(int(circleB.centerPosition.x), int(circleB.centerPosition.y), int(circleB.radius), int(circleB.radius), 0.0f, BLUE, kFillModeSolid);
		}
		
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

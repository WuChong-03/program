#include <Novice.h>

const char kWindowTitle[] = "GC1C_02_ゴ_チュウ";

struct Vector2
{
	float x;
	float y;
};

struct Ball
{
	Vector2 position;
	Vector2 velocity;
	Vector2 acceleration;
	float radius;
	unsigned int color;
};
// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	Ball ball{
		{600.0f,500.0f}, // 初期位置
		{0.0f, 0.0f}, // 初期速度
		{0.0f, -0.8f}, // 初期加速度
		50.0f,
		WHITE // 色
	};
	bool ismoving = false; // ボールが動いているかどうか
	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

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
		if (keys[DIK_SPACE] && !preKeys[DIK_SPACE])
		{
			ismoving = true;
		}
		if (ismoving)
		{
			ball.velocity.x += ball.acceleration.x;
			ball.velocity.y += ball.acceleration.y;
			ball.position.x += ball.velocity.x;
			ball.position.y += ball.velocity.y;
		}
		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		Novice::DrawEllipse(
			int(ball.position.x),
			int(ball.position.y),
			int(ball.radius),
			int(ball.radius),
			0.0f, // 回転角
			ball.color,
			kFillModeSolid
		);
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

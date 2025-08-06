#include <Novice.h>

const char kWindowTitle[] = "GC1C_02_ゴ_チュウ";
struct Vector2
{
	float x;
	float y;
};

struct Ball
{
	Vector2 position; // ボールの位置
	Vector2 velocity; // ボールの速度
	float radius;    // ボールの半径
	unsigned int color; // ボールの色
};
// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	Ball ball{
		{300.0f,100.0f}, // 初期位置
		{10.0f, 4.0f}, // 初期速度
		50.0f, // 半径
		0xFFFFFFFF // 白色
	};
	bool isMoving = false; // ボールが動いているかどうか

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
			isMoving = true;
		}
		if (isMoving)
		{
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
			static_cast<int>(ball.position.x),
			static_cast<int>(ball.position.y),
			static_cast<int>(ball.radius),
			static_cast<int>(ball.radius),
			0.0f, // 回転角
			ball.color, // 色
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

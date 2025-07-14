#include <Novice.h>
#include <math.h>
const char kWindowTitle[] = "GC_1C_02_ゴ_チュウ";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);
	float posX = 400.0f;
	float posY = 400.0f;
	float radius = 50.0f;
	float speed = 10.0f;

	float inputDirX = 0.0f;
	float inputDirY = 0.0f;

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
		inputDirX = 0.0f;
		inputDirY = 0.0f;
		if (keys[DIK_D]) {
			inputDirX += 1.0f;
		}
		if (keys[DIK_A]) {
			inputDirX -= 1.0f;
		}
		if (keys[DIK_W]) {
			inputDirY -= 1.0f;
		}
		if (keys[DIK_S]) {
			inputDirY += 1.0f;
		}

		float length = sqrtf(inputDirX * inputDirX + inputDirY * inputDirY);
		float dirX = inputDirX;
		float dirY = inputDirY;
		if (length != 0.0f) {
			dirX = dirX / length;
			dirY = dirY / length;
		}

		posX += dirX * speed;
		posY += dirY * speed;
		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		Novice::ScreenPrintf(0, 45, "dirX : %f", dirX);
		Novice::ScreenPrintf(0, 60, "dirY: %f", dirY);
		Novice::ScreenPrintf(0, 0, "inputdirX : %f", inputDirX);
		Novice::ScreenPrintf(0, 15, "inputdirY: %f", inputDirY);
		Novice::DrawBox(
			int(posX), int(posY),
			int(radius * 2), int(radius * 2),
			0.0f,
			WHITE,
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

#include <Novice.h>

const char kWindowTitle[] = "GC_1C_02_ゴ_チュウ";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);
	int posX = 100;
	int	posY = 100;
	int speed = 10;
	int radius = 30;

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






		//移動
		if (keys[DIK_D]) {
			posX = posX + speed;

		}
		if (keys[DIK_A]) {
			posX = posX - speed;

		}
		if (keys[DIK_W]) {
			posY = posY - speed;

		}
		if (keys[DIK_S]) {
			posY = posY + speed;
		}
		//座標を固定
		if (posX >= 1280 - radius) {
			posX = 1280 - radius;
		}

		if (posY >= 720 - radius) {
			posY = 720 - radius;
		}

		if (posX <= radius) {
			posX = radius;
		}

		if (posY <= radius) {
			posY = radius;
		}
		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		Novice::DrawEllipse(posX, posY, radius, radius, 0, WHITE, kFillModeSolid);
		Novice::ScreenPrintf(10, 10, "W:UP A:Left S:Down D:Right");
		Novice::ScreenPrintf(10, 40, "posX: %d", posX);
		Novice::ScreenPrintf(100, 40, " / posY: %d", posY);


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

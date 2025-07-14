#include <Novice.h>

const char kWindowTitle[] = "GC_1C_02_ゴ_チュウ";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);
	float posX = 640.0f;
	float posY = 360.0f;
	float radius = 30.0f;

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




		float speedx = 0.0f;
		float speedy = 0.0f;

		//移動
		if (keys[DIK_D]) {
			speedx = 10.0f;
			posX += speedx;
		}
		if (keys[DIK_A]) {
			speedx = -10.0f;
			posX += speedx;
		}
		if (keys[DIK_W]) {
			speedy = -10.0f;
			posY += speedy;
		}
		if (keys[DIK_S]) {
			speedy = 10.0f;
			posY += speedy;
		}
		if (keys[DIK_W] && keys[DIK_A]) {
			speedx = -7.071f;
			speedy = -7.071f;
		}
		if (keys[DIK_W] && keys[DIK_D]) {
			speedx = 7.071f;
			speedy = -7.071f;
		}
		if (keys[DIK_S] && keys[DIK_A]) {
			speedx = -7.071f;
			speedy = 7.071f;
		}
		if (keys[DIK_S] && keys[DIK_D]) {
			speedx = 7.071f;
			speedy = 7.071f;
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
		/// 
		Novice::DrawEllipse(static_cast<int>(posX), static_cast<int>(posY), static_cast<int>(radius), static_cast<int>(radius), 0, WHITE, kFillModeSolid);

		//移動の説明
		Novice::ScreenPrintf(20, 20, "W||ArrowUP:UP A||ArrowLeft:Left S||ArrowDown:Down D||ArrowRight:Right");

		//値
		Novice::ScreenPrintf(10, 50, "  posX: %4.1f", posX);
		Novice::ScreenPrintf(100, 50, "    /   posY: %4.1f", posY);
		Novice::ScreenPrintf(10, 70, "speedx:  %4.1f", speedx);
		Novice::ScreenPrintf(100, 70, "    / speedy:  %4.1f", speedy);


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

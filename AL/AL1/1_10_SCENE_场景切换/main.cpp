#include <Novice.h>
#define _USE_MATH_DEFINES
#include <math.h>

const char kWindowTitle[] = "GC_1C_02_ゴ_チュウ";

enum SCENE {
	TITLE,
	PLAY,
	RESULT
};


// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	SCENE currentScene = TITLE;

	int	titleGrHandle = Novice::LoadTexture("./images/bg.png");
	int bg1Y = 0;
	int bg2Y = -720;
	int speed = 5;

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
		/// 
		switch (currentScene) {
		case TITLE:
			if (keys[DIK_SPACE] && !preKeys[DIK_SPACE]) {
				currentScene = PLAY;
			}
			bg1Y += speed;
			bg2Y += speed;
			if (bg1Y >= 720) {
				bg1Y = -720;
			}
			if (bg2Y >= 720) {
				bg2Y = -720;
			}
			break;
		case PLAY:
			if (keys[DIK_SPACE] && !preKeys[DIK_SPACE]) {
				currentScene = RESULT;
			}
			break;
		case RESULT:
			if (keys[DIK_SPACE] && !preKeys[DIK_SPACE]) {
				currentScene = TITLE;
			}
			bg1Y = 0;
			bg2Y = -720;
			break;
		}
		///
		/// ↑更新処理ここまで
		///


		///
		/// ↓描画処理ここから
		///
		/// 
		/// 
		Novice::ScreenPrintf(0, 0, "scene : %d", currentScene);

		switch (currentScene)
		{
		case TITLE:
			Novice::DrawSprite(0, bg1Y, titleGrHandle, 1, 1, 0.0f, WHITE);
			Novice::DrawSprite(0, bg2Y, titleGrHandle, 1, 1, 0.0f, WHITE);
			break;
		case PLAY:
			Novice::DrawBox(0, 0, 1280, 720, 0.0f, RED, kFillModeSolid);
			break;
		case RESULT:
			Novice::DrawBox(0, 0, 1280, 720, 0.0f, BLUE, kFillModeSolid);
			break;
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

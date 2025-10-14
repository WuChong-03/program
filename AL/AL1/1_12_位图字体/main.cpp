#include <Novice.h>
#define _USE_MATH_DEFINES
#include <math.h>

const char kWindowTitle[] = "GC_1C_02_ゴ_チュウ";


// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);
	int numberFonts[10] = {
		 Novice::LoadTexture("./Images/0.png"),
		 Novice::LoadTexture("./Images/1.png"),
		 Novice::LoadTexture("./Images/2.png"),
		 Novice::LoadTexture("./Images/3.png"),
		 Novice::LoadTexture("./Images/4.png"),
		 Novice::LoadTexture("./Images/5.png"),
		 Novice::LoadTexture("./Images/6.png"),
		 Novice::LoadTexture("./Images/7.png"),
		 Novice::LoadTexture("./Images/8.png"),
		 Novice::LoadTexture("./Images/9.png"),
	};
	int targetNumber = 65535;//表示する数

	const int numberCount = 5;
	int numberArray[numberCount] = {};

	for (int i = 0; i < numberCount; i++) {
		numberArray[i] = targetNumber / (int)powf(float(10.0f), float(numberCount - 1 - i));
		targetNumber %= (int)powf(float(10.0f), float(numberCount - 1 - i));
	}

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

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		/// 

		for (int i = 0; i < numberCount; i++) {
			Novice::DrawSprite(100 + 71 * i, 100, numberFonts[numberArray[i]], 1, 1, 0.0f, WHITE);
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

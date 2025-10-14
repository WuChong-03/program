#include <Novice.h>

const char kWindowTitle[] = "GC_1C_02_ゴ_チュウ";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	//初期化処理
	int deHandle;
	int	dkHandle;
	int jpHandle;

		deHandle = Novice::LoadTexture("./images/de.png");//长 170 宽 256
		dkHandle = Novice::LoadTexture("./images/dk.png");
		jpHandle = Novice::LoadTexture("./images/jp.png");
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

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		/// 
		/// //长 170 宽 256
					Novice::ScreenPrintf(10, 280, "Image function");
						Novice::DrawSprite(10, 300, deHandle, 1, 1, 0.0f, WHITE);
						Novice::DrawSpr	ite(298, 300, jpHandle, 1, 1, 0.0f, WHITE);
						Novice::DrawSprite(586, 300, dkHandle, 1, 1, 0.0f, WHITE);

					Novice::ScreenPrintf(10, 10, "Shape function");
//DE
						Novice::DrawBox(10, 30, 256, 56, 0.0f, 0x000000FF, kFillModeSolid);
						Novice::DrawBox(10, 86, 256, 57, 0.0f, 0xDD0000FF, kFillModeSolid);
						Novice::DrawBox(10, 143, 256, 56, 0.0f, 0xFF7E00FF, kFillModeSolid);
//JP
						Novice::DrawBox(298, 30, 256, 170, 0.0f, WHITE, kFillModeSolid);
						Novice::DrawEllipse(426, 115, 42, 42, 0.0f, RED, kFillModeSolid);
//DK
						Novice::DrawBox(586, 30, 256, 170, 0.0f, RED, kFillModeSolid);
						Novice::DrawBox(586, 102, 256, 22, 0.0f, WHITE, kFillModeSolid);
						Novice::DrawBox(658, 30, 22, 170, 0.0f, WHITE, kFillModeSolid);
			

		//Novice::DrawLine(0, 0, 100, 100, RED);
		//Novice::DrawBox(0, 0, 1280, 720, 0.0f, WHITE, kFillModeSolid);
		//Novice::DrawEllipse(100, 100, 50, 50, 0.0f, RED, kFillModeSolid);
		//Novice::DrawTriangle(0, 0, 100, 100, 100, 0, RED, kFillModeSolid);
		//Novice::ScreenPrintf(0,0,"Hello Windows");
	






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

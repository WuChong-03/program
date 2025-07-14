#include <Novice.h>

const char kWindowTitle[] = "GC_1C_02_ゴ_チュウ";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };



	int isAlive = 1;
	int timer = 120;


	int graphHandle1;
	int graphHandle2;
	graphHandle1 = Novice::LoadTexture("./image/explosion.png");
	graphHandle2 = Novice::LoadTexture("./image/pien.png");
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
			isAlive = 0;
		}

		if (isAlive == 0)
		{
			timer -= 1;

			if (timer == 0)
			{
				timer = 120;
				isAlive = 1;
			}
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		Novice::DrawBox(0, 0, 1280, 720, 0.0f, WHITE, kFillModeSolid);
		Novice::DrawBox(0, 600, 1280, 120, 0.0f, BLACK, kFillModeSolid);
		Novice::ScreenPrintf(30, 650, "isAlive = %d", isAlive);
		Novice::ScreenPrintf(230, 650, "timer = %d", timer);
		Novice::ScreenPrintf(500, 630, "Press Space wo Exprode Pien");





		if (isAlive != 0)
		{
			Novice::DrawSprite(500, 200, graphHandle2, 1, 1, 0.0f, WHITE);
		}
		else if (isAlive == 0 && timer > 90)
		{
			Novice::DrawSprite(500, 200, graphHandle1, 1, 1, 0.0f, WHITE);
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
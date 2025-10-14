#include <Novice.h>

const char kWindowTitle[] = "GC_1C_02_ゴ_チュウ";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	//初期化処理
	int image;
		image = Novice::LoadTexture("./images/uk.png");
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
		    //文字
			Novice::ScreenPrintf(10,10,"Shape Function");

			//蓝矩形
			Novice::DrawBox(10, 40, 512, 340, 0.0f, 0x012169, kFillModeSolid);

			//白左六边
			//Novice::DrawTriangle(10, 40, 80, 40, 10, 85, WHITE, kFillModeSolid);
			//Novice::DrawTriangle(452, 380, 80, 40, 10, 85, WHITE, kFillModeSolid);
			//Novice::DrawTriangle(452, 380, 80, 40, 522, 335, WHITE, kFillModeSolid);
			//Novice::DrawTriangle(452, 380, 522, 380, 522, 335, WHITE, kFillModeSolid);

			//白右六边
			//Novice::DrawTriangle(522,40,452,40,522,85, WHITE, kFillModeSolid);
			//Novice::DrawTriangle(10, 335, 452, 40, 522, 85, WHITE, kFillModeSolid);
			//Novice::DrawTriangle(10, 335, 80, 380, 522, 85, WHITE, kFillModeSolid);
			//Novice::DrawTriangle(10, 335, 80, 380, 10,380, WHITE, kFillModeSolid);


			//左上右下红
			Novice::DrawTriangle(10, 40, 10, 70, 266, 210, 0xc8102eff, kFillModeSolid);
			Novice::DrawTriangle(10, 70, 266, 210, 266, 240, 0xc8102eff, kFillModeSolid);
			Novice::DrawTriangle(266,210, 266, 180, 522, 350, 0xc8102eff, kFillModeSolid);
			Novice::DrawTriangle(266, 210, 522, 380, 522, 350, 0xc8102eff,kFillModeSolid);

			//右上左下红
			Novice::DrawTriangle(487,40,522,40,226,210, 0xc8102eff, kFillModeSolid);
			Novice::DrawTriangle(226, 210, 266, 210, 522, 40, 0xc8102eff, kFillModeSolid);
			Novice::DrawTriangle(10, 380, 50, 380, 266, 210, 0xc8102eff, kFillModeSolid);
			Novice::DrawTriangle(50, 380, 306, 210, 266, 210, 0xc8102ffe, kFillModeSolid);

			//白十字
			//Novice::DrawBox(10, 170, 512, 80, 0.0f, WHITE, kFillModeSolid);
			//Novice::DrawBox(226, 40, 80, 340, 0.0f, WHITE, kFillModeSolid);
			//红十字
			//Novice::DrawBox(246, 40, 40, 340, 0.0f, 0xc8102eff, kFillModeSolid);
			//Novice::DrawBox(10, 190, 512, 40, 0.0f, 0xc8102eff, kFillModeSolid);
			 
			 

			



			






			Novice::ScreenPrintf(640,10,"Image Function");
			Novice::DrawSprite(640, 40, image, 1, 1, 0.0f, WHITE);





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

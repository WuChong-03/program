#include <Novice.h>
#define _USE_MATH_DEFINES
#include <math.h>

const char kWindowTitle[] = "GC_1C_02_ゴ_チュウ";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	bool start = 0;
	float weight = 200.0f;
	float height = 100.0f;
	float theta = 0.0f;

	//kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk
	float kLeftTopX = -weight / 2.0f;
	float kLeftTopY = -height / 2.0f;
	float kRightTopX = weight / 2.0f;
	float kRightTopY = -height / 2.0f;
	float kLeftBomtomX = -weight / 2.0f;
	float kLeftBomtomY = height / 2;
	float KRightBomtomX = weight / 2;
	float KRightBomtomY = height / 2;

	int whiteTextureHandle;
	whiteTextureHandle = Novice::LoadTexture("./NoviceResources/white1x1.png");
	float posX = 400.0f;
	float posY = 400.0f;


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

		if (keys[DIK_SPACE] && !preKeys[DIK_SPACE]) {
			start = 1;
		}




		if (start)
		{
			Novice::ScreenPrintf(0, 0, "start:%d", start);

				theta += 0.1f;				// degree 360 == radian 2 * M_PI（约 6.28318)
			if (theta >= 2 * M_PI)		//フレームごとに　thetaを足す　,　360だったら　0にする。
			{
				theta = 0.0f;
			}
		}


		//回転したraotated座標
		float leftTopRotatedX = kLeftTopX * cosf(theta) - kLeftTopY * sinf(theta);
		float leftTopRotatedY = kLeftTopY * cosf(theta) + kLeftTopX * sinf(theta);


		float rightTopRotatedX = kRightTopX * cosf(theta) - kRightTopY * sinf(theta);
		float rightTopRotatedY = kRightTopY * cosf(theta) + kRightTopX * sinf(theta);


		float leftBottomRotatedX = kLeftBomtomX * cosf(theta) - kLeftBomtomY * sinf(theta);
		float leftBottomRotatedY = kLeftBomtomY * cosf(theta) + kLeftBomtomX * sinf(theta);

		float rightBottomRotatedX = KRightBomtomX * cosf(theta) - KRightBomtomY * sinf(theta);
		float rightBottomRotatedY = KRightBomtomY * cosf(theta) + KRightBomtomX * sinf(theta);



		//実際座標
		float LeftTopPosionX = leftTopRotatedX + posX;
		float LeftTopPosionY = leftTopRotatedY + posY;

		float RightTopPosionX = rightTopRotatedX + posX;
		float RightTopPosionY = rightTopRotatedY + posY;

		float LeftBottomPosionX = leftBottomRotatedX + posX;
		float LeftBottomPosionY = leftBottomRotatedY + posY;

		float RightBottomPosionX = rightBottomRotatedX + posX;
		float RightBottomPosionY = rightBottomRotatedY + posY;




		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		Novice::DrawQuad(
			int(LeftTopPosionX), int(LeftTopPosionY),
			int(RightTopPosionX), int(RightTopPosionY),
			int(LeftBottomPosionX), int(LeftBottomPosionY),
			int(RightBottomPosionX),int(RightBottomPosionY),
			0,0,
			int(weight),int(height),
			whiteTextureHandle,
			WHITE);




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

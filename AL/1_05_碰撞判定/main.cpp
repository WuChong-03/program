#include <Novice.h>
#include <math.h>
const char kWindowTitle[] = "GC_1C_02_ゴ_チュウ";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	//white ball
	float targetPosX = 400.0f;
	float targetPosY = 400.0f;
	float targetRadius = 100.0f;
	bool isHit = false;

	//player
	float playerPosX = 100.0f;
	float playerPosY = 100.0f;
	float playerRadius = 50.0f;
	int redMovex = 0;
	int	redMovey = 0;
	float redSpeed = 10.0f;

	//衝突判定:ab絶対値、distance距離
	float a = 0.0f;
	float b = 0.0f;
	float length = 0.0f;

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
		//移動処理
		redMovex = 0;
		redMovey = 0;
		redSpeed = 10.0f;
		if (keys[DIK_W]) {
			redMovey = -1;
		}
		if (keys[DIK_S]) {
			redMovey = 1;
		}
		if (keys[DIK_A]) {
			redMovex = -1;
		}
		if (keys[DIK_D]) {
			redMovex = 1;
		}
		playerPosX += redSpeed * redMovex;
		playerPosY += redSpeed * redMovey;


		//画面外超えない処理
		if (playerPosX >= 1280 - playerRadius) {
			playerPosX = 1280 - playerRadius;
		}
		if (playerPosX <= playerRadius) {
			playerPosX = playerRadius;
		}
		if (playerPosY >= 720 - playerRadius) {
			playerPosY = 720 - playerRadius;
		}
		if (playerPosY <= playerRadius) {
			playerPosY = playerRadius;
		}


		//衝突判定
		a = targetPosX - playerPosX;
		b = targetPosY - playerPosY;
		length = static_cast<float>(sqrt(a * a + b * b));
		if (length <= targetRadius + playerRadius)
		{
			isHit = 1;
		}
		else
		{
			isHit = 0;
		}
		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから　　　//必ず整数型で
		///

		///運行状況
		Novice::ScreenPrintf(10, 10, "player to target(x)%.6f", fabs(a));
		Novice::ScreenPrintf(10, 30, "player to target(y)%.6f", fabs(b));
		Novice::ScreenPrintf(10, 50, "player to target(length)%.6f", length);
		Novice::ScreenPrintf(10, 70, "player radius + target radius :%.6f", targetRadius + playerRadius);
		Novice::ScreenPrintf(10, 90, "isHit :%d", isHit);

		/// 敵
		Novice::DrawEllipse(
			static_cast<int>(targetPosX), static_cast<int>(targetPosY),
			static_cast<int>(targetRadius), static_cast<int>(targetRadius),
			0.0f, WHITE, kFillModeWireFrame);

		if (isHit)
		{
			Novice::DrawEllipse(
				static_cast<int>(targetPosX), static_cast<int>(targetPosY),
				static_cast<int>(targetRadius), static_cast<int>(targetRadius),
				0.0f, WHITE, kFillModeSolid);
		}

		///player
		Novice::DrawEllipse(
			static_cast<int>(playerPosX), static_cast<int>(playerPosY),
			static_cast<int>(playerRadius), static_cast<int>(playerRadius),
			0.0f, WHITE, kFillModeWireFrame);

		///線
		Novice::DrawLine(
			static_cast<int>(targetPosX),
			static_cast<int>(targetPosY),

			static_cast<int>(playerPosX),
			static_cast<int>(playerPosY),
			WHITE);


		Novice::DrawLine(
			static_cast<int>(playerPosX),
			static_cast<int>(targetPosY),

			static_cast<int>(playerPosX),
			static_cast<int>(playerPosY),
			WHITE);

		Novice::DrawLine(
			static_cast<int>(targetPosX),
			static_cast<int>(targetPosY),

			static_cast<int>(playerPosX),
			static_cast<int>(targetPosY),
			WHITE);

		///
		///
		/// ↑ static_cast<int>(posy)
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

#include <Novice.h>

const char kWindowTitle[] = "GC_1C_02_ゴ_チュウ";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);


	//player初期化
	int playerPosX = 640;
	int playerPosY = 360;
	int radius = 30;


	//bullet初期化
	bool isBulletshot = false;
	int bulletPosX = 0;
	int bulletPosY = 0;

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
		//フレームごとにベクトルと速度を０にする
		int moveX = 0;
		int moveY = 0;
		int speed = 10;

		//八つの方向に同じ速度で移動させる
		if (keys[DIK_W]) {
			moveY = -1;
		}
		if (keys[DIK_A]) {
			moveX = -1;
		}
		if (keys[DIK_S]) {
			moveY = 1;
		}
		if (keys[DIK_D]) {
			moveX = 1;
		}
		if ((keys[DIK_W] && keys[DIK_D]) ||
			(keys[DIK_W] && keys[DIK_A]) ||
			(keys[DIK_S] && keys[DIK_D]) ||
			(keys[DIK_S] && keys[DIK_A])) {
			speed = 7;
		}
		if ((keys[DIK_A] && keys[DIK_D]) ||
			(keys[DIK_W] && keys[DIK_S])) {
			speed = 0;
		}
		playerPosX += speed * moveX;
		playerPosY += speed * moveY;


		//越えていたら停止させる
		if (playerPosX >= 1280 - radius) {
			playerPosX = 1280 - radius;
		}
		if (playerPosX <= radius) {
			playerPosX = radius;
		}
		if (playerPosY >= 720 - radius) {
			playerPosY = 720 - radius;
		}
		if (playerPosY <= radius) {
			playerPosY = radius;
		}

		//弾の発射
		if (keys[DIK_SPACE] && !preKeys[DIK_SPACE] && !isBulletshot) {
			isBulletshot = 1;
			bulletPosX = playerPosX;
			bulletPosY = playerPosY;
		}
		// 弾道計算
		if (isBulletshot) {
			bulletPosY -= 10;
			if (bulletPosY <= 0){
				isBulletshot = false;
			}
		}




		///
		/// ↑更新処理ここまで
		///







		///
		/// ↓描画処理ここから
		///
		//運行状況
		Novice::ScreenPrintf(0, 0, "W:up A:left S:down D:right");
		Novice::ScreenPrintf(0, 20, "space:shot");
		Novice::ScreenPrintf(200, 20, "speedX = %d", speed);
		Novice::ScreenPrintf(0, 60, "playerPosX = %d", playerPosX);
		Novice::ScreenPrintf(200, 60, "PlayerPosY = %d", playerPosY);
		Novice::ScreenPrintf(400, 60, "isBulletshot = %d", isBulletshot);
		Novice::ScreenPrintf(0, 80, "bulletPosX = %d", bulletPosX);
		Novice::ScreenPrintf(200, 80, "bulletPosy = %d", bulletPosY);



		//player
		Novice::DrawEllipse(playerPosX, playerPosY, radius, radius, 0.0f, WHITE, kFillModeSolid);


		//弾
		if (isBulletshot) {
			Novice::DrawTriangle(
				bulletPosX, bulletPosY,
				bulletPosX - 10, bulletPosY + 10,
				bulletPosX + 10, bulletPosY + 10,
				RED, kFillModeSolid);
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

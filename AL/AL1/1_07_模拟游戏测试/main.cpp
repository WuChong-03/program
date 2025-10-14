#include <Novice.h>
#define _USE_MATH_DEFINES
#include <math.h>


const char kWindowTitle[] = "GC_1C_02_ゴ_チュウ";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);


	//player
	float playerPosX = 640.0f;
	float playerPosY = 360.0f;
	float playerRadius = 20.0f;

	//bullet
	float bulletPosX = 0.0f;
	float bulletPosY = 0.0f;
	float bulletRadius = 15.0f;
	float bulletSpeed = 30.0f;

	//bullet頂点座標
	float bulletTopX = 0.0f;
	float bulletTopY = 0.0f;
	float bulletLeftX = 0.0f;
	float bulletLeftY = 0.0f;
	float bulletRightX = 0.0f;
	float bulletRightY = 0.0f;



	//発射のスイッチ
	bool isShot = false;


	//enemy　
	float enemyPosX = 100.0f;
	float enemyPosY = 100.0f;
	float enemyRadius = 30.0f;
	float enemySpeed = 10.0f;

	//enemy復活
	bool isEnemyAlive = 1;
	int timer = 60;

	//player移動、斜め移動
	float degree = 0.0f;
	float theta = 0.0f;
	float moveX = 0.0f;
	float moveY = 0.0f;
	float playerSpeed = 0.0f;

	//bullet enemy衝突
	int graphHandle;
	graphHandle = Novice::LoadTexture("./image/explode.png");
	float distance = 0.0f;
	float e2bX = 0.0f;
	float e2bY = 0.0f;


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


		//player speed フレームごとに初期化
		if (keys[DIK_W] || keys[DIK_A] || keys[DIK_S] || keys[DIK_D]) {
			playerSpeed = 10.0f;
		}
		else {
			playerSpeed = 0.0f;
		}
		//player degree
		if (keys[DIK_D]) {
			degree = 0.0f;
		}
		if (keys[DIK_W]) {
			degree = 90.0f;
		}
		if (keys[DIK_A]) {
			degree = 180.0f;
		}
		if (keys[DIK_S]) {
			degree = 270.0f;
		}
		if (keys[DIK_W] && keys[DIK_D]) {
			degree = 45.0f;
		}
		if (keys[DIK_W] && keys[DIK_A]) {
			degree = 135.0f;
		}
		if (keys[DIK_S] && keys[DIK_A]) {
			degree = 225.0f;
		}
		if (keys[DIK_S] && keys[DIK_D]) {
			degree = 315.0f;
		}
		//player　移動
		theta = degree * (float(M_PI) / 180.0f);  //①ラジアンに変更
		moveX = cosf(theta);					  //②単位円上のｘを求める
		moveY = sinf(theta);					  //②単位円上のｙを求める
		playerPosX += moveX * playerSpeed;			  //③speedとかけ  実際座標に ｘ足す
		playerPosY -= moveY * playerSpeed;			  //③speedとかけ  実際座標に ｙ引く
		//player   画面を超えない
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



		//bullet　 発射判定
		if (keys[DIK_SPACE] && !preKeys[DIK_SPACE] && !isShot) {
			isShot = 1;

			bulletPosX = playerPosX;			//bullet と player を同じ中心点にする
			bulletPosY = playerPosY;
		}
		if (bulletPosY <= 0)
		{
			isShot = 0;
		}
		//bullet　 頂点
		bulletTopX = bulletPosX + (bulletRadius * cosf(90.0f * float(M_PI) / 180.0f));
		bulletTopY = bulletPosY - (bulletRadius * sinf(90.0f * (float(M_PI) / 180.0f)));
		bulletLeftX = bulletPosX + (bulletRadius * cosf(210.0f * (float(M_PI) / 180.0f)));
		bulletLeftY = bulletPosY - (bulletRadius * sinf(210.0f * (float(M_PI) / 180.0f)));
		bulletRightX = bulletPosX + (bulletRadius * cosf(330.0f * (float(M_PI) / 180.0f)));
		bulletRightY = bulletPosY - (bulletRadius * sinf(330.0f * (float(M_PI) / 180.0f)));
		//bullet　移動
		if (isShot) {
			bulletPosY -= bulletSpeed;
		}

		//enemy　移動
		if (isEnemyAlive)
		{
			enemyPosX += enemySpeed;
		}
		else
		{
			enemyPosX += 0;
		}

		//enemy　反射
		if (enemyPosX >= 1280.0f - enemyRadius || enemyPosX <= enemyRadius) {
			enemySpeed *= -1.0f;
		}





		//衝突判定
		e2bX = enemyPosX - bulletPosX, e2bY = enemyPosY - bulletPosY;					//差分
		distance = static_cast<float>(sqrt(e2bX * e2bX + e2bY * e2bY));			//距離
		if (distance <= enemyRadius + bulletRadius)				//衝突判定　距離＜＝半径
		{
			isEnemyAlive = 0;
		}


		//死んだあど判定
		if (!isEnemyAlive) {
			timer -= 1;			//復活タイマー
			if (timer == 0)
			{
				timer = 60;				//復活タイマーを直す
				isEnemyAlive = 1;		//復活
			}
		}

		///
		/// ↑更新処理ここまで
		///





		///
		/// ↓描画処理ここから
		///

		//三角形を描画
		if (isShot) {
			Novice::DrawTriangle(int(bulletTopX), int(bulletTopY), int(bulletLeftX), int(bulletLeftY), int(bulletRightX), int(bulletRightY), RED, kFillModeSolid);
		}


		//プレイヤーを描画
		Novice::DrawBox(int(playerPosX - playerRadius), int(playerPosY - playerRadius), int(playerRadius * 2), int(playerRadius * 2), 0.0f, WHITE, kFillModeSolid);


		//生きてるenemy　；死亡　描画
		if (isEnemyAlive) {
			Novice::DrawEllipse(int(enemyPosX), int(enemyPosY), int(enemyRadius), int(enemyRadius), 0.0f, WHITE, kFillModeSolid);
		}
		else if (timer > 45) {
			Novice::DrawSprite(int(enemyPosX), int(enemyPosY), graphHandle, 1, 1, 0.0f, WHITE);
		}




		//運行状況
		Novice::ScreenPrintf(0, 0, "player: posX:  %.1f, posY:  %.1f       degree = %.1f radian = %.1f speed = %.1f  moveX = %.1f moveY = %.1f ", playerPosX, playerPosY, degree, theta, playerSpeed, moveX, moveY);
		Novice::ScreenPrintf(0, 30, "bullet: posX:  %.1f, posY:  %.1f, isBulletShot :  %d", bulletTopX, bulletTopY, isShot);
		Novice::ScreenPrintf(0, 60, "enemyX: posX:  %.1f, enemyY:  %.1f, isEnemyAlive :  %d", enemyPosX, enemyPosY, isEnemyAlive);
		Novice::ScreenPrintf(0, 110, "e2bX^2 : %.1f , e2bY^2 : %.1f , e2bR : %.1f", e2bX * e2bX, e2bY * e2bY, distance);
		Novice::ScreenPrintf(0, 90, "%d", timer);
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

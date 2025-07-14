#include <Novice.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <time.h> 

const char kWindowTitle[] = "GC_1C_02_ゴ_チュウ";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////矩形ここから
	//①player大きさ
	float playerWeight = 50.0f;
	float playerHeight = 50.0f;
	//②local矩形座標（local Position）
	float kLeftTopX = -playerWeight / 2.0f;
	float kLeftTopY = -playerHeight / 2.0f;
	float kRightTopX = playerWeight / 2.0f;
	float kRightTopY = -playerHeight / 2.0f;
	float kLeftBomtomX = -playerWeight / 2.0f;
	float kLeftBomtomY = playerHeight / 2;
	float kRightBomtomX = playerWeight / 2;
	float kRightBomtomY = playerHeight / 2;
	//③回転用theta初期化
	float rotateTheta = 0.0f;
	//④player world Position
	float playerCenterPositionX = 400.0f;
	float playerCenterPositionY = 400.0f;
	//⑤描画用textureハンドル
	int whiteTextureHandle;
	whiteTextureHandle = Novice::LoadTexture("./NoviceResources/white1x1.png");
	//⑥衝突判定用円のradius
	float playerRadius = sqrtf(playerWeight / 2 * playerWeight / 2 + playerHeight / 2 * playerHeight / 2);
	//⑦player移動用
	float playerTheta = 0.0f;
	float playerSpeed = 0.0f;
	float playerMoveX = 0.0f;
	float playerMoveY = 0.0f;
	//⑧ダッシュ移動用
	bool isDash = false;
	int dashTimer = 0;
	float dashSpeed = 0.0f;
	///////////////////////////////////////////////////////////////////////////////////////////playerここまで



	////////////////////////////////////////////////////////////////////////////////////////////enemyここから
	//①enemy大きさ
	float enemyRadius = 20.0f;
	//②seed作成
	unsigned int currentTime = unsigned int(time(nullptr));
	srand(currentTime);
	//③初期の値（乱数で）
	const int enemyCount = 8;	//要素数
	float enemyPositionX[enemyCount] = {};
	float enemyPositionY[enemyCount] = {};
	float enemyMoveX[enemyCount] = {};
	float enemyMoveY[enemyCount] = {};
	float enemySpeed[enemyCount] = {};
	unsigned int color[3] = { WHITE,BLUE,RED };	//色配列
	unsigned int enemyColor[enemyCount] = {};	//描画用ランダム色
	for (int i = 0; i < enemyCount; i++) {
		enemyPositionX[i] = float(rand() % int(1280 - 2 * enemyRadius) + enemyRadius);//画面外超えないように
		enemyPositionY[i] = float(rand() % int(720 - 2 * enemyRadius) + enemyRadius);
		enemySpeed[i] = float(rand() % 3 + 2);
		enemyMoveX[i] = 1;
		enemyMoveY[i] = 1;//0抜き取りがまだわかりませんが、残念。
		enemyColor[i] = color[rand() % 3];
	}

	///////////////////////////////////////////////////////////////////////////////////////////enemyここまで

	////////////////////////////////////////////////////////////////////////////////自然死亡、アタリ処理初期化ここから
	int score = 0;
	float a[enemyCount] = {};
	float b[enemyCount] = {};
	float distance[enemyCount] = {};
	int reviveTime = 120;					//復活の時間			
	int reviveTimer[enemyCount] = {};		//復活タイマー
	int disappearTimer[enemyCount] = {};	//消えるタイマー
	bool isEnemyAlive[enemyCount] = {};		//復活用スイッチ
	for (int i = 0; i < enemyCount; i++) {
		isEnemyAlive[i] = true;
		reviveTimer[i] = reviveTime;
		disappearTimer[i] = rand() % 250 + 50;
	}
	///////////////////////////////////////////////////////////////////////////////自然死亡、アタリ処理初期化ここまで



	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {/////////////////////////                                           START！！！！
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		///player回転処理ここから
		rotateTheta += 0.1f;//フレームごとにrotateThetaを足す（degree 360 == radian 2 * M_PI（约 6.28318)
		//回転した座標（Rotated Position）
		float leftTopRotatedX = kLeftTopX * cosf(rotateTheta) - kLeftTopY * sinf(rotateTheta);
		float leftTopRotatedY = kLeftTopY * cosf(rotateTheta) + kLeftTopX * sinf(rotateTheta);
		float rightTopRotatedX = kRightTopX * cosf(rotateTheta) - kRightTopY * sinf(rotateTheta);
		float rightTopRotatedY = kRightTopY * cosf(rotateTheta) + kRightTopX * sinf(rotateTheta);
		float leftBottomRotatedX = kLeftBomtomX * cosf(rotateTheta) - kLeftBomtomY * sinf(rotateTheta);
		float leftBottomRotatedY = kLeftBomtomY * cosf(rotateTheta) + kLeftBomtomX * sinf(rotateTheta);
		float rightBottomRotatedX = kRightBomtomX * cosf(rotateTheta) - kRightBomtomY * sinf(rotateTheta);
		float rightBottomRotatedY = kRightBomtomY * cosf(rotateTheta) + kRightBomtomX * sinf(rotateTheta);
		//頂点ワールド座標（world Position）
		float LeftTopPosionX = leftTopRotatedX + playerCenterPositionX;
		float LeftTopPosionY = leftTopRotatedY + playerCenterPositionY;
		float RightTopPosionX = rightTopRotatedX + playerCenterPositionX;
		float RightTopPosionY = rightTopRotatedY + playerCenterPositionY;
		float LeftBottomPosionX = leftBottomRotatedX + playerCenterPositionX;
		float LeftBottomPosionY = leftBottomRotatedY + playerCenterPositionY;
		float RightBottomPosionX = rightBottomRotatedX + playerCenterPositionX;
		float RightBottomPosionY = rightBottomRotatedY + playerCenterPositionY;
		///player回転処理ここまで

		///player移動処理ここから
		//普通移動SPEED
		if (keys[DIK_D] || keys[DIK_W] || keys[DIK_A] || keys[DIK_S]) {
			playerSpeed = 5.0f;
		}
		else {
			playerSpeed = 0.0f;
		}

		//ダッシュ移動ここから
		if (keys[DIK_SPACE] && !preKeys[DIK_SPACE] && !isDash) {
			isDash = true;
			dashSpeed = 10.0f;
			dashTimer = 20;	//キーを押してから20フレームダッシュする
		}
		if (isDash) {
			playerCenterPositionX += cosf(playerTheta) * dashSpeed;
			playerCenterPositionY -= sinf(playerTheta) * dashSpeed;
			dashTimer--;
			if (dashTimer <= 0) {
				isDash = false;
			}
		}
		//ダッシュ移動ここまで

		//theta処理
		if (keys[DIK_D]) {
			playerTheta = 0.0f * (float(M_PI) / 180.0f);
		}
		if (keys[DIK_W]) {
			playerTheta = 90.0f * (float(M_PI) / 180.0f);
		}
		if (keys[DIK_A]) {
			playerTheta = 180.0f * (float(M_PI) / 180.0f);
		}
		if (keys[DIK_S]) {
			playerTheta = 270.0f * (float(M_PI) / 180.0f);
		}
		if (keys[DIK_W] && keys[DIK_D]) {
			playerTheta = 45.0f * (float(M_PI) / 180.0f);
		}
		if (keys[DIK_W] && keys[DIK_A]) {
			playerTheta = 135.0f * (float(M_PI) / 180.0f);
		}
		if (keys[DIK_S] && keys[DIK_A]) {
			playerTheta = 225.0f * (float(M_PI) / 180.0f);
		}
		if (keys[DIK_S] && keys[DIK_D]) {
			playerTheta = 315.0f * (float(M_PI) / 180.0f);
		}
		playerMoveX = cosf(playerTheta);
		playerMoveY = sinf(playerTheta);
		//playerワールド座標
		playerCenterPositionX += playerMoveX * playerSpeed;
		playerCenterPositionY -= playerMoveY * playerSpeed;

		//player画面外超えないように処理
		if (playerCenterPositionX >= 1280 - playerRadius) {
			playerCenterPositionX = 1280 - playerRadius;
		}
		if (playerCenterPositionX <= playerRadius) {
			playerCenterPositionX = playerRadius;
		}
		if (playerCenterPositionY >= 720 - playerRadius) {
			playerCenterPositionY = 720 - playerRadius;
		}
		if (playerCenterPositionY <= playerRadius) {
			playerCenterPositionY = playerRadius;
		}


		//enemy移動処理
		for (int i = 0; i < enemyCount; i++)
		{
			enemyPositionX[i] += enemyMoveX[i] * enemySpeed[i];
			enemyPositionY[i] += enemyMoveY[i] * enemySpeed[i];
		}


		//enemy反射処理
		for (int i = 0; i < enemyCount; i++) {
			if (enemyPositionX[i] >= 1280 - enemyRadius) {
				enemyMoveX[i] *= -1;
			}
			if (enemyPositionX[i] <= enemyRadius) {
				enemyMoveX[i] *= -1;
			}
			if (enemyPositionY[i] >= 720 - enemyRadius) {
				enemyMoveY[i] *= -1;
			}
			if (enemyPositionY[i] <= enemyRadius) {
				enemyMoveY[i] *= -1;
			}
		}


		//アタリ処理
		for (int i = 0; i < enemyCount; i++) {
			//①距離求める
			a[i] = enemyPositionX[i] - playerCenterPositionX;
			b[i] = enemyPositionY[i] - playerCenterPositionY;
			distance[i] = static_cast<float>(sqrt(a[i] * a[i] + b[i] * b[i]));

			//描画、得点判定
			if (isEnemyAlive[i] == true) {
				disappearTimer[i]--;

				//自然死亡
				if (disappearTimer[i] == 0) {
					disappearTimer[i] = rand() % 250 + 50;			//死んでいたら自然死亡タイマー戻る
					isEnemyAlive[i] = false;
				}
				//当たる死亡（得点YES）
				else if (distance[i] <= enemyRadius + playerRadius) {	//当たったら死ぬ
					disappearTimer[i] = rand() % 250 + 50;		//死んでいたら自然死亡タイマー戻る
					isEnemyAlive[i] = false;
					switch (enemyColor[i]) {
					case WHITE:
						score += 10;
						break;
					case BLUE:
						score += 100;
						break;
					case RED:
						score += 1000;
						break;
					}
				}
			}

			//死んでいたらすぐ復活する
			//上の部分にelseで繋げられるけど、よく読めるように、一応こう書いておくと思う
			if (isEnemyAlive[i] == false) {
				reviveTimer[i]--;
				if (reviveTimer[i] == 0) {
					reviveTimer[i] = reviveTime;
					isEnemyAlive[i] = true;
					enemySpeed[i] = float(rand() % 3 + 2);
					enemyPositionX[i] = float(rand() % int(1280 - 2 * enemyRadius) + enemyRadius);
					enemyPositionY[i] = float(rand() % int(720 - 2 * enemyRadius) + enemyRadius);
					enemyColor[i] = color[rand() % 3];
				}
			}

		}

		///
		/// ↑更新処理ここまで
		///



		///
		/// ↓描画処理ここから
		///

		//player描画ここから
		Novice::DrawQuad(
			int(LeftTopPosionX), int(LeftTopPosionY),
			int(RightTopPosionX), int(RightTopPosionY),
			int(LeftBottomPosionX), int(LeftBottomPosionY),
			int(RightBottomPosionX), int(RightBottomPosionY),
			0, 0,
			int(playerWeight), int(playerHeight),
			whiteTextureHandle,
			WHITE);

		//enemy描画ここから
		for (int i = 0; i < enemyCount; i++) {
			if (isEnemyAlive[i] == true) {
				Novice::DrawEllipse(int(enemyPositionX[i]), int(enemyPositionY[i]), int(enemyRadius), int(enemyRadius), 0.0f, enemyColor[i]
					, kFillModeSolid);
			}
		}



		//実行情報ここから
		Novice::ScreenPrintf(0, 0, "score:%d	(RED:1000  BULE:100  WHITE:10)", score);
		Novice::ScreenPrintf(0, 30, "playerPosition:%.1f  %.1f", playerCenterPositionX, playerCenterPositionY);
		Novice::ScreenPrintf(0, 60, "playerWeight:%.1f  Height:%.1f  Radius（hitBox）:%.1f", playerWeight, playerHeight, playerRadius);
		Novice::ScreenPrintf(0, 90, "playerSpeed: %.1f  playerMoveX: %.1f  playerMoveY: %.1f ", playerSpeed, playerMoveX, playerMoveY);
		Novice::ScreenPrintf(0, 120, "dashTimer:%d", dashTimer);
		for (int i = 0; i < enemyCount; i++) {
			Novice::ScreenPrintf(500, 10 * i, "enemy[%d]: Position(RAND):%.1f  %.1f   Move:%.1f  %.1f  Color(RAND):%d  isAlive:%d", i, enemyPositionX[i], enemyPositionY[i], enemyMoveX[i], enemyMoveY[i], enemyColor[i], isEnemyAlive[i]);
		}
		for (int i = 0; i < enemyCount; i++) {
			Novice::ScreenPrintf(0, 500 + 10 * i, "enemy[%d]: reviveTimer:%d   disappearTimer(RAND):%d ", i, reviveTimer[i], disappearTimer[i]);
		}

		//情報ここまで

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

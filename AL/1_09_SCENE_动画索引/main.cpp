#include <Novice.h>
#define _USE_MATH_DEFINES
#include <math.h>

const char kWindowTitle[] = "GC_1C_02_ゴ_チュウ";

/*		flamecount
*	　 0   1   2   3
S₀ = { g₀, g₁, g₂, g₃ }
S₁ = { g₄, g₅, g₆, g₇ }
S₂ = { g₈, g₉, g₁₀, g₁₁ }
S₃ = { g₁₂, g₁₃, g₁₄, g₁₅ }
*/

//S₀-3 読みやすくする
enum Direction
{
	FRONT,	//{ g₀, g₁, g₂, g₃ }
	BACK,	//{ g₄, g₅, g₆, g₇ }
	LEFT,	//{ g₈, g₉, g₁₀, g₁₁ }
	RIGHT,	//{ g₁₂, g₁₃, g₁₄, g₁₅ }
};


// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	//player
	int playerPosX = 100;
	int playerPosY = 100;
	int playerSpeed = 10;

	int playerWidth = 128;//当たり判定用，画像の読み込みには使わない
	int playerHeight = 164;



	Direction playerDirection = FRONT;//向き初期化
	int playerAnimTimer = 0;		//timer初期化 

	int graphHandles[16] = {
		Novice::LoadTexture("./images/front0.png"),
		Novice::LoadTexture("./images/front1.png"),
		Novice::LoadTexture("./images/front2.png"),
		Novice::LoadTexture("./images/front3.png"),

		Novice::LoadTexture("./images/back0.png"),
		Novice::LoadTexture("./images/back1.png"),
		Novice::LoadTexture("./images/back2.png"),
		Novice::LoadTexture("./images/back3.png"),

		Novice::LoadTexture("./images/left0.png"),
		Novice::LoadTexture("./images/left1.png"),
		Novice::LoadTexture("./images/left2.png"),
		Novice::LoadTexture("./images/left3.png"),

		Novice::LoadTexture("./images/right0.png"),
		Novice::LoadTexture("./images/right1.png"),
		Novice::LoadTexture("./images/right2.png"),
		Novice::LoadTexture("./images/right3.png"),

	};

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
		if (keys[DIK_W]) {
			playerPosY -= playerSpeed;
			playerDirection = BACK;
		}
		if (keys[DIK_S]) {
			playerPosY += playerSpeed;
			playerDirection = FRONT;
		}
		if (keys[DIK_A]) {
			playerPosX -= playerSpeed;
			playerDirection = LEFT;
		}
		if (keys[DIK_D]) {
			playerPosX += playerSpeed;
			playerDirection = RIGHT;
		}

		//画面外防止
		if (playerPosX > 1280 - playerWidth / 2) {
			playerPosX = 1280 - playerWidth / 2;
		}
		if (playerPosX < playerWidth / 2) {
			playerPosX = playerWidth / 2;
		}
		if (playerPosY > 720 - playerHeight / 2) {
			playerPosY = 720 - playerHeight / 2;
		}
		if (playerPosY < playerHeight / 2) {
			playerPosY = playerHeight / 2;
		}



		
		playerAnimTimer += 2;//再生速度：1フレームごとに 2 増やす
		int animCount = playerAnimTimer / 15;	// int型なので小数点以下は切り捨てられ、countは0～3の値になります

		//1ループの合計は 4 × 15 = 40フレーム
		if (playerAnimTimer >= 60) { playerAnimTimer = 0; }
	


		///
		/// ↑更新処理ここまで
		///





		///
		/// ↓描画処理ここから
		///
		switch (playerDirection)
		{
		case FRONT:
			Novice::ScreenPrintf(0, 0, "FRONT,s0");
			break;
		case BACK:
			Novice::ScreenPrintf(0, 0, "BACK,s1");
			break;
		case RIGHT:
			Novice::ScreenPrintf(0, 0, "RIGHT,s3");
			break;
		case LEFT:
			Novice::ScreenPrintf(0, 0, "LEFT,s2");
			break;
		default:
			break;
		}


		Novice::ScreenPrintf(0, 15, "frameCount:%d", playerAnimTimer);
		Novice::ScreenPrintf(0, 30, "animCount:%d", animCount);


		Novice::DrawBox(	//アタリ判定hitbox
			playerPosX - playerWidth / 2,
			playerPosY - playerHeight / 2,
			playerWidth,
			playerHeight,
			0.0f,
			0x00000000,
			kFillModeSolid
		);

		Novice::DrawSprite(
			playerPosX - playerWidth / 2,
			playerPosY - playerHeight / 2,
			graphHandles[playerDirection * 4 + animCount],//方向*4是找起点，count是那小组的4帧动画
			1,
			1,
			0.0f,
			WHITE
		);

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

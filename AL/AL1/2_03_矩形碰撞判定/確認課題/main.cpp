#include <Novice.h>
#include <math.h>
const char kWindowTitle[] = "GC1C_02_ゴ_チュウ";
typedef struct Vector2
{
	float x;
	float y;
}Vector2;
typedef struct Box
{
	Vector2 pos;
	float width;
	float height;
	Vector2 dir;
	Vector2 vel;
	float speed;
} Box;
// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);
#pragma region 初期化
	Box player{
		.pos = {640.0f,620.0f},
		.width = 100.0f,
		.height = 100.0f,
		.dir = { 0,0 },
		.vel = { 0,0 },
		.speed = 5.0f
	};
	Box target{
		.pos = {640.0f,360.0f},
		.width = 100.0f,
		.height = 100.0f,
		.dir = { 0,0 },
		.vel = { 0,0 },
		.speed = 0.0f
	};
	//bool isHit = false; //当たり判定用
	bool isShowHitBox = false; //当たり判定表示切り替え用
	bool isHitX = false;
	bool isHitY = false;
	bool isHit = false; //当たり判定用

	int graphHandle = Novice::LoadTexture("./Resources/images/Circle.png");
	int graphHandleHit = Novice::LoadTexture("./Resources/images/CircleHit.png");
	int hitBoxHandle = Novice::LoadTexture("./Resources/images/CircleW.png");
	int hitBoxHandleHit = Novice::LoadTexture("./Resources/images/CircleWHit.png");
	int currentHandle = graphHandle; //現在のハンドル
	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

#pragma endregion
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

#pragma region hitBox显示(非正规:切换带有hit图片)
		if (keys[DIK_SPACE] && !preKeys[DIK_SPACE]) {
			isShowHitBox = !isShowHitBox;
		}
#pragma endregion
#pragma region move
		player.dir = { 0,0 };
		if (keys[DIK_A]) {
			player.dir.x--;
		}
		if (keys[DIK_D]) {
			player.dir.x++;
		}
		if (keys[DIK_W]) {
			player.dir.y--;
		}
		if (keys[DIK_S]) {
			player.dir.y++;
		}
		float length = sqrtf(powf(player.dir.x, 2) + powf(player.dir.y, 2));
		if (length != 0.0f) {
			player.dir.x /= length;
			player.dir.y /= length;
		}
		player.vel = { player.dir.x * player.speed, player.dir.y * player.speed };
		player.pos.x += player.vel.x;
		player.pos.y += player.vel.y;
#pragma endregion
#pragma region 边界计算
		float playerLeft = player.pos.x - player.width / 2;
		float playerRight = player.pos.x + player.width / 2;
		float playerTop = player.pos.y - player.height / 2;
		float playerBottom = player.pos.y + player.height / 2;
		float targetLeft = target.pos.x - target.width / 2;
		float targetRight = target.pos.x + target.width / 2;
		float targetTop = target.pos.y - target.height / 2;
		float targetBottom = target.pos.y + target.height / 2;
#pragma endregion
#pragma region 顶点计算
		//player
		Vector2 playerLeftTop = { playerLeft , playerTop };
		Vector2 playerLeftBottom = { playerLeft , playerBottom };
		Vector2 playerRightTop = { playerRight , playerTop };
		Vector2 playerRightBottom = { playerRight , playerBottom };

		//target
		Vector2 targetLeftTop = { targetLeft , targetTop };
		Vector2 targetLeftBottom = { targetLeft , targetBottom };
		Vector2 targetRightTop = { targetRight , targetTop };
		Vector2 targetRightBottom = { targetRight , targetBottom };
#pragma endregion	
#pragma region あたり判定
		if (playerLeft < targetRight && playerRight > targetLeft) {
			isHitX = true;
		}
		else {
			isHitX = false;
		}
		if (playerTop < targetBottom && playerBottom > targetTop) {
			isHitY = true;
		}
		else {
			isHitY = false;
		}

		if (isHitX && isHitY) {
			isHit = true;
		}
		else {
			isHit = false;
		}
#pragma endregion
#pragma region 缩放计算
		float scale = 2.0f;
		if (keys[DIK_Q]) {
			player.width += scale;
		}
		if (keys[DIK_E]) {
			player.width -= scale;
		}
		if (keys[DIK_Z]) {
			player.height += scale;
		}
		if (keys[DIK_C]) {
			player.height -= scale;
		}
#pragma endregion

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

#pragma region 描画ハンドル判定
		if (isShowHitBox)
		{
			if (isHit) {
				currentHandle = hitBoxHandleHit;
			}
			else
			{
				currentHandle = hitBoxHandle;
			}
		}
		else
		{
			if (isHit) {
				currentHandle = graphHandleHit;
			}
			else {
				currentHandle = graphHandle;
			}
		}
#pragma endregion
#pragma region 描画
		Novice::DrawQuad(
			int(playerLeftTop.x), int(playerLeftTop.y),
			int(playerRightTop.x), int(playerRightTop.y),
			int(playerLeftBottom.x), int(playerLeftBottom.y),
			int(playerRightBottom.x), int(playerRightBottom.y),
			0,		//剪裁起点
			0,
			32,		//剪裁范围	
			32,
			currentHandle,
			WHITE
		);
		Novice::DrawQuad(
			int(targetLeftTop.x), int(targetLeftTop.y),
			int(targetRightTop.x), int(targetRightTop.y),
			int(targetLeftBottom.x), int(targetLeftBottom.y),
			int(targetRightBottom.x), int(targetRightBottom.y),
			0,		//剪裁起点
			0,
			32,		//剪裁范围	
			32,
			currentHandle,
			WHITE
		);
#pragma endregion
#pragma region UI
		Novice::DrawLine(640, 0, 640, 720, WHITE);
		Novice::DrawLine(0, 360, 1280, 360, WHITE);
		Novice::ScreenPrintf(10, 10, "WASD:move");
		Novice::ScreenPrintf(10, 30, "Q:width+ E:width-");
		Novice::ScreenPrintf(10, 50, "Z:height+ C:height-");
		Novice::ScreenPrintf(10, 70, "Space:hitBox表示切替");
		// X方向判定
		Novice::ScreenPrintf(900, 40, "isHitX: %d", isHitX);
		Novice::ScreenPrintf(900, 60, "  if(PlayerLeftX(%d) < TargetRightX(%d)) : %d",
			(int)playerLeft, (int)targetRight, (int)(playerLeft < targetRight));
		Novice::ScreenPrintf(900, 80, "  if(TargetLeftX(%d) < PlayerRightX(%d)) : %d",
			(int)targetLeft, (int)playerRight, (int)(targetLeft < playerRight));

		// Y方向判定
		Novice::ScreenPrintf(900, 110, "isHitY: %d", isHitY);
		Novice::ScreenPrintf(900, 130, "  if(PlayerTopY(%d) < TargetBottomY(%d)) : %d",
			(int)playerTop, (int)targetBottom, (int)(playerTop < targetBottom));
		Novice::ScreenPrintf(900, 150, "  if(TargetTopY(%d) < PlayerBottomY(%d)) : %d",
			(int)targetTop, (int)playerBottom, (int)(targetTop < playerBottom));

		// 综合判定
		Novice::ScreenPrintf(900, 180, "isHit: %d", isHit);


#pragma endregion
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

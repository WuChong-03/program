#include <Novice.h>
#include <math.h>

const char kWindowTitle[] = "GC1C_02_ゴ_チュウ";
typedef struct Vector2 {
	float x;
	float y;
} Vector2;

typedef struct Player {
	Vector2 worldPos = { 0, 0 };
	Vector2 dir = { 0, 0 };
	Vector2 vel = { 0, 0 };
	float speed = 10.0f;
	float radius = 16.0f;
	Vector2 screenPos = { 0, 0 };
}Player;

// アプリのエントリーポイント
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	// ライブラリ初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	Player player = {};
	float scrollX = 0.0f;
	float scrollLineX = 800.0f;

	int bgGrHandle[4] = {
		Novice::LoadTexture("./Resources/images/bg1.png"),
		Novice::LoadTexture("./Resources/images/bg2.png"),
		Novice::LoadTexture("./Resources/images/bg3.png"),
		Novice::LoadTexture("./Resources/images/bg4.png")
	};

	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	while (Novice::ProcessMessage() == 0) {
		Novice::BeginFrame();
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);
		///
		///更新处理
		/// 
#pragma region プレイヤー移動処理
		player.dir = { 0, 0 };
		if (keys[DIK_A]) player.dir.x--;
		if (keys[DIK_D]) player.dir.x++;
		float length = sqrtf(player.dir.x * player.dir.x + player.dir.y * player.dir.y);
		if (length != 0.0f) {
			player.dir.x /= length;
			player.dir.y /= length;
		}
		player.vel.x = player.dir.x * player.speed;
		player.vel.y = player.dir.y * player.speed;
		player.worldPos.x += player.vel.x;
		player.worldPos.y += player.vel.y;
#pragma endregion


#pragma region 座標変換
		player.screenPos.x = player.worldPos.x - scrollX;   // ← 只做 WCS→SCS 平移
		player.screenPos.y = -player.worldPos.y + 577;
#pragma endregion


#pragma region スクロール処理
		// 玩家方块右端在 SCS 的坐标
		float playerRightEdge = player.screenPos.x + player.radius * 2;

		// 如果超过红线，背景卷动同样的超出量
		if (playerRightEdge > scrollLineX) {
			float overshoot = playerRightEdge - scrollLineX;   // 超出的距离
			scrollX += overshoot;                              // 背景左移抵消
		}

		// 背景卷轴边界限制
		if (scrollX < 0.0f)        scrollX = 0.0f;
		if (scrollX > 1280 * 3)    scrollX = 1280 * 3;
#pragma endregion


#pragma region line移动
		if (keys[DIK_LEFT] && scrollLineX >= 0 && scrollLineX >= playerRightEdge)
		{
			scrollLineX -= 10.0f;
		}
		if (keys[DIK_RIGHT] && scrollLineX <= 1280)
		{
			scrollLineX += 10.0f;
		}
#pragma endregion



		///
		///描画处理
		/// 
		for (int i = 0; i < 4; i++) {
			Novice::DrawSprite(1280 * i - (int)scrollX, 0, bgGrHandle[i], 1, 1, 0.0f, WHITE);
		}

		// プレイヤー描画
		Novice::DrawBox(
			(int)(player.screenPos.x - player.radius),
			(int)(player.screenPos.y - player.radius),
			(int)(player.radius * 2),
			(int)(player.radius * 2),
			0.0f,
			0x66ffffFF,
			kFillModeSolid
		);

		Novice::DrawLine((int)scrollLineX, 0, (int)scrollLineX, 720, RED);



#pragma region デバッグ表示
		Novice::ScreenPrintf(10, 10, "哈啊哈Player Pos(WCS): %.2f, %.2f", player.worldPos.x, player.worldPos.y);
		Novice::ScreenPrintf(10, 40, "Player Pos(SCS): %.2f, %.2f", player.screenPos.x, player.screenPos.y);
		Novice::ScreenPrintf(10, 100, "scrollLineX(SCS): %.2f", scrollLineX);
		Novice::ScreenPrintf(10, 130, "scrollX: %.2f", scrollX);
		Novice::ScreenPrintf(10, 70, "Player RightEdge: %.2f", playerRightEdge);
#pragma endregion

		Novice::EndFrame();
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) break;
	}

	Novice::Finalize();
	return 0;
}
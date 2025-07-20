#include <Novice.h>
#include <math.h>

const char kWindowTitle[] = "GC1C_02_ゴ_チュウ";

// --- 構造体定義 ---

typedef struct Vector2 {
	float x;
	float y;
} Vector2;

typedef struct Player {
	Vector2 pos;
	Vector2 dir;
	Vector2 vel;
	float radius;
	float speed;
} Player;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};


	// WCSからのSCSの原点の座標、スカラー倍率を初期化：最後の描画処理で座標変換用
	const float scale = 1.0f;
	const float screenOriginX = 0.0f;
	const float screenOriginY = 500.0f;

	// player初期化
	Player player = {
		{0.0f, 0.0f},
		{0.0f,0.0f},
		{0.0f,0.0f},
		30.0f,
		5.0f
	};

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
		player.dir = { 0.0f, 0.0f };
		if (keys[DIK_UP]) {
			player.dir.y++;
		}
		if (keys[DIK_DOWN]) {
			player.dir.y--;
		}
		if (keys[DIK_RIGHT]) {
			player.dir.x++;
		}
		if (keys[DIK_LEFT]) {
			player.dir.x--;
		}
		float length = sqrtf(powf(player.dir.x, 2) + powf(player.dir.y, 2));
		if (length != 0) {
			player.dir.x /= length;
			player.dir.y /= length;
		}
		player.vel.x = player.dir.x * player.speed;
		player.vel.y = player.dir.y * player.speed;
		player.pos.x += player.vel.x;
		player.pos.y += player.vel.y;
		///
		/// ↑更新処理ここまで
		///

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		// ワールド座標からスクリーン座標に変換（TRS）
		int drawX = (int)(player.pos.x * scale + screenOriginX);
		int drawY = (int)(-player.pos.y * scale + screenOriginY);

		// プレイヤーを描画（白の半径30の円）
		Novice::DrawEllipse(drawX, drawY, 30, 30, 0.0f, WHITE, kFillModeSolid);

		// ワールド座標具体的位置をデバッグ表示
		Novice::DrawLine(0, (int)screenOriginY, 1280, (int)screenOriginY, WHITE);

		Novice::ScreenPrintf(10, 10, "Player Position(WCS): (%.2f, %.2f)", player.pos.x, player.pos.y);
		Novice::ScreenPrintf(10, 30, "Player Position(SCS): (%d, %d)", drawX, drawY);

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

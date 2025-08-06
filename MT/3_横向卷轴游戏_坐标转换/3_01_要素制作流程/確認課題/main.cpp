#include <Novice.h>
#include <math.h>

const char kWindowTitle[] = "GC1C_02_ゴ_チュウ";

// --- 構造体定義 ---

typedef struct Vector2 {
	float x;
	float y;
} Vector2;

typedef struct Item {
	Vector2 pos;
	Vector2 dir;
	Vector2 vel;
	float radius;
	float speed;
	bool isActive; // アイテムがアクティブかどうか
} Item;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };


	// WCSからのSCSの原点の座標、スカラー倍率を初期化：最後の描画処理で座標変換用
	const float scale = 10.0f;
	const float screenOriginX = 0.0f;
	const float screenOriginY = 500.0f;

	// item_00初期化
	Item item_00 = {
		{600.0f, 0.0f},
		{1.0f,0.0f},
		{0.0f,0.0f},
		100.0f,
		5.0f,
		false // 初期状態では非アクティブ
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
		if (keys[DIK_SPACE] && !preKeys[DIK_SPACE]) {
			// スペースキーが押されたらアイテムをアクティブにする
			item_00.isActive = true;
		}

		if (item_00.isActive) {
			item_00.vel.x = item_00.dir.x * item_00.speed;
			item_00.pos.x += item_00.vel.x;
			float worldRightEdge = 1280.0f / scale - item_00.radius;
			float worldLeftEdge = 0.0f / scale + item_00.radius;
			if (item_00.pos.x  >= worldRightEdge || item_00.pos.x <= worldLeftEdge){
				item_00.dir.x *= -1.0f; // 反転
			}
		}
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
		int drawX = (int)(item_00.pos.x * scale + screenOriginX);
		int drawY = (int)(-item_00.pos.y * scale + screenOriginY);
		int drawRadius = (int)(item_00.radius * scale);

		// item_00 を描画（白の半径30の円）
		if (item_00.isActive){
			Novice::DrawEllipse(drawX, drawY, drawRadius, drawRadius, 0.0f, WHITE, kFillModeSolid);
		}
	
		// ワールド座標具体的位置をデバッグ表示
		Novice::ScreenPrintf(10, 10, "item_00 Position(WCS): (%.2f, %.2f)", item_00.pos.x, item_00.pos.y);
		Novice::ScreenPrintf(10, 30, "item_00 Position(SCS): (%d, %d)", drawX, drawY);

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

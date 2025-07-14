#include <Novice.h>
#include <math.h>

const char kWindowTitle[] = "MT_02_06_Telesa";

#pragma region 構造体
typedef struct Vector2 {
	float x;
	float y;
}Vector2;

typedef struct Circle {
	Vector2 pos;
	Vector2 dir;
	float radius;
	float speed;
}Circle;
#pragma endregion

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
#pragma region 初期化
	Circle A = { {800.0f, 100.0f}, {0.0f, 0.0f}, 30.0f, 2.0f };
	Circle B = { {300.0f, 100.0f}, {1.0f, 0.0f}, 30.0f, 5.0f };
	float dot = 0.0f;
#pragma endregion

	Novice::Initialize(kWindowTitle, 1280, 720);
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	while (Novice::ProcessMessage() == 0) {
		Novice::BeginFrame();
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

#pragma region B移動
		if (keys[DIK_RIGHT]) {
			B.pos.x += B.speed;
			B.dir = { 1.0f, 0.0f };
		}
		if (keys[DIK_LEFT]) {
			B.pos.x -= B.speed;
			B.dir = { -1.0f, 0.0f };
		}
#pragma endregion

#pragma region Aの向き
		if (A.pos.x > B.pos.x) {
			A.dir = { -1.0f, 0.0f };
		}
		else {
			A.dir = { 1.0f, 0.0f };
		}
#pragma endregion

#pragma region 内積判定
		dot = A.dir.x * B.dir.x + A.dir.y * B.dir.y;
		bool isFacing = (dot < 0.0f);
#pragma endregion

#pragma region 動作
		// 向き合っていなければ A が B に向かって移動
		if (!isFacing) {
			if (B.pos.x > A.pos.x) {
				A.pos.x += A.speed;
			}
			else {
				A.pos.x -= A.speed;
			}
		}
#pragma endregion

#pragma region 描画
		// A
		Novice::DrawEllipse(
			(int)A.pos.x, (int)A.pos.y,
			(int)A.radius, (int)A.radius,
			0.0f, isFacing ? BLUE : WHITE, kFillModeSolid
		);
		// Aの向きを200表示
		Novice::DrawLine(
			(int)A.pos.x, (int)A.pos.y,
			(int)(A.pos.x + A.dir.x * 200),
			(int)(A.pos.y + A.dir.y * 200),
			WHITE
		);
		// B
		Novice::DrawEllipse(
			(int)B.pos.x, (int)B.pos.y,
			(int)B.radius, (int)B.radius,
			0.0f, RED, kFillModeSolid
		);
		// Bの向きを200表示
		Novice::DrawLine(
			(int)B.pos.x, (int)B.pos.y,
			(int)(B.pos.x + B.dir.x * 200),
			(int)(B.pos.y + B.dir.y * 200),
			WHITE
		);
#pragma endregion

		Novice::EndFrame();
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	Novice::Finalize();
	return 0;
}
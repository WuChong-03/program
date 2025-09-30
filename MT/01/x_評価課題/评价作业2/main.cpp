#include <Novice.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <time.h>
const char kWindowTitle[] = "GC_1C_02_ゴ_チュウ";

#pragma region 構造体
typedef struct Vector2 {
	float x;
	float y;
}Vector2;
typedef struct Player {
	Vector2 centerPos;
	Vector2 dir;
	Vector2 vel;
	Vector2 offsetTop;
	Vector2 offsetLeft;
	Vector2 offsetRight;
	Vector2 topPos;
	Vector2 leftPos;
	Vector2 rightPos;

	float speed;
	float radius;
	int dashTimer;
}Player;
typedef struct Enemy {
	Vector2 pos;
	Vector2 dir;
	Vector2 vel;
	float speed;
	float radius;
	bool isAlive;
	int respawnTimer;
}Enemy;
typedef struct Bullet {
	Vector2 pos;
	Vector2 dir;
	Vector2 vel;
	float speed;
	bool isShot;
}Bullet;
#pragma endregion
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
#pragma region 初期化
	Player player{
	.centerPos = {600,400},
	.dir = {0,0},
	.vel = {0,0},
	.speed = 10.0f,
	.radius = 50.0f,
	.dashTimer = 20
	};
	Enemy enemy[3];
	for (int i = 0; i < 3; i++) {
		enemy[i].pos = { 50.0f + i * 50.0f, 100.0f };
		enemy[i].dir = { 0,0 };
		enemy[i].vel = { 0,0 };
		enemy[i].speed = 10.0f;
		enemy[i].radius = 25.0f;
		enemy[i].isAlive = true;
		enemy[i].respawnTimer = 60;
	}
	Bullet bullet{
		.pos = {-128.0f,-128.0f},
		.dir = {0,0},
		.vel = {0,0},
		.speed = 10.0,
		.isShot = false
	};
	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);
	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };
#pragma endregion
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();
#pragma region キー受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);
#pragma endregion
		/// ↓更新処理ここから
#pragma region playerPos
		player.dir = { 0,0 };
		if (keys[DIK_W]) {
			player.dir.y--;
		}
		if (keys[DIK_S]) {
			player.dir.y++;
		}
		if (keys[DIK_A]) {
			player.dir.x--;
		}
		if (keys[DIK_D]) {
			player.dir.x++;
		}
		float length = sqrtf(powf(player.dir.x, 2) + powf(player.dir.y, 2));
		if (length != 0) {
			player.dir.x /= length;
			player.dir.y /= length;
		}
		player.vel = { player.dir.x * player.speed, player.dir.y * player.speed };
		player.centerPos.x += player.vel.x;
		player.centerPos.y += player.vel.y;
#pragma endregion
#pragma region 移動ベクトル
		player.offsetTop = {
			player.radius * cosf(90.0f * float(M_PI) / 180.0f),
			player.radius * sinf(90.0f * float(M_PI) / 180.0f)
		};
		player.offsetLeft ={
			player.radius * cosf(210.0f * float(M_PI) / 180.0f),
			player.radius * sinf(210.0f * float(M_PI) / 180.0f)
		};
		player.offsetRight ={
			player.radius * cosf(330.0f * float(M_PI) / 180.0f),
			player.radius * sinf(330.0f * float(M_PI) / 180.0f) 
		};
#pragma endregion
#pragma region player頂点pos
		player.topPos.x = player.centerPos.x + player.offsetTop.x;
		player.topPos.y = player.centerPos.y + player.offsetTop.y;
		player.leftPos.x = player.centerPos.x + player.offsetLeft.x;
		player.leftPos.y = player.centerPos.y + player.offsetLeft.y;
		player.rightPos.x = player.centerPos.x + player.offsetRight.x;
		player.rightPos.y = player.centerPos.y + player.offsetRight.y;
#pragma endregion
		/// ↑更新処理ここまで


		/// ↓描画処理ここから
		
		
		/// ↑描画処理ここまで

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

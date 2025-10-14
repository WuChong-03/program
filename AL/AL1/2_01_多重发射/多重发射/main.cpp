#include <Novice.h>
#define _USE_MATH_DEFINES
#include <math.h>

const char kWindowTitle[] = "GC_1C_02_ゴ_チュウ";
const int kWindowWidth = 1280;
const int kWindowHeight = 720;
typedef struct Vector2
{
	float x;
	float y;
}Vector2;
typedef struct Bullet
{
	Vector2 pos;
	Vector2 dir;
	Vector2 vel;
	float radius;
	bool isShot;
	bool isAlive;
	float speed;
}Bullet;
typedef struct Player
{
	Vector2 pos;
	Vector2 dir;
	Vector2 vel;
	float radius;
	float speed;
	bool isAlive;
} Player;
typedef struct Enemy
{
	Vector2 pos;
	Vector2 dir;
	Vector2 vel;
	float radius;
	float speed;
	bool isAlive;
} Enemy;
// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, kWindowWidth, kWindowHeight);

	//初期化ここから
#pragma region bullet
	const int kBulletCount = 20;
	const int kShotCoolTime = 5;
	int shotCoolTime = kShotCoolTime;
	Bullet bullet[kBulletCount];
	for (int i = 0; i < kBulletCount; i++)
	{
		bullet[i].pos.x = -128.0f;
		bullet[i].pos.y = -128.0f;
		bullet[i].radius = 10.0f;
		bullet[i].isShot = false;
		bullet[i].speed = 10.0f;
		bullet[i].isAlive = false;
	}
#pragma endregion
#pragma region player
	Player player;
	player.pos = { 600,600 };
	player.speed = 10.0f;
	player.radius = 25.0f;
	player.isAlive = true;
#pragma endregion
#pragma region enemy
	Enemy enemy;
	enemy.pos = { 600.0f ,400.0f };
	enemy.dir = { 1,0 };
	enemy.speed = 10.0f;
	enemy.radius = 25.0f;
	enemy.isAlive = true;
#pragma endregion
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
#pragma region fit screen
		//player
		if (player.pos.x < player.radius) {
			player.pos.x = player.radius;
		}
		if (player.pos.x > kWindowWidth - player.radius) {
			player.pos.x = kWindowWidth - player.radius;
		}
		if (player.pos.y < player.radius) {
			player.pos.y = player.radius;
		}
		if (player.pos.y > kWindowHeight - player.radius) {
			player.pos.y = kWindowHeight - player.radius;
		}
		//enemy
		if (enemy.pos.x < enemy.radius || enemy.pos.x > kWindowWidth - enemy.radius) {
			enemy.dir.x *= -1;
		}
		if (enemy.pos.y < enemy.radius || enemy.pos.y > kWindowHeight - enemy.radius) {
			enemy.dir.y *= -1;
		}

		for (int i = 0; i < kBulletCount; i++)
		{
			if (bullet[i].pos.x < bullet[i].radius ||
				bullet[i].pos.x >= kWindowWidth - bullet[i].radius ||
				bullet[i].pos.y < bullet[i].radius ||
				bullet[i].pos.y >= kWindowHeight - bullet[i].radius) {
				bullet[i].isShot = false;
			}
		}
#pragma endregion
#pragma region player正規化移動
		player.dir = { 0,0 };
		if (keys[DIK_W]) {
			player.dir.y--;
		}
		if (keys[DIK_A]) {
			player.dir.x--;
		}
		if (keys[DIK_S]) {
			player.dir.y++;
		}
		if (keys[DIK_D]) {
			player.dir.x++;
		}
		float length = sqrtf(powf(player.dir.x, 2) + powf(player.dir.y, 2));
		//移動中だったら、正規化移動する
		if (length != 0.0f) {
			player.vel = {
				player.dir.x / length * player.speed,
				player.dir.y / length * player.speed
			};
			player.pos.x += player.vel.x;
			player.pos.y += player.vel.y;
		}
#pragma endregion
#pragma region enemy移動
		enemy.vel = { enemy.dir.x * enemy.speed , enemy.dir.y * enemy.speed };
		enemy.pos.x += enemy.vel.x;
		enemy.pos.y += enemy.vel.y;
#pragma endregion
#pragma region 弾
		if (shotCoolTime > 0)
		{
			shotCoolTime--;
		};

		if (keys[DIK_SPACE] && shotCoolTime == 0) {
			for (int i = 0; i < kBulletCount; i++) {
				//線形探索
				if (!bullet[i].isShot) {
					bullet[i].isShot = true;
					bullet[i].pos = player.pos;
					//dir（相対ベクトル）を求める
					bullet[i].dir.x = enemy.pos.x - player.pos.x;
					bullet[i].dir.y = enemy.pos.y - player.pos.y;
					//正規化
					float bulletLength = sqrtf(powf(bullet[i].dir.x, 2) + powf(bullet[i].dir.y, 2));
					//velを求める
					bullet[i].vel.x = bullet[i].dir.x / bulletLength * bullet[i].speed;
					bullet[i].vel.y = bullet[i].dir.y / bulletLength * bullet[i].speed;
					//cooltime戻す
					shotCoolTime = kShotCoolTime;
					//線形探索終わり
					break;

				}
			}
		}
		for (int i = 0; i < kBulletCount; i++) {
			if (bullet[i].isShot) {
				bullet[i].pos.x += bullet[i].vel.x;
				bullet[i].pos.y += bullet[i].vel.y;
			}
		}
#pragma endregion
		///
		/// ↑更新処理ここまで
		///


		///
		/// ↓描画処理ここから
		///

#pragma region player
		Novice::DrawEllipse(
			int(player.pos.x),
			int(player.pos.y),
			int(player.radius),
			int(player.radius),
			0.0f,
			WHITE,
			kFillModeSolid
		);
#pragma endregion
#pragma region bullet
		//bullet
		for (int i = 0; i < kBulletCount; i++) {
			if (bullet[i].isShot) {
				Novice::DrawTriangle(
					int(bullet[i].pos.x + bullet[i].radius),
					int(bullet[i].pos.y),
					int(bullet[i].pos.x),
					int(bullet[i].pos.y - bullet[i].radius),
					int(bullet[i].pos.x - bullet[i].radius),
					int(bullet[i].pos.y),
					WHITE,
					kFillModeSolid
				);
			};
		};
#pragma endregion
#pragma region enemy
		Novice::DrawEllipse(
			int(enemy.pos.x),
			int(enemy.pos.y),
			int(enemy.radius),
			int(enemy.radius),
			0.0f,
			RED,
			kFillModeSolid
		);
#pragma endregion
#pragma region デバッグ情報
		Novice::ScreenPrintf(0, 30, "BulletShotCoolTime: %d", shotCoolTime);
		for (int i = 0; i < kBulletCount; i++)
		{
			Novice::ScreenPrintf(0, 50 + i * 20, "isBulletShot[%d]:%d", i, bullet[i].isShot);
		}
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
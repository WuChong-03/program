#include <Novice.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <time.h> 

const char kWindowTitle[] = "GC_1C_02_ゴ_チュウ";
const int kWindowWidth = 1280;
const int kWindowHeight = 720;

typedef struct Vector2 {
	float x;
	float y;
} Vector2;
typedef enum {
	TITLE,
	PLAY,
	RESULT,
}SCENE;
typedef	struct BgGr
{
	int grHandle;
	Vector2 position;
	float MoveSpeed;
}BgGr;
enum Direction
{
	RIGHT,	//{ g0, g1, g2, g3, g4}
	LEFT,	//{ g5, g6, g7, g8 , g9}
};
typedef struct HitBox {
	Vector2 pos;
	float radius;
} HitBox;
typedef struct Player {
	//描画
	float width;
	float height;
	Direction dir;
	//movement
	Vector2 pos;
	Vector2 move;
	float speed;
	//アニメ
	int animTimer;
	int animCount;
	int moveHandlesIndex;
	int standHandlesIndex;
	int standFireHandlesIndex;
	int moveFireHandlesIndex;
	bool isMoving;
	bool isFiring;
	int shotCoolTime;
	//判定
	HitBox hitbox;
} Player;
//敌人现在是靶子，后期做别的的时候再加上和player类似的类型
typedef struct Enemy {
	//movement
	Vector2 pos;
	Vector2 explosionPos;
	//描画
	float width;
	float height;
	//判定
	HitBox hitbox;
	bool isHit;	//受击动画
	int hitTimer; //受击动画timer
	int hitCount; // 被击中次数
	bool isAlive;	//普通动画，判复活定用
	int respawnTimer;
}Enemy;
typedef struct Bullet {
	//描画
	float width;
	float height;
	Direction dir;
	//movement
	Vector2 pos;
	float speed;
	//判定
	HitBox hitbox;
	bool isShoot;
} Bullet;


// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, kWindowWidth, kWindowHeight);

	//初期化ここから、
	bool resultBgmPlayed = false;
	bool isGameStarted = false; // space是否按下
	int score = 0;              // 得分
	int timer = 60 * 60;        // 60秒（60FPS）
#pragma region seed初期化
	unsigned int currentTime = unsigned int(time(nullptr));
	srand(currentTime);
#pragma endregion
#pragma region audio,scene,bg初期化
	int titleBgmHandle = Novice::LoadAudio("./audio/start.wav");
	int playBgmHandle = Novice::LoadAudio("./audio/play.wav");
	int resultBgmHandle = Novice::LoadAudio("./audio/result.wav");
	int titlePlayHandle = -1;
	int playPlayHandle = -1;
	int resultPlayHandle = -1;
	BgGr titleBggr = {
Novice::LoadTexture("./images/scene/title.png"),
{0,0},
10.0f,
	};
	BgGr playBggr = {
	Novice::LoadTexture("./images/scene/play.png"),
	{0,0},
	0.0f,
	};
	BgGr resultBggr = {
	Novice::LoadTexture("./images/scene/result.png"),
	{0,0},
	0.0f,
	};
	SCENE currentScene = TITLE;
#pragma endregion
#pragma region player初期化
	Player player;
	//基本
	player.pos = { 600.0f,400.0f };
	player.width = 64.0f;
	player.height = 84.0f;
	player.shotCoolTime = 5;
	//move
	player.speed = 5.0f;
	player.move.x = 0.0f;
	player.move.y = 0.0f;
	//hitbox
	player.hitbox.pos.x = 0.0f;
	player.hitbox.pos.y = 0.0f;
	player.hitbox.radius = 0.0f;

	//anim
	player.dir = RIGHT;
	player.animTimer = 60;
	player.animCount = 0;
	player.moveHandlesIndex = 0;
	player.standHandlesIndex = 0;
	player.standFireHandlesIndex = 0;
	player.moveFireHandlesIndex = 0;
	player.isMoving = false;
	player.isFiring = false;
#pragma endregion
#pragma region bullet初期化
	const int bulletCount = 20;
	Bullet bullet[bulletCount];
	for (int i = 0; i < bulletCount; i++) {
		bullet[i].pos.x = -128.0f;		//一开始在画面外
		bullet[i].pos.y = -128.0f;
		bullet[i].width = 21.0f;
		bullet[i].height = 11.0f;
		bullet[i].speed = 30.0f;
		bullet[i].isShoot = false;
		bullet[i].dir = LEFT;
		//hitbox
		bullet[i].hitbox.pos.x = 0.0f;
		bullet[i].hitbox.pos.y = 0.0f;
		bullet[i].hitbox.radius = 0.0f;
	}
#pragma endregion
#pragma region enemy初期化
	const int enemyCount = 6;
	int enemyRespawnTime = 90;
	Enemy enemy[enemyCount];
	for (int i = 0; i < enemyCount; i++)
	{
		enemy[i].pos.x = 800.0f;
		enemy[i].pos.y = 360.0f + i * 70.0f;
		enemy[i].width = 92.0f;
		enemy[i].height = 136.0f;
		enemy[i].pos.x = float(rand() % int(kWindowWidth - enemy[i].width) + enemy[i].width / 2);
		enemy[i].pos.y = float(rand() % int(kWindowHeight - enemy[i].height) + enemy[i].height / 2);
		enemy[i].hitbox.pos.x = 0.0f;
		enemy[i].hitbox.pos.y = 0.0f;
		enemy[i].hitbox.radius = 0.0f;
		enemy[i].isHit = false;
		enemy[i].isAlive = true;
		enemy[i].respawnTimer = enemyRespawnTime;
		enemy[i].hitCount = 0;
		enemy[i].hitTimer = 0;
		enemy[i].explosionPos = enemy[i].pos; // 初始化设为当前位置
	}
#pragma endregion
#pragma region 画像load
	int playerStandHandles[10] = {
		Novice::LoadTexture("./images/player/idle/idleRight0.PNG"),
		Novice::LoadTexture("./images/player/idle/idleRight1.PNG"),
		Novice::LoadTexture("./images/player/idle/idleRight2.PNG"),
		Novice::LoadTexture("./images/player/idle/idleRight3.PNG"),
		Novice::LoadTexture("./images/player/idle/idleRight4.PNG"),
		Novice::LoadTexture("./images/player/idle/idleLeft0.PNG"),
		Novice::LoadTexture("./images/player/idle/idleLeft1.PNG"),
		Novice::LoadTexture("./images/player/idle/idleLeft2.PNG"),
		Novice::LoadTexture("./images/player/idle/idleLeft3.PNG"),
		Novice::LoadTexture("./images/player/idle/idleLeft4.PNG"),
	};
	int playerMoveHandles[10] = {
	Novice::LoadTexture("./images/player/move/moveRight0.PNG"),
	Novice::LoadTexture("./images/player/move/moveRight1.PNG"),
	Novice::LoadTexture("./images/player/move/moveRight2.PNG"),
	Novice::LoadTexture("./images/player/move/moveRight3.PNG"),
	Novice::LoadTexture("./images/player/move/moveRight4.PNG"),
	Novice::LoadTexture("./images/player/move/moveLeft0.PNG"),
	Novice::LoadTexture("./images/player/move/moveLeft1.PNG"),
	Novice::LoadTexture("./images/player/move/moveLeft2.PNG"),
	Novice::LoadTexture("./images/player/move/moveLeft3.PNG"),
	Novice::LoadTexture("./images/player/move/moveLeft4.PNG"),
	};
	int playerStandFireHandles[10] = {
		Novice::LoadTexture("./images/player/standfire/fireRight0.PNG"),
		Novice::LoadTexture("./images/player/standfire/fireRight1.PNG"),
		Novice::LoadTexture("./images/player/standfire/fireRight2.PNG"),
		Novice::LoadTexture("./images/player/standfire/fireRight3.PNG"),
		Novice::LoadTexture("./images/player/standfire/fireRight4.PNG"),
		Novice::LoadTexture("./images/player/standfire/fireLeft0.PNG"),
		Novice::LoadTexture("./images/player/standfire/fireLeft1.PNG"),
		Novice::LoadTexture("./images/player/standfire/fireLeft2.PNG"),
		Novice::LoadTexture("./images/player/standfire/fireLeft3.PNG"),
		Novice::LoadTexture("./images/player/standfire/fireLeft4.PNG"),
	};
	int playerMoveFireHandles[10] = {
	Novice::LoadTexture("./images/player/movefire/fireRight0.PNG"),
	Novice::LoadTexture("./images/player/movefire/fireRight1.PNG"),
	Novice::LoadTexture("./images/player/movefire/fireRight2.PNG"),
	Novice::LoadTexture("./images/player/movefire/fireRight3.PNG"),
	Novice::LoadTexture("./images/player/movefire/fireRight4.PNG"),
	Novice::LoadTexture("./images/player/movefire/fireLeft0.PNG"),
	Novice::LoadTexture("./images/player/movefire/fireLeft1.PNG"),
	Novice::LoadTexture("./images/player/movefire/fireLeft2.PNG"),
	Novice::LoadTexture("./images/player/movefire/fireLeft3.PNG"),
	Novice::LoadTexture("./images/player/movefire/fireLeft4.PNG"),
	};
	int bulletHandles = Novice::LoadTexture("./images/player/ather/bullet.png");
	int enemyHandles = Novice::LoadTexture("./images/enemy/enemy.png");
	int hitEnemyHandles = Novice::LoadTexture("./images/enemy/hitEnemy.png");
	int explosionHandles = Novice::LoadTexture("./images/explosion.png");
	//我认识到节约资源很重要，人物，肢体，枪，特效，分开管理，我估计我一个游戏的素材都和10个相同游戏一样大了
		//不过如果是简单的程序，相同的画像数，还能套用index，这里这样做下来也比较容易。
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
		switch (currentScene)
		{
			case TITLE: {
			#pragma region audio,scene更新処理
			if (!Novice::IsPlayingAudio(titlePlayHandle)) {
				titlePlayHandle = Novice::PlayAudio(titleBgmHandle, false, 0.5f);
			}
			if (keys[DIK_RETURN] && !preKeys[DIK_RETURN]) {
				currentScene = PLAY;
				Novice::StopAudio(titlePlayHandle);
			}
#pragma endregion
			break;
			}
			case PLAY: {
			#pragma region audio, scene処理
			if (!Novice::IsPlayingAudio(playPlayHandle)) {
				playPlayHandle = Novice::PlayAudio(playBgmHandle, false, 0.5f);
			}
			if (keys[DIK_RETURN] && !preKeys[DIK_RETURN]) {
				currentScene = RESULT;
				Novice::StopAudio(playPlayHandle);
			}
#pragma endregion	
			#pragma region player処理
#pragma region player正规化移動処理
			Vector2 inputMove = { 0.0f,0.0f };
			if (keys[DIK_D]) {
				player.dir = RIGHT;
				inputMove.x = 1.0f;
			}
			if (keys[DIK_A]) {
				player.dir = LEFT;
				inputMove.x = -1.0f;
			}
			if (keys[DIK_W]) {
				inputMove.y = -1.0f;
			}
			if (keys[DIK_S]) {
				inputMove.y = 1.0f;
			}
			//正规化
			float length = sqrtf(inputMove.x * inputMove.x + inputMove.y * inputMove.y);
			player.move.x = inputMove.x;
			player.move.y = inputMove.y;
			if (length != 0.0f) {
				player.move.x = player.move.x / length;
				player.move.y = player.move.y / length;
			}
			player.pos.x += player.move.x * player.speed;
			player.pos.y += player.move.y * player.speed;
#pragma endregion	
#pragma region 画面外防止
			//画面外防止
			if (player.pos.x > 1280 - player.width / 2) {
				player.pos.x = 1280 - player.width / 2;
			}
			if (player.pos.x < player.width / 2) {
				player.pos.x = player.width / 2;
			}
			if (player.pos.y > 720 - player.height / 2) {
				player.pos.y = 720 - player.height / 2;
			}
			if (player.pos.y < player.height / 2) {
				player.pos.y = player.height / 2;
			}

#pragma endregion	
#pragma region playerAnimation index計算
			//播放timer
			player.animTimer += 2;
			if (player.animTimer >= 60) {
				player.animTimer = 0;
			}
			player.animCount = player.animTimer / 12;			//60 / 5 得 0 - 5 整数

			if (length != 0)
			{
				//移动动画开关
				player.isMoving = true;
				player.moveHandlesIndex = player.dir * 5 + player.animCount;
				//射击动画开关
				if (keys[DIK_J] && isGameStarted) {
					player.isFiring = true;
					player.moveFireHandlesIndex = player.dir * 5 + player.animCount;
				}
				else {
					player.isFiring = false;
				}
			}
			else
			{
				player.isMoving = false;
				player.standHandlesIndex = player.dir * 5 + player.animCount;
				if (keys[DIK_J] && isGameStarted) {
					player.isFiring = true;
					player.standFireHandlesIndex = player.dir * 5 + player.animCount;
				}
				else {
					player.isFiring = false;
				}
			}




			//player.moveHandlesIndex = player.dir * 5 + player.animCount;

#pragma endregion
#pragma endregion	
			#pragma region 开始游戏
			if (!isGameStarted) {
				if (keys[DIK_SPACE] && !preKeys[DIK_SPACE]) {
					isGameStarted = true;
				}
			}
#pragma endregion
				if (isGameStarted){
					#pragma region bullet处理
				//发射间隔处理
				if (player.shotCoolTime > 0) {
					player.shotCoolTime--;
				}
				//发射开关控制
				if (keys[DIK_J] && player.shotCoolTime == 0) {	//发射间隔没有--到0，按键也不会发射
					for (int i = 0; i < bulletCount; i++)
					{
						//for循环依次检查0号到29号子弹的发射状态，直到拿出一颗状态为未发射的子弹发射
						if (!bullet[i].isShoot) {
							//初始化此子弹位置,方向(距离中心点调☞枪口位置)
							bullet[i].pos.x = player.pos.x + 20;
							bullet[i].pos.y = player.pos.y + 4;
							bullet[i].dir = player.dir;
							bullet[i].isShoot = true;
							//初始化发射间隔
							player.shotCoolTime = 5;
							//检测到可用子弹退出循环检测
							break;
						}
					}
				}
				//实时方向发射处理
				for (int i = 0; i < bulletCount; i++) {
					if (bullet[i].isShoot) {
						if (bullet[i].dir == LEFT) {
							bullet[i].pos.x -= bullet[i].speed;
						}
						else if (bullet[i].dir == RIGHT) {
							bullet[i].pos.x += bullet[i].speed;
						}
					}
					if (bullet[i].pos.x <= bullet[i].width / 2 || bullet[i].pos.x >= 1280 - bullet[i].width / 2) {
						bullet[i].isShoot = false;
					}
				}



#pragma endregion
					#pragma region hitbox库
				//player Hitbox
				player.hitbox.pos.x = player.pos.x;
				player.hitbox.pos.y = player.pos.y;
				player.hitbox.radius = player.width / 2;
				//bullet Hitbox
				for (int i = 0; i < bulletCount; i++)
				{
					bullet[i].hitbox.pos.x = bullet[i].pos.x;
					bullet[i].hitbox.pos.y = bullet[i].pos.y;
					bullet[i].hitbox.radius = bullet[i].width / 2;
				}
				//enemy Hitbox
				for (int i = 0; i < enemyCount; i++)
				{
					enemy[i].hitbox.pos.x = enemy[i].pos.x - 7.0F;
					enemy[i].hitbox.pos.y = enemy[i].pos.y - 26.0f;
					enemy[i].hitbox.radius = enemy[i].width / 2 - 8.0f;
				}

#pragma endregion
					#pragma region アタリ判定
				for (int i = 0; i < enemyCount; i++) {
					if (enemy[i].isAlive) {
						if (enemy[i].hitTimer > 0) {
							enemy[i].hitTimer--;  // 每帧递减
						}
					}
				}
				// 依次检查敌人
				for (int i = 0; i < enemyCount; i++) {
					if (enemy[i].isAlive) {
						// 依次检查子弹
						for (int j = 0; j < bulletCount; j++) {
							if (!bullet[j].isShoot) continue;
							float dx = enemy[i].hitbox.pos.x - bullet[j].hitbox.pos.x;
							float dy = enemy[i].hitbox.pos.y - bullet[j].hitbox.pos.y;
							float distance = sqrtf(dx * dx + dy * dy);
							if (distance <= enemy[i].hitbox.radius + bullet[j].hitbox.radius) {
								enemy[i].hitTimer = 15; // 受击后闪红15帧
								enemy[i].hitCount++;
								bullet[j].isShoot = false;
								bullet[j].pos.x = -128.0f;
								bullet[j].pos.y = -128.0f;

								if (enemy[i].hitCount >= 5) {
									enemy[i].isAlive = false;
									enemy[i].hitCount = 0;
									// 死亡时强制结束闪红
									enemy[i].hitTimer = 0;
									// 记录当前位置作为爆炸图像起点
									enemy[i].explosionPos = enemy[i].pos;
									score += 100;
								}
							}
						}
					}
					else {
						// 敌人死亡后复活计时
						enemy[i].respawnTimer--;
						if (enemy[i].respawnTimer <= 0) {
							enemy[i].isAlive = true;
							enemy[i].respawnTimer = enemyRespawnTime;
							enemy[i].hitCount = 0;
							enemy[i].pos.x = float(rand() % int(kWindowWidth - enemy[i].width) + enemy[i].width / 2);
							enemy[i].pos.y = float(rand() % int(kWindowHeight - enemy[i].height) + enemy[i].height / 2);
						}
					}
				}
#pragma endregion
					#pragma region 结束游戏
				if (isGameStarted) {
					timer--;
					if (timer <= 0) {
						currentScene = RESULT;
						Novice::StopAudio(playPlayHandle);
					}
				}
#pragma endregion
				}
			break;
			}
			case RESULT: {
			#pragma region audio,scene更新処理
				if (!resultBgmPlayed) {
					resultPlayHandle = Novice::PlayAudio(resultBgmHandle, false, 0.3f);
					resultBgmPlayed = true;
				}
			if (keys[DIK_RETURN] && !preKeys[DIK_RETURN]) {
				currentScene = TITLE;
				Novice::StopAudio(resultPlayHandle);
				resultBgmPlayed = false; // 重置结束音
				score = 0;
				timer = 60 * 60;
				isGameStarted = false;
				// 重置处理
				for (int i = 0; i < enemyCount; i++) {
					enemy[i].isAlive = true;
					enemy[i].hitCount = 0;
					enemy[i].respawnTimer = enemyRespawnTime;
					enemy[i].hitTimer = 0;
					enemy[i].isHit = false;
					enemy[i].pos.x = float(rand() % int(kWindowWidth - enemy[i].width) + enemy[i].width / 2);
					enemy[i].pos.y = float(rand() % int(kWindowHeight - enemy[i].height) + enemy[i].height / 2);
					enemy[i].explosionPos = enemy[i].pos;
				}
				for (int i = 0; i < bulletCount; i++) {
					bullet[i].isShoot = false;
					bullet[i].pos.x = -128.0f;
					bullet[i].pos.y = -128.0f;
				}
				player.pos = { 600.0f, 400.0f };
				player.shotCoolTime = 5;
			}

#pragma endregion
			break;
			}
		}
		///
		/// ↑更新処理ここまで
		///


		///
		/// ↓描画処理ここから
		///
		switch (currentScene) {
		case TITLE:
#pragma region scene描画処理
			Novice::DrawSprite(
				int(titleBggr.position.x),
				int(titleBggr.position.y),
				titleBggr.grHandle,
				1,
				1,
				0.0f,
				WHITE);
			Novice::ScreenPrintf(500, 500, "Press ENTER to start the game");
#pragma endregion
			break;
		case PLAY:
#pragma region scene描画処理

			Novice::DrawSprite(
				int(playBggr.position.x),
				int(playBggr.position.y),
				playBggr.grHandle,
				1,
				1,
				0.0f,
				WHITE);
#pragma endregion
#pragma region 信息
			if (!isGameStarted){
				Novice::ScreenPrintf(540, 660, "Press SPACE to start!");
			}
			
			Novice::ScreenPrintf(20, 20, "Practice Field");
			Novice::ScreenPrintf(20, 40, "WASD: Move");
			Novice::ScreenPrintf(20, 60, "J: Shoot");
			Novice::ScreenPrintf(20, 80, "ENTER: Finish");
			// 得分显示（右上角）
			Novice::ScreenPrintf(1080, 20, "Score: %d", score);
			// 时间显示（右上角下面）
			Novice::ScreenPrintf(1080, 40, "Time: %d", timer / 60);
#pragma endregion
#pragma region enemy描画処理
			if (isGameStarted)
			{
				for (int i = 0; i < enemyCount; i++) {
					int tex = enemyHandles; // 默认贴图

					// 死亡状态：显示爆炸图像并跳过普通绘制
					if (!enemy[i].isAlive) {
						int floatOffset = (enemyRespawnTime - enemy[i].respawnTimer);
						Novice::DrawSprite(
							//得分画像移动
							int(enemy[i].explosionPos.x),
							int(enemy[i].explosionPos.y - floatOffset),
							explosionHandles,
							1, 1,
							0.0f,
							WHITE
						);
						continue; // 不继续绘制其他贴图
					}

					// 如果处于受击状态，替换贴图
					if (enemy[i].hitTimer > 0) {
						tex = hitEnemyHandles;
					}

					// 绘制敌人贴图（普通 or 受击）
					Novice::DrawSprite(
						int(enemy[i].pos.x - enemy[i].width / 2),
						int(enemy[i].pos.y - enemy[i].height / 2),
						tex,
						1, 1,
						0.0f,
						WHITE);
					// 敌人HitBox绘制
					Novice::DrawEllipse(
						int(enemy[i].hitbox.pos.x), int(enemy[i].hitbox.pos.y),
						int(enemy[i].hitbox.radius), int(enemy[i].hitbox.radius),
						0.0f, 0x00000000, kFillModeWireFrame);
				}
			}

#pragma endregion
#pragma region bullet描画処理
			for (int i = 0; i < bulletCount; i++) {
				if (bullet[i].isShoot) {
					//sprite
					Novice::DrawSprite(
						int(bullet[i].pos.x - bullet[i].width / 2),
						int(bullet[i].pos.y - bullet[i].height / 2),
						bulletHandles, 1, 1,
						0.0f,
						WHITE);
					//hitbox
					Novice::DrawEllipse(
						int(bullet[i].hitbox.pos.x), int(bullet[i].hitbox.pos.y),
						int(bullet[i].hitbox.radius), int(bullet[i].hitbox.radius),
						0.0f, 0x00000000, kFillModeWireFrame
					);
				}
			}
#pragma endregion
#pragma region player描画処理

			//sprite
			if (player.isFiring) {
				if (player.isMoving) {
					Novice::DrawSprite(
						int(player.pos.x - player.width / 2),
						int(player.pos.y - player.height / 2),
						playerMoveFireHandles[player.moveFireHandlesIndex],
						1, 1,
						0.0f,
						WHITE);
				}
				else {
					Novice::DrawSprite(
						int(player.pos.x - player.width / 2),
						int(player.pos.y - player.height / 2),
						playerStandFireHandles[player.standFireHandlesIndex],
						1, 1,
						0.0f,
						WHITE);
				}
			}
			else {
				if (player.isMoving) {
					Novice::DrawSprite(
						int(player.pos.x - player.width / 2),
						int(player.pos.y - player.height / 2),
						playerMoveHandles[player.moveHandlesIndex],
						1, 1,
						0.0f,
						WHITE);
				}
				else {
					Novice::DrawSprite(
						int(player.pos.x - player.width / 2),
						int(player.pos.y - player.height / 2),
						playerStandHandles[player.standHandlesIndex],
						1, 1,
						0.0f,
						WHITE);
				}
			}
			//hitbox
			Novice::DrawEllipse(
				int(player.hitbox.pos.x), int(player.hitbox.pos.y),
				int(player.hitbox.radius), int(player.hitbox.radius),
				0.0f, 0x00000000, kFillModeWireFrame
			);
#pragma endregion
			break;
		case RESULT:
#pragma region scene描画処理
			Novice::DrawSprite(
				int(resultBggr.position.x),
				int(resultBggr.position.y),
				resultBggr.grHandle,
				1,
				1,
				0.0f,
				WHITE);
#pragma endregion
#pragma region 信息
			Novice::ScreenPrintf(540, 600, "Your Score: %d", score);
			Novice::ScreenPrintf(540, 640, "Press ENTER to return to title");
#pragma endregion
			break;
		}

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

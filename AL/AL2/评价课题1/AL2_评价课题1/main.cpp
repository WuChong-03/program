#include <Novice.h>

//----------------------------------
// 定数定义
//----------------------------------
const char kWindowTitle[] = "GC1B_03_ゴ_チュウ";

const int TILE_SIZE = 32;     // 每个方块大小(px)
const int MAP_WIDTH = 50;     // 地图横向格数
const int MAP_HEIGHT = 25;    // 地图纵向格数
const int WIN_WIDTH = 800;    // 窗口宽度
const int WIN_HEIGHT = 800;   // 窗口高度

//----------------------------------
// 地图数据（1=墙, 0=路）
//----------------------------------
int map[MAP_HEIGHT][MAP_WIDTH] = {
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
{1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
{1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
{1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
{1,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
{1,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
{1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
{1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
{1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,1},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
{1,0,0,0,1,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1},
{1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1},
{1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1},
{1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1},
{1,0,0,0,1,1,1,1,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,1,1,1,1,1,1,0,0,1},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
{1,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,1,1,1,1,1,1,0,0,1},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

//----------------------------------
// 主程序
//----------------------------------
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	Novice::Initialize(kWindowTitle, WIN_WIDTH, WIN_HEIGHT);

	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	float playerX = 1 * TILE_SIZE;
	float playerY = 1 * TILE_SIZE;
	float speed = 4.0f;

	float cameraX = 0;
	float cameraY = 0;

	while (Novice::ProcessMessage() == 0) {
		Novice::BeginFrame();

		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		// 移动输入
		float moveX = 0;
		float moveY = 0;
		if (keys[DIK_W]) moveY -= speed;
		if (keys[DIK_S]) moveY += speed;
		if (keys[DIK_A]) moveX -= speed;
		if (keys[DIK_D]) moveX += speed;

		// 预测位置
		float nextX = playerX + moveX;
		float nextY = playerY + moveY;

		int gridLeft = int(nextX / TILE_SIZE);
		int gridRight = int((nextX + TILE_SIZE - 1) / TILE_SIZE);
		int gridTop = int(nextY / TILE_SIZE);
		int gridBottom = int((nextY + TILE_SIZE - 1) / TILE_SIZE);

		bool canMove = true;
		if (gridTop >= 0 && gridBottom < MAP_HEIGHT &&
			gridLeft >= 0 && gridRight < MAP_WIDTH) {
			for (int y = gridTop; y <= gridBottom; y++) {
				for (int x = gridLeft; x <= gridRight; x++) {
					if (map[y][x] == 1) canMove = false;
				}
			}
		}
		else {
			canMove = false;
		}

		if (canMove) {
			playerX = nextX;
			playerY = nextY;
		}

		// 摄像机跟随
		cameraX = playerX - WIN_WIDTH / 2 + TILE_SIZE / 2;
		cameraY = playerY - WIN_HEIGHT / 2 + TILE_SIZE / 2;

		// 边缘保护
		int mapPixelWidth = MAP_WIDTH * TILE_SIZE;
		int mapPixelHeight = MAP_HEIGHT * TILE_SIZE;
		if (cameraX < 0) cameraX = 0;
		if (cameraY < 0) cameraY = 0;
		if (cameraX > mapPixelWidth - WIN_WIDTH) cameraX = (float)(mapPixelWidth - WIN_WIDTH);
		if (cameraY > mapPixelHeight - WIN_HEIGHT) cameraY = (float)(mapPixelHeight - WIN_HEIGHT);

		//----------------------------------
		// 绘制
		//----------------------------------
		for (int y = 0; y < MAP_HEIGHT; y++) {
			for (int x = 0; x < MAP_WIDTH; x++) {
				int tile = map[y][x];
				int drawX = x * TILE_SIZE - (int)cameraX;
				int drawY = y * TILE_SIZE - (int)cameraY;

				if (drawX + TILE_SIZE >= 0 && drawX < WIN_WIDTH &&
					drawY + TILE_SIZE >= 0 && drawY < WIN_HEIGHT) {

					if (tile == 1) {
						// 黑色墙壁
						Novice::DrawBox(drawX, drawY, TILE_SIZE, TILE_SIZE, 0.0f, 0x000000FF, kFillModeSolid);
					}
					else {
						// 白色道路
						Novice::DrawBox(drawX, drawY, TILE_SIZE, TILE_SIZE, 0.0f, 0xFFFFFFFF, kFillModeSolid);
					}
				}
			}
		}

		// 玩家（红色）
		int playerDrawX = (int)(playerX - cameraX);
		int playerDrawY = (int)(playerY - cameraY);
		Novice::DrawBox(playerDrawX, playerDrawY, TILE_SIZE, TILE_SIZE, 0.0f, 0xFF0000FF, kFillModeSolid);

		Novice::EndFrame();
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) break;
	}

	Novice::Finalize();
	return 0;
}

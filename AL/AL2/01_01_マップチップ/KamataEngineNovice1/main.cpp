#include <Novice.h>

const char kWindowTitle[] = "GC1B_03_ゴ_チュウ";

const int TILE_SIZE = 32;
const int MAP_WIDTH = 20;
const int MAP_HEIGHT = 10;

int map[MAP_HEIGHT][MAP_WIDTH] = {
	{0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,1,0,1,1,0,1,0,0,0,1,0,0,0,0,0,0,0,0},
	{0,0,1,0,0,1,0,1,0,0,1,0,0,1,0,0,0,0,0,0},
	{0,0,1,1,1,1,0,1,0,0,1,0,0,1,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,1,0,0,0,1,0,0,1,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0},
	{0,0,1,1,1,1,1,1,0,1,0,0,1,0,0,0,0,0,0,0},
	{0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0},
	{0,0,1,0,1,1,1,1,0,0,1,0,0,0,0,0,0,0,0,0},
	{0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0}
};

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	int windowWidth = MAP_WIDTH * TILE_SIZE;
	int windowHeight = MAP_HEIGHT * TILE_SIZE + 300;
	Novice::Initialize(kWindowTitle, windowWidth, windowHeight);

	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	int blockTexture = Novice::LoadTexture("./block.png");

	float playerWorldX = 1.5f;
	float playerWorldY = 1.5f;
	float speed = 5.0f;

	while (Novice::ProcessMessage() == 0) {
		Novice::BeginFrame();

		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		/// 更新処理ここから
		
// 上
		if (keys[DIK_W] && !preKeys[DIK_W]) {
			//防止出现穿墙()
			int nextY = int(playerWorldY) - 1;
			if (nextY >= 0 && map[nextY][int(playerWorldX)] == 0) {
				playerWorldY-= speed;
			}
		}

		// 下
		if (keys[DIK_S] && !preKeys[DIK_S]) {
			int nextY = int(playerWorldY) + 1;
			if (nextY < MAP_HEIGHT && map[nextY][int(playerWorldX)] == 0) {
				playerWorldY+= speed;
			}
		}

		// 左
		if (keys[DIK_A] && !preKeys[DIK_A]) {
			int nextX = int(playerWorldX) - 1;
			if (nextX >= 0 && map[int(playerWorldY)][nextX] == 0) {
				playerWorldX-=speed;
			}
		}

		// 右
		if (keys[DIK_D] && !preKeys[DIK_D]) {
			int nextX = int(playerWorldX) + 1;
			if (nextX < MAP_WIDTH && map[int(playerWorldY)][nextX] == 0) {
				playerWorldX+= speed;
			}
		}

		/// 更新処理ここまで

		/// 描画処理ここから
		for (int y = 0; y < MAP_HEIGHT; y++) {
			for (int x = 0; x < MAP_WIDTH; x++) {
				int drawX = x * TILE_SIZE;
				int drawY = y * TILE_SIZE;

				if (map[y][x] == 1) {
					Novice::DrawSprite(drawX, drawY, blockTexture, 1.0f, 1.0f, 0.0f, 0xFFFFFFFF);
				}
				else {
					Novice::DrawBox(drawX, drawY, TILE_SIZE, TILE_SIZE, 0.0f, 0x000000FF, kFillModeSolid);
				}
				//信息
				
			}
		}

		// 玩家绘制（红色方块）
		int playerDrawX = int(playerWorldX * TILE_SIZE);
		int playerDrawY = int(playerWorldY * TILE_SIZE);
		Novice::DrawEllipse(playerDrawX, playerDrawY, TILE_SIZE/2, TILE_SIZE/2, 0.0f, 0xFF0000FF, kFillModeSolid);
		// 信息输出
		Novice::ScreenPrintf(
			20, 800,
			"(map[%d][%d]) = %d",
			int(playerWorldY),
			int(playerWorldX),
			map[int(playerWorldY)][int(playerWorldX)]
		);

		/// 描画処理ここまで

		Novice::EndFrame();

		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	Novice::Finalize();
	return 0;
}

#include <Novice.h>

const char kWindowTitle[] = "GC1B_03_ゴ_チュウ";

void Move(float& posX, float& posY, float& speed , char keys[256]);
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	Novice::Initialize(kWindowTitle, 1280, 720);
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	float playerX = 646.0f;
	float playerY = 320.0f;
	float playerSpeed = 15.0f;

	while (Novice::ProcessMessage() == 0) {
		Novice::BeginFrame();
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		// 更新処理
		Move(playerX,playerY,playerSpeed,keys);

		// 描画処理
		Novice::DrawEllipse((int)playerX, (int)playerY, 32, 32, 0.0f, WHITE, kFillModeSolid);

		Novice::EndFrame();

		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	Novice::Finalize();
	return 0;
}
void Move(float& posX, float& posY, float& speed, char keys[256]) {
	if (keys[DIK_W]) posY -= speed;
	if (keys[DIK_S]) posY += speed;
	if (keys[DIK_A]) posX -= speed;
	if (keys[DIK_D]) posX += speed;
}

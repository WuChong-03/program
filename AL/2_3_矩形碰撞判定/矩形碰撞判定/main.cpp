#include <Novice.h>
#include <math.h>
const char kWindowTitle[] = "GC1C_02_ゴ_チュウ";
typedef struct Vector2
{
	float x;
	float y;
}Vector2;
typedef struct Box
{
	Vector2 pos; // 位置
	Vector2 leftTop;
	Vector2 rightTop;
	Vector2 leftBottom;
	Vector2 rightBottom;
	float width;
	float height;
	Vector2 dir;
	Vector2 vec;
	float speed = 5.0f; // 移動速度
} Box;
// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);
	//初期化ここから
	Box player{
		.pos = {640.0f,620.0f},
		.width = 100.0f,
		.height = 100.0f,
		.speed = 5.0f,
	};
	Box target{
		.pos = {640.0f,360.0f},
		.width = 100.0f,
		.height = 100.0f,
	};
	int graphHandle1 = Novice::LoadTexture("./Resources/images/Circle.png");
	int graphHandle2 = Novice::LoadTexture("./Resources/images/CircleHit.png");
	int graphHandle1 = Novice::LoadTexture("./Resources/images/CircleW.png");
	int graphHandle2 = Novice::LoadTexture("./Resources/images/CircleWHit.png");
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
#pragma region 图片切换
		if (keys[DIK_SPACE])
		{

		}
#pragma endregion
#pragma region "move"
		player.dir.x = 0.0f;
		player.dir.y = 0.0f;
		if (keys[DIK_A]) {
			player.dir.x = -1.0f;
		}
		if (keys[DIK_D]) {
			player.dir.x = 1.0f;
		}
		if (keys[DIK_W]) {
			player.dir.y = -1.0f;
		}
		if (keys[DIK_S]) {
			player.dir.y = 1.0f;
		}
		float length = sqrtf(player.dir.x * player.dir.x + player.dir.y * player.dir.y);
		if (length != 0.0f) {
			player.dir.x /= length;
			player.dir.y /= length;
		}
		player.vec.x = player.dir.x * player.speed;
		player.vec.y = player.dir.y * player.speed;
		player.pos.x += player.vec.x;
		player.pos.y += player.vec.y;
#pragma endregion
#pragma region "顶点计算"
		//player
		player.leftTop.x = player.pos.x - player.width / 2;
		player.leftTop.y = player.pos.y - player.height / 2;

		player.leftBottom.x = player.pos.x - player.width / 2;
		player.leftBottom.y = player.pos.y + player.height / 2;

		player.rightTop.x = player.pos.x + player.width / 2;
		player.rightTop.y = player.pos.y - player.height / 2;

		player.rightBottom.x = player.pos.x + player.width / 2;
		player.rightBottom.y = player.pos.y + player.height / 2;

		//enemy
		target.leftTop.x = target.pos.x - target.width / 2;
		target.leftTop.y = target.pos.y - target.height / 2;

		target.leftBottom.x = target.pos.x - target.width / 2;
		target.leftBottom.y = target.pos.y + target.height / 2;

		target.rightTop.x = target.pos.x + target.width / 2;
		target.rightTop.y = target.pos.y - target.height / 2;

		target.rightBottom.x = target.pos.x + target.width / 2;
		target.rightBottom.y = target.pos.y + target.height / 2;
#pragma endregion	
#pragma region "sacle"
		if (keys[DIK_Q]){
			player.width++;
		}
		if (keys[DIK_E]){
			player.width--;
		}
		if (keys[DIK_Z]){
			player.height++;
		}
		if (keys[DIK_C]){
			player.height--;
		}

#pragma endregion	
		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		Novice::DrawQuad(
			int(player.leftTop.x), int(player.leftTop.y),
			int(player.rightTop.x), int(player.rightTop.y),
			int(player.leftBottom.x), int(player.leftBottom.y),
			int(player.rightBottom.x), int(player.rightBottom.y),
			0,		//剪裁起点
			0,
			32,		//剪裁范围	
			32,
			graphHandle1,
			WHITE
		);
		Novice::DrawQuad(
			int(target.leftTop.x), int(target.leftTop.y),
			int(target.rightTop.x), int(target.rightTop.y),
			int(target.leftBottom.x), int(target.leftBottom.y),
			int(target.rightBottom.x), int(target.rightBottom.y),
			0,		//剪裁起点
			0,
			32,		//剪裁范围	
			32,
			graphHandle1,
			WHITE
		);

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

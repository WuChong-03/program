#include <Novice.h>
#define _USE_MATH_DEFINES
#include <math.h>

const char kWindowTitle[] = "GC_1C_02_ゴ_チュウ";

//今回目標：構造体と列挙型をちゃんと利用し、scene、background、audioを分けてかき、読み、書き直し安くようにする
typedef enum {
	TITLE,
	PLAY,
	RESULT,
}SCENE;
typedef struct Vector2 {
	float x;
	float y;
} Vector2;
typedef	struct BgGr
{
	int grHandle;
	Vector2 position;
	float MoveSpeed;
}BgGr;


// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	//初期化ここから

	//sceneの初期化
	SCENE currentScene = TITLE;
	//Bgの初期化
	BgGr titleBggr = {
	Novice::LoadTexture("./images/title.png"),
	{0,0},
	10.0f,
	};
	BgGr titleBggrCopy = {
	Novice::LoadTexture("./images/title.png"),
	{0,-720},
	10.0f,
	};
	BgGr playBggr = {
	Novice::LoadTexture("./images/play.png"),
	{0,0},
	0.0f,
	};
	BgGr resultBggr = {
	Novice::LoadTexture("./images/result.png"),
	{0,0},
	0.0f,
	};
	//audioの初期化
	int titleBgmHandle = Novice::LoadAudio("./music/start.wav");
	int playBgmHandle = Novice::LoadAudio("./music/play.wav");
	int resultBgmHandle = Novice::LoadAudio("./music/result.wav");
	int titlePlayHandle = -1;
	int playPlayHandle = -1;
	int resultPlayHandle = -1;


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
		/// 
		switch (currentScene) {
		case TITLE:
			if (!Novice::IsPlayingAudio(titlePlayHandle)) {
				titlePlayHandle = Novice::PlayAudio(titleBgmHandle, false, 0.5f);
			}

			titleBggr.position.y += titleBggr.MoveSpeed;
			titleBggrCopy.position.y += titleBggr.MoveSpeed;
			if (titleBggr.position.y >= 720) {
				titleBggr.position.y = -720;
			}
			if (titleBggrCopy.position.y >= 720) {
				titleBggrCopy.position.y = -720;
			}

			if (keys[DIK_SPACE] && !preKeys[DIK_SPACE]) {
				currentScene = PLAY;
				Novice::StopAudio(titlePlayHandle);
			}
			break;

		case PLAY:
			if (!Novice::IsPlayingAudio(playPlayHandle)) {
				playPlayHandle = Novice::PlayAudio(playBgmHandle, false, 0.5f);
			}
			if (keys[DIK_SPACE] && !preKeys[DIK_SPACE]) {
				currentScene = RESULT;
				Novice::StopAudio(playPlayHandle);
			}
			break;
		case RESULT:
			if (!Novice::IsPlayingAudio(resultPlayHandle)) {
				resultPlayHandle = Novice::PlayAudio(resultBgmHandle, false, 0.5f);
			}
			if (keys[DIK_SPACE] && !preKeys[DIK_SPACE]) {
				currentScene = TITLE;
				Novice::StopAudio(resultPlayHandle);
				titleBggr.position.y = 0.0f;
				titleBggrCopy.position.y = -720.0f;
			}
			break;
		}
		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		/// 
		/// 
		switch (currentScene) {
		case TITLE:
			Novice::DrawSprite(
				int(titleBggr.position.x),
				int(titleBggr.position.y),
				titleBggr.grHandle,
				1,
				1,
				0.0f,
				WHITE);
			Novice::DrawSprite(
				int(titleBggrCopy.position.x),
				int(titleBggrCopy.position.y),
				titleBggrCopy.grHandle,
				1,
				1,
				0.0f,
				WHITE);
			break;
		case PLAY:
			Novice::DrawSprite(
				int(playBggr.position.x),
				int(playBggr.position.y),
				playBggr.grHandle,
				1,
				1,
				0.0f,
				WHITE);
			break;
		case RESULT:
			Novice::DrawSprite(
				int(resultBggr.position.x),
				int(resultBggr.position.y),
				resultBggr.grHandle,
				1,
				1,
				0.0f,
				WHITE);
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

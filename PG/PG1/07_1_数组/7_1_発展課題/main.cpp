#include <Novice.h>

const char kWindowTitle[] = "GC1C_02_ゴ_チュウ";

const int kBallCount = 10;
const float kBallRadius = 10.0f;

struct Ball {
    float posX;   // X座標
    float posY;   // Y座標
    float velX;   // X方向の速度
};

Ball balls[kBallCount];

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

    // ライブラリの初期化
    Novice::Initialize(kWindowTitle, 1280, 720);

    // キー入力結果を受け取る箱
    char keys[256] = { 0 };
    char preKeys[256] = { 0 };

    // ボールの初期化（位置と速度を代入）
    for (int i = 0; i < kBallCount; i++) {
        balls[i].posX = float(100 + i * 30);  // x座標：右下に斜めに並べる
        balls[i].posY = float(100 + i * 30);
        balls[i].velX = 4.0f;          // 初速：右方向
    }

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
        for (int i = 0; i < kBallCount; i++) {
            balls[i].posX += balls[i].velX;

            // 画面端に当たったら反射
            if (balls[i].posX - kBallRadius < 0 || balls[i].posX + kBallRadius > 1280) {
                balls[i].velX *= -1;
            }
        }
        ///
        /// ↑更新処理ここまで
        ///

        ///
        /// ↓描画処理ここから
        ///
        for (int i = 0; i < kBallCount; i++) {
            Novice::DrawEllipse(
                (int)balls[i].posX,
                (int)balls[i].posY,
                (int)kBallRadius,
                (int)kBallRadius,
                0.0f,
                0xFF0000FF,
                kFillModeSolid
            );
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

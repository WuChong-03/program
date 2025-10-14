#include <Novice.h>
#include <cmath>

struct Vector2 { float x, y; };

const char kWindowTitle[] = "GC1C_02_ゴ_チュウ";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

    // ライブラリの初期化
    Novice::Initialize(kWindowTitle, 1280, 720);

    // キー入力結果を受け取る箱
    char keys[256] = { 0 };
    char preKeys[256] = { 0 };

    Vector2 a = { 100, 100 };
    Vector2 b = { 1000, 400 };
    float   capsuleR = 60.0f;

    Vector2 c = { 500, 0 };
    float   circleR = 30.0f;
    float   moveSpeed = 5.0f;

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
        Vector2 input = { 0, 0 };
        if (keys[DIK_W]) input.y -= 1.0f;
        if (keys[DIK_S]) input.y += 1.0f;
        if (keys[DIK_A]) input.x -= 1.0f;
        if (keys[DIK_D]) input.x += 1.0f;
        float inLen = std::sqrt(input.x * input.x + input.y * input.y);
        if (inLen > 0.0f) {
            input.x /= inLen;  // ← normalize
            input.y /= inLen;
            c.x += input.x * moveSpeed;
            c.y += input.y * moveSpeed;
        }

        //---- カプセル  円 衝突判定 ----
        bool hit = false;

        // d = c - a
        Vector2 d = { c.x - a.x, c.y - a.y };

        // ba = b - a
        Vector2 ba = { b.x - a.x, b.y - a.y };

        // |ba|, e = normalize(ba)
        float baLen = std::sqrt(ba.x * ba.x + ba.y * ba.y);
        Vector2 e = { 0.0f, 0.0f };
        if (baLen > 0.0f) {
            e.x = ba.x / baLen;  // ← normalize(ba)
            e.y = ba.y / baLen;
        }

        // t = dot(d, e) / length(ba)
        float t = (d.x * e.x + d.y * e.y) / baLen;

        // t を 0～1 に clamp
        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;

        // f = (1 - t) * a + t * b （線形補間 LERP）
        Vector2 f;
        f.x = (1.0f - t) * a.x + t * b.x;
        f.y = (1.0f - t) * a.y + t * b.y;

        // distance = length(c - f)
        Vector2 cf = { c.x - f.x, c.y - f.y };
        float distance = std::sqrt(cf.x * cf.x + cf.y * cf.y);

        if (distance < circleR + capsuleR) hit = true;
        ///
        /// ↑更新処理ここまで
        ///

        ///
        /// ↓描画処理ここから
        ///
        // カプセル
        Novice::DrawLine((int)a.x, (int)a.y, (int)b.x, (int)b.y, 0xFFFFFFFF);
        Novice::DrawEllipse((int)a.x, (int)a.y, (int)capsuleR, (int)capsuleR, 0.0f, 0xFFFFFFFF, kFillModeSolid);
        Novice::DrawEllipse((int)b.x, (int)b.y, (int)capsuleR, (int)capsuleR, 0.0f, 0xFFFFFFFF, kFillModeSolid);


        int col = hit ? 0xFF0000FF : 0xFFFFFFFF;
        Novice::DrawEllipse((int)c.x, (int)c.y, (int)circleR, (int)circleR, 0.0f, col, kFillModeSolid);
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

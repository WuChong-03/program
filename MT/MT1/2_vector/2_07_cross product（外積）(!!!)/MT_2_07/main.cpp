#include <Novice.h>

const char kWindowTitle[] = "GC1C_02_ゴ_チュウ";

// ベクトル用の構造体
struct Vector2 {
    float x;
    float y;
};

// 円用の構造体
struct Circle {
    Vector2 center;
    float radius;
};

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

    Novice::Initialize(kWindowTitle, 1280, 720);

    char keys[256] = { 0 };
    char preKeys[256] = { 0 };

    // 線の始点と終点
    Vector2 point1 = { 100, 100 };
    Vector2 point2 = { 1000, 400 };

    // 移動する円B
    Circle circle;
    circle.center.x = 500;
    circle.center.y = 300;
    circle.radius = 30;

    while (Novice::ProcessMessage() == 0) {
        Novice::BeginFrame();

        memcpy(preKeys, keys, 256);
        Novice::GetHitKeyStateAll(keys);

        /// ↓更新処理ここから

        // キー操作で円を動かす
        float speed = 5.0f;
        if (keys[DIK_UP]) { circle.center.y -= speed; }
        if (keys[DIK_DOWN]) { circle.center.y += speed; }
        if (keys[DIK_LEFT]) { circle.center.x -= speed; }
        if (keys[DIK_RIGHT]) { circle.center.x += speed; }

        // 外積計算
        float ABx = point2.x - point1.x;
        float ABy = point2.y - point1.y;
        float ACx = circle.center.x - point1.x;
        float ACy = circle.center.y - point1.y;

        float cross = ABx * ACy - ABy * ACx;

        // 外積によって色を決める
        unsigned int color = WHITE;
        if (cross < 0) {
            color = BLUE;
        }

        /// ↑更新処理ここまで

        /// ↓描画処理ここから
        Novice::DrawLine((int)point1.x, (int)point1.y, (int)point2.x, (int)point2.y, WHITE); 
        Novice::DrawEllipse((int)circle.center.x, (int)circle.center.y, (int)circle.radius, (int)circle.radius, 0.0f, color, kFillModeSolid);
        /// ↑描画処理ここまで

        Novice::EndFrame();

        if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
            break;
        }
    }

    Novice::Finalize();
    return 0;
}

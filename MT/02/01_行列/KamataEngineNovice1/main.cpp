#include <Novice.h>

const char kWindowTitle[] = "GC1C_02_ゴ_チュウ";

//==================== 结构体定义 ====================//
struct Matrix2x2
{
    float m[2][2];
};

struct Vector2
{
    float x;
    float y;
};

//==================== 打印函数工具 ====================//
static const int kRowHeight = 20;
static const int kColumnWidth = 44;

void MatrixScreenPrint(int x, int y, const Matrix2x2& matrix) {
    for (int row = 0; row < 2; ++row) {
        for (int column = 0; column < 2; ++column) {
            Novice::ScreenPrintf(
                x + column * kColumnWidth,
                y + row * kRowHeight,
                "%.02f",
                matrix.m[row][column]
            );
        }
    }
}

void VectorScreenPrint(int x, int y, const Vector2& vector) {
    Novice::ScreenPrintf(x, y, "%.02f", vector.x);
    Novice::ScreenPrintf(x + kColumnWidth, y, "%.02f", vector.y);
}

//==================== 矩阵基本运算 ====================//
Matrix2x2 Add(const Matrix2x2& a, const Matrix2x2& b) {
    Matrix2x2 r{};
    r.m[0][0] = a.m[0][0] + b.m[0][0];
    r.m[0][1] = a.m[0][1] + b.m[0][1];
    r.m[1][0] = a.m[1][0] + b.m[1][0];
    r.m[1][1] = a.m[1][1] + b.m[1][1];
    return r;
}

Matrix2x2 Subtract(const Matrix2x2& a, const Matrix2x2& b) {
    Matrix2x2 r{};
    r.m[0][0] = a.m[0][0] - b.m[0][0];
    r.m[0][1] = a.m[0][1] - b.m[0][1];
    r.m[1][0] = a.m[1][0] - b.m[1][0];
    r.m[1][1] = a.m[1][1] - b.m[1][1];
    return r;
}

Matrix2x2 Multiply(const Matrix2x2& a, const Matrix2x2& b) {
    Matrix2x2 r{};
    r.m[0][0] = a.m[0][0] * b.m[0][0] + a.m[0][1] * b.m[1][0];
    r.m[0][1] = a.m[0][0] * b.m[0][1] + a.m[0][1] * b.m[1][1];
    r.m[1][0] = a.m[1][0] * b.m[0][0] + a.m[1][1] * b.m[1][0];
    r.m[1][1] = a.m[1][0] * b.m[0][1] + a.m[1][1] * b.m[1][1];
    return r;
}

Vector2 Multiply(const Vector2& v, const Matrix2x2& m) {
    Vector2 r{};
    r.x = v.x * m.m[0][0] + v.y * m.m[1][0];
    r.y = v.x * m.m[0][1] + v.y * m.m[1][1];
    return r;
}

//==================== WinMain ====================//
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
    //测试数据初始化
    Matrix2x2 m1{ { {1.0f, 2.0f}, {3.0f, 4.0f} } };
    Matrix2x2 m2{ { {5.0f, 6.0f}, {7.0f, 8.0f} } };
    Vector2 v{ 10.0f, 20.0f };

    Matrix2x2 resultAdd = Add(m1, m2);
    Matrix2x2 resultSubtract = Subtract(m1, m2);
    Matrix2x2 resultMultiply = Multiply(m1, m2);
    Vector2 resultVectorMultiply = Multiply(v, m1);

    Novice::Initialize(kWindowTitle, 1280, 720);

    char keys[256] = { 0 };
    char preKeys[256] = { 0 };

    while (Novice::ProcessMessage() == 0) {
        Novice::BeginFrame();

        memcpy(preKeys, keys, 256);
        Novice::GetHitKeyStateAll(keys);

        //============== 画面显示 ==============//
        MatrixScreenPrint(0, kRowHeight * 0, resultAdd);
        MatrixScreenPrint(0, kRowHeight * 2 + 10, resultSubtract);
        MatrixScreenPrint(0, kRowHeight * 4 + 20, resultMultiply);
        VectorScreenPrint(0, kRowHeight * 6 + 30, resultVectorMultiply);

        Novice::EndFrame();

        if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
            break;
        }
    }

    Novice::Finalize();
    return 0;
}

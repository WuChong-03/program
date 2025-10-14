#include <Novice.h>

const char kWindowTitle[] = "GC1B_03_ゴ_チュウ";

struct Matrix2x2
{
    float m[2][2];
};
struct Vector2
{
    float x;
    float y;
};

static const int kRowHeight = 20;
static const int kColumnWidth = 44;

//Print関数
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

//Print関数
void VectorScreenPrint(int x, int y, const Vector2& vector) {
    Novice::ScreenPrintf(x, y, "%.02f", vector.x);
    Novice::ScreenPrintf(x + kColumnWidth, y, "%.02f", vector.y);
}

//加
Matrix2x2 Matrix2x2Add(Matrix2x2 matrix1, Matrix2x2 matrix2) {
    Matrix2x2 result{};
    result.m[0][0] = matrix1.m[0][0] + matrix2.m[0][0];
    result.m[0][1] = matrix1.m[0][1] + matrix2.m[0][1];
    result.m[1][0] = matrix1.m[1][0] + matrix2.m[1][0];
    result.m[1][1] = matrix1.m[1][1] + matrix2.m[1][1];
    return result;
}

//減
Matrix2x2 Matrix2x2Subtract(Matrix2x2 matrix1, Matrix2x2 matrix2) {
    Matrix2x2 result{};
    result.m[0][0] = matrix1.m[0][0] - matrix2.m[0][0];
    result.m[0][1] = matrix1.m[0][1] - matrix2.m[0][1];
    result.m[1][0] = matrix1.m[1][0] - matrix2.m[1][0];
    result.m[1][1] = matrix1.m[1][1] - matrix2.m[1][1];
    return result;
}

//積
Matrix2x2 Matrix2x2Multiply(Matrix2x2 matrix1, Matrix2x2 matrix2) {
    Matrix2x2 result{};
    result.m[0][0] = matrix1.m[0][0] * matrix2.m[0][0] + matrix1.m[0][1] * matrix2.m[1][0];
    result.m[0][1] = matrix1.m[0][0] * matrix2.m[0][1] + matrix1.m[0][1] * matrix2.m[1][1];
    result.m[1][0] = matrix1.m[1][0] * matrix2.m[0][0] + matrix1.m[1][1] * matrix2.m[1][0];
    result.m[1][1] = matrix1.m[1][0] * matrix2.m[0][1] + matrix1.m[1][1] * matrix2.m[1][1];
    return result;
}

//ベクトルと行列の積
Vector2 Vector2Multiply(Vector2 vector, Matrix2x2 matrix) {
    Vector2 result{};
    result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0];
    result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1];
    return result;
}
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

    // 初期化
    Matrix2x2 matrix1 = { {{1.0f, 2.0f}, {3.0f, 4.0f}} };
    Matrix2x2 matrix2 = { {{5.0f, 6.0f}, {7.0f, 8.0f}} };
    Vector2 vector = { 10.0f, 20.0f };
    Matrix2x2 resultAdd = Matrix2x2Add(matrix1, matrix2);
    Matrix2x2 resultSubtract = Matrix2x2Subtract(matrix1, matrix2);
    Matrix2x2 resultMultiply = Matrix2x2Multiply(matrix1, matrix2);
    Vector2 resultVectorMultiply = Vector2Multiply(vector, matrix1);

    Novice::Initialize(kWindowTitle, 1280, 720);

    char keys[256] = { 0 };
    char preKeys[256] = { 0 };

    while (Novice::ProcessMessage() == 0) {
        Novice::BeginFrame();

        memcpy(preKeys, keys, 256);
        Novice::GetHitKeyStateAll(keys);

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

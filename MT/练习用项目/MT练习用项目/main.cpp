#include <Novice.h>
#include <cmath>

const char kWindowTitle[] = "GC1B_03_ゴ_チュウ";

struct Vector2 {
	float x;
	float y;
};

struct Matrix2x2 {
	float m[2][2];
};
Matrix2x2 MakeRotateMatrix(float theta) {
	Matrix2x2 result{};
	float c = cosf(theta);
	float s = sinf(theta);
	result.m[0][0] = c;  result.m[0][1] = -s;
	result.m[1][0] = s;  result.m[1][1] = c;
	return result;
}

Vector2 Multiply(Vector2 v, Matrix2x2 m) {
	Vector2 result{};
	result.x = v.x * m.m[0][0] + v.y * m.m[0][1];
	result.y = v.x * m.m[1][0] + v.y * m.m[1][1];
	return result;
}


// 世界座標 → スクリーン座標
float ToScreenY(float worldY) {
	return 720 - worldY; 
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	Novice::Initialize(kWindowTitle, 1280, 720);

	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	float theta = 0.0f; 

	while (Novice::ProcessMessage() == 0) {
		Novice::BeginFrame();
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);


		theta += 0.02f;

		Vector2 center = { 400.0f, 100.0f };

		Vector2 leftTop = { -100, +50 };
		Vector2 rightTop = { +100, +50 };
		Vector2 leftBottom = { -100, -50 };
		Vector2 rightBottom = { +100, -50 };


		Matrix2x2 rotateMatrix = MakeRotateMatrix(theta);

		Vector2 rotatedLeftTop = Multiply(leftTop, rotateMatrix);
		Vector2 rotatedRightTop = Multiply(rightTop, rotateMatrix);
		Vector2 rotatedLeftBottom = Multiply(leftBottom, rotateMatrix);
		Vector2 rotatedRightBottom = Multiply(rightBottom, rotateMatrix);

		rotatedLeftTop.x += center.x;
		rotatedRightTop.x += center.x;
		rotatedLeftBottom.x += center.x;
		rotatedRightBottom.x += center.x;

		rotatedLeftTop.y += center.y;
		rotatedRightTop.y += center.y;
		rotatedLeftBottom.y += center.y;
		rotatedRightBottom.y += center.y;

		// 描画処理
		Novice::DrawQuad(
			static_cast<int>(rotatedLeftTop.x),
			static_cast<int>(ToScreenY(rotatedLeftTop.y)),
			static_cast<int>(rotatedRightTop.x),
			static_cast<int>(ToScreenY(rotatedRightTop.y)),
			static_cast<int>(rotatedLeftBottom.x),
			static_cast<int>(ToScreenY(rotatedLeftBottom.y)),
			static_cast<int>(rotatedRightBottom.x),
			static_cast<int>(ToScreenY(rotatedRightBottom.y)),
			0, 0, 200, 100,
			Novice::LoadTexture("white1x1.png"),
			0xFFFFFFFF);

		Novice::EndFrame();

		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) break;
	}

	Novice::Finalize();
	return 0;
}

#include <Novice.h>

const char kWindowTitle[] = "GC_1C_02_�S_�`���E";

// Windows�A�v���ł̃G���g���[�|�C���g(main�֐�)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ���C�u�����̏�����
	Novice::Initialize(kWindowTitle, 1280, 720);

	// �L�[���͌��ʂ��󂯎�锠
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };



	int isAlive = 1;
	int timer = 120;


	int graphHandle1;
	int graphHandle2;
	graphHandle1 = Novice::LoadTexture("./image/explosion.png");
	graphHandle2 = Novice::LoadTexture("./image/pien.png");
	// �E�B���h�E�́~�{�^�����������܂Ń��[�v
	while (Novice::ProcessMessage() == 0) {
		// �t���[���̊J�n
		Novice::BeginFrame();

		// �L�[���͂��󂯎��
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ���X�V������������
		///

		if (keys[DIK_SPACE] && !preKeys[DIK_SPACE])
		{
			isAlive = 0;
		}

		if (isAlive == 0)
		{
			timer -= 1;

			if (timer == 0)
			{
				timer = 120;
				isAlive = 1;
			}
		}

		///
		/// ���X�V���������܂�
		///

		///
		/// ���`�揈����������
		///
		Novice::DrawBox(0, 0, 1280, 720, 0.0f, WHITE, kFillModeSolid);
		Novice::DrawBox(0, 600, 1280, 120, 0.0f, BLACK, kFillModeSolid);
		Novice::ScreenPrintf(30, 650, "isAlive = %d", isAlive);
		Novice::ScreenPrintf(230, 650, "timer = %d", timer);
		Novice::ScreenPrintf(500, 630, "Press Space wo Exprode Pien");





		if (isAlive != 0)
		{
			Novice::DrawSprite(500, 200, graphHandle2, 1, 1, 0.0f, WHITE);
		}
		else if (isAlive == 0 && timer > 90)
		{
			Novice::DrawSprite(500, 200, graphHandle1, 1, 1, 0.0f, WHITE);
		}

		///
		/// ���`�揈�������܂�
		///

		// �t���[���̏I��
		Novice::EndFrame();

		// ESC�L�[�������ꂽ�烋�[�v�𔲂���
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ���C�u�����̏I��
	Novice::Finalize();
	return 0;
}
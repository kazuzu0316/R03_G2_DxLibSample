#include "DxLib.h" //DxLib���g�����͕K�v

//�}�N����`
#define GAME_TITLE  "�Q�[���^�C�g��"  //�Q�[���^�C�g��
#define GAME_WIDTH  1280			  //�Q�[����ʂ̕�(�E�B�h�X)
#define GAME_HEIGHT 720				  //�Q�[����ʂ̍���(�n�C�g)
#define GAME_COLOR  32				  //�Q�[���̐F��

#define GAME_ICON_ID   333				  //�Q�[����ICON��ID

#define GAME_WINDOW_BAR 0				//�E�B���h�E�o�[�̎��

// �v���O������ WinMain ����n�܂�܂�
//Windows�̃v���O���~���O���@ = (WinAPI)�œ����Ă���
//DxLib�́ADirecttX�Ƃ����A�Q�[���v���O���~���O���ȒP�Ɏg����d�g��
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetOutApplicationLogValidFlag(FALSE);
	ChangeWindowMode(TRUE);  //�E�B���h�E���[�h�ɐݒ�
	SetMainWindowText(GAME_TITLE);  //�E�B���h�E�̃^�C�g���̕���
	SetGraphMode(GAME_WIDTH, GAME_HEIGHT, GAME_COLOR);  //�E�B���h�E�̉𑜓x��ݒ�
	SetWindowSize(GAME_WIDTH, GAME_HEIGHT);  //�E�B���h�E�̑傫����ݒ�
	SetBackgroundColor(255, 255, 255);  //�f�t�H���g�̔w�i�̐F
	SetWindowIconID(GAME_ICON_ID);  //�A�C�R���t�@�C���̓Ǎ�
	SetWindowStyleMode(GAME_WINDOW_BAR);  //�E�B���h�E�o�[�̏��
	SetWaitVSyncFlag(TRUE);  //�f�B�X�v���C�̐���������L���ɂ���
	SetAlwaysRunFlag(TRUE);  //�E�B���h�E�������ƃA�N�e�B�u�ɂ���


	if (DxLib_Init() == -1)		// �c�w���C�u��������������
	{
		return -1;			// �G���[���N�����璼���ɏI��
	}

	//�ʒu
	int X = GAME_WIDTH / 2;  //���̒��S
	int Y = GAME_HEIGHT / 2;  //�c�̒��S


	int width = 32;
	int height = 32;  //����

	//�~�̔��a�����߂�
	int radius = 100;

	int Speed = 5;
	int XSpeed = Speed;
	int YSpeed = Speed;

	//�_�u���o�b�t�@�����O�L����
	SetDrawScreen(DX_SCREEN_BACK);

	//�������[�v
	while (1)
	{
		//��������̃L�[�������ꂽ�Ƃ�
		if (CheckHitKeyAll() != 0)  //0�̂Ƃ��ɁA�����L�[�������ꂽ
		{
			break;  //�������[�v�𔲂���
		}

		//���b�Z�[�W���󂯎�葱����
		if (ProcessMessage() != 0)  //-1�̂Ƃ��A�G���[��E�B���h�E������ꂽ
		{
			break;  //�������[�v�𔲂���
		}

		//��ʂ���������
		if (ClearDrawScreen() != 0) { break; }

		/*
		//�l�p��`��
		DrawBox(
			X, Y, X + width, Y + height,
			GetColor(255, 0, 0),  //�F�̎擾
			TRUE				  //�h��Ԃ�?
		);
		*/

		//�~�̕`��
		DrawCircle(
			X, Y, radius,
			GetColor(0, 255, 0),
			FALSE, 5
		);



		X += XSpeed;  //�l�p�̈ʒu���E�ɂ��炷

		//����@�@�l�p���΂߉E���ɓ�������!
		Y += YSpeed;

		//����A�@�l�p����ʂ̒[�ɂ�����A�ړ���������𔽓]�����Ă݂悤
		if (X - radius < 0 || X + radius > GAME_WIDTH)  //��ʂ̉�����o����
		{
			//�ړ����x�̕����𔽓]������
			//+1�Ȃ�A-1���� / -1�Ȃ�΁A+1�ɂ���
			XSpeed = -XSpeed;  //�ړ���������𔽓]

			//�ǂɓ��������瑬���Ȃ�
			if (XSpeed > 0) { XSpeed += 10; }
			else if (XSpeed < 0) { XSpeed -= 10; }
		}

		if (Y - radius < 0 || Y + radius > GAME_HEIGHT)  //��ʂ̉�����o����
		{
			//�ړ����x�̕����𔽓]������
			//+1�Ȃ�A-1���� / -1�Ȃ�΁A+1�ɂ���
			YSpeed = -YSpeed;  //�ړ���������𔽓]

			//�ǂɓ��������瑬���Ȃ�
			if (YSpeed > 0) { YSpeed += 10; }
			else if (YSpeed < 0) { YSpeed -= 10; }
		}



		ScreenFlip();  //�_�u���o�b�t�@�����O������ʂ�`��

	}

	DxLib_End();				// �c�w���C�u�����g�p�̏I������

	return 0;				// �\�t�g�̏I�� 
}
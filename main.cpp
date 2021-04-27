#include "DxLib.h" //DxLib���g�����͕K�v

//�}�N����`
#define GAME_TITLE  "�Q�[���^�C�g��"  //�Q�[���^�C�g��
#define GAME_WIDTH  1280			  //�Q�[����ʂ̕�(�E�B�h�X)
#define GAME_HEIGHT 720				  //�Q�[����ʂ̍���(�n�C�g)
#define GAME_COLOR  32				  //�Q�[���̐F��

#define GAME_ICON_ID   333				  //�Q�[����ICON��ID

// �v���O������ WinMain ����n�܂�܂�
//Windows�̃v���O���~���O���@ = (WinAPI)�œ����Ă���
//DxLib�́ADirecttX�Ƃ����A�Q�[���v���O���~���O���ȒP�Ɏg����d�g��
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	ChangeWindowMode(TRUE);  //�E�B���h�E���[�h�ɐݒ�
	SetMainWindowText("GAME_TITLE");  //�E�B���h�E�̃^�C�g���̕���
	SetGraphMode(GAME_WIDTH, GAME_HEIGHT, GAME_COLOR);  //�E�B���h�E�̉𑜓x��ݒ�
	SetWindowSize(GAME_WIDTH, GAME_HEIGHT);  //�E�B���h�E�̑傫����ݒ�
	SetBackgroundColor(255, 255, 255);  //�f�t�H���g�̔w�i�̐F

	if (DxLib_Init() == -1)		// �c�w���C�u��������������
	{
		return -1;			// �G���[���N�����璼���ɏI��
	}

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
	}

	DxLib_End();				// �c�w���C�u�����g�p�̏I������

	return 0;				// �\�t�g�̏I�� 
}